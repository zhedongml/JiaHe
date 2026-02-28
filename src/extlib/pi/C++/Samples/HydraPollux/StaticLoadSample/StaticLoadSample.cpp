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

    This sample shows how to load the PI_HYDRA_GCS2_DLL and its methods statically,
    connect to a controller using a RS232 Connection or TCP/IP Connection and reads the device identification.
    Lib and dll files have to exist in ../../../API/, have a look at the ReadMe.txt
    for specific paths.

**/

#include <Windows.h>
#include "StaticLoadSample.h"
#include "PI_HydraPollux_GCS2_DLL.h"



int main ()
{
    int iD = OpenConnection (RS232);

    if (iD < 0)
    {
        std::cout << "Unable to connect";
        return 1;
    }

    try
    {
        PrintControllerIdentification (iD);
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

void ReportError (int iD)
{
    int err = Hydra_GetError (iD);
    char szErrMsg[300];

    if (Hydra_TranslateError (err, szErrMsg, 299))
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}
