/**
    (c)2015-2020 Physik Instrumente (PI) GmbH & Co. KG
    Software products that are provided by PI are subject to the General Software License Agreement of
    Physik Instrumente (PI) GmbH & Co. KG and may incorporate and/or make use of third-party software components.
    For more information, please read the General Software License Agreement and the Third Party Software Note linked below.

    General Software License Agreement :
    http://www.physikinstrumente.com/download/EULA_PhysikInstrumenteGmbH_Co_KG.pdf
    Third Party Software Note :
    http://www.physikinstrumente.com/download/TPSWNote_PhysikInstrumenteGmbH_Co_KG.pdf
**/

/**
    !! MOTION WARNING !!

    This sample connects to a controller using a RS232 connection or TCP/IP connection, and references the stage.
    The stage is either referencing with the reference command FRF, FNL or FPL command according to the stage type and user choice.
    The controller and stage have to be initialized with PIMikroMove before starting this sample.

**/


#include <iostream>
#include <time.h>

#include "ReferenceSample.h"
#include "PI_HydraPollux_GCS2_DLL.h"

int main ()
{
    int iD = OpenConnection (RS232);

    if (iD < 0)
    {
        std::cout << "Unable to connect.";
        return 1;
    }

    try
    {
        PrintControllerIdentification (iD);
        const std::string axis = "1";

        SetServoState (iD, axis, SERVO_ON);

        StartReferencing (iD, axis, NegativeLimitSwitch);

        // Wait until the reference move is done.
        BOOL bRefDone = FALSE;

        while (!bRefDone)
        {
            if (!Hydra_IsControllerReady (iD, &bRefDone))
            {
                throw std::runtime_error ("Unable to retrieve controller ready state.");
            }
        }

        BOOL bReferenced;

        // check if axis was referenced
        if (!Hydra_qFRF (iD, axis.c_str (), &bReferenced))
        {
            throw std::runtime_error ("Unable to retrieve referenced state.");
        }

        if (!bReferenced)
        {
            std::cout << "Axis " << axis << " is not referenced." << std::endl;
        }
        else
        {
            std::cout << "Axis " << axis << " is referenced." << std::endl;
        }

        SetServoState (iD, axis, SERVO_OFF);
        Hydra_CloseConnection (iD);
        std::cout << "Closing connection." << std::endl;
    }
    catch (std::runtime_error e)
    {
        CloseConnectionWithComment (iD, e.what ());
        return 1;
    }

    return 0;
}

int OpenConnection (ConnectionType type, std::string hostName)
{
    int iD = 0;
    std::string connType;

    switch (type)
    {
        case RS232:
            iD = Hydra_ConnectRS232 (1, 115200);
            connType = "RS232";
            break;
        case TCPIP:
            iD = Hydra_ConnectTCPIP (hostName.c_str (), 50000);
            connType = "TCPIP";
            break;
    }

    if (iD < 0)
    {
        ReportError (iD);
        std::cout << "Connect " << connType << ": ERROR" << std::endl;
    }

    return iD;
}

void PrintControllerIdentification (int iD)
{
    char szIDN[200];

    if (!Hydra_qIDN (iD, szIDN, 199))
    {
        std::runtime_error ("qIDN failed. Exiting.");
    }
    std::cout << "qIDN returned: " << szIDN << std::endl;
}

void CloseConnectionWithComment (int iD, std::string comment)
{
    std::cout << comment << std::endl;

    ReportError (iD);
    Hydra_CloseConnection (iD);
}

void SetServoState (const int ID, const std::string axes, BOOL state)
{
    if (!Hydra_SVO (ID, axes.c_str (), &state))
    {
        throw std::runtime_error ("SVO failed. Exiting");
    }

    std::cout << "Turned servo " << (state ? "on" : "off") << std::endl;
}

void StartReferencing (int iD, std::string axis, ReferenceType referenceType)
{

    switch (referenceType)
    {
        case NegativeLimitSwitch:
        {
            if (!Hydra_FNL (iD, axis.c_str ()))
            {
                throw std::runtime_error ("Unable to start referencing to negative limit switch.");
            }
            break;
        }
        case PositiveLimitSwitch:
        {
            if (!Hydra_FPL (iD, axis.c_str ()))
            {
                throw std::runtime_error ("Unable to start referencing to positive limit switch.");
            }
            break;
        }
        case ReferenceSwitch:
        {
            if (!Hydra_FPL (iD, axis.c_str ()))
            {
                throw std::runtime_error ("Unable to start referencing to reference switch.");
            }
            break;
        }
        default:
            throw std::runtime_error ("Unknown reference type.");
    }

}

void ReportError (int iD)
{
    int err = Hydra_GetError (iD);
    char szErrMsg[300];

    if (Hydra_TranslateError (err, szErrMsg, 299))
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}
