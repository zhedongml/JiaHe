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

    This sample connects to a two-axes controller and references both axes.
    Afterwards, it moves the two axes forward and backward repeatedly.
    Stage has to be correctly configured i.e. by PIMikroMove and referenced i.e. by the reference sample.

**/

#include "TwoAxesSample.h"
#include <iostream>
#include <time.h>
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

        SetServoState (iD, AxesIdentifier, SERVO_ON);

        std::vector<double> minPosLimits = GetMinPositionLimit(iD, AxesIdentifier);
        std::vector<double> maxPosLimits = GetMaxPositionLimit(iD, AxesIdentifier);
        std::vector<double> travelRange = CalculateTravelRange(minPosLimits, maxPosLimits);

        MoveAbsoluteToNearMinimum (iD, AxesIdentifier, minPosLimits, travelRange);
        ShadowMoveAxes (iD, AxesIdentifier, maxPosLimits, travelRange);
        SetServoState (iD, AxesIdentifier, SERVO_OFF);

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

std::vector<double> GetCurrentPosition(int ID, std::string axes)
{
    double currentPositions[NUMBER_OF_AXES];

    if (!Hydra_qPOS (ID, axes.c_str (), currentPositions))
    {
        throw std::runtime_error ("Unable to query current position.");
    }

    return std::vector<double> (currentPositions, currentPositions + NUMBER_OF_AXES);
}

std::vector<double> GetMinPositionLimit(int ID, std::string axes)
{
    double minPosLimit[NUMBER_OF_AXES];

    if (!Hydra_qTMN(ID, axes.c_str(), minPosLimit))
    {
        throw std::runtime_error("Unable to get minimum position limit.");
    }

    return std::vector<double> (minPosLimit, minPosLimit + NUMBER_OF_AXES);
}

std::vector<double> GetMaxPositionLimit(int ID, std::string axes)
{
    double maxPosLimit[NUMBER_OF_AXES];

    if (!Hydra_qTMX(ID, axes.c_str(), maxPosLimit))
    {
        throw std::runtime_error("Unable to get maximum position limit.");
    }

    return std::vector<double> (maxPosLimit, maxPosLimit + NUMBER_OF_AXES);

}

std::vector<double> CalculateTravelRange(std::vector<double> minPosLimits, std::vector<double> maxPosLimits)
{
    double travelRange[NUMBER_OF_AXES];

    for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
    {
        travelRange[idx] = std::abs (maxPosLimits.at(idx)) + std::abs (minPosLimits.at(idx));
    }

    return std::vector<double> (travelRange, travelRange + NUMBER_OF_AXES);
}

bool IsNotNearLimit(std::string axes, std::vector<double> maxPositionLimits, std::vector<double> currentPositions)
{
    bool isNotNearLimit = true;

    for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
    {
        isNotNearLimit = isNotNearLimit && maxPositionLimits.at(idx) * 0.75 > currentPositions.at(idx);
    }

    return isNotNearLimit;
}

void MoveAbsoluteToNearMinimum(int ID, std::string axes, std::vector<double> minPosLimits, std::vector<double> travelRange)
{
    double targetPositions[NUMBER_OF_AXES];

    for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
    {
        targetPositions[idx] = minPosLimits.at(idx) + (travelRange.at(idx) * 0.1);
    }

    if (!Hydra_MOV (ID, axes.c_str (), targetPositions))
    {
        throw std::runtime_error ("Unable to move axes near minimum position.");
    }
}

bool ShadowMoveAxes (int ID, std::string axes, std::vector<double> maxPosLimits, std::vector<double> travelRange)
{
    std::vector<double> currentPositions = GetCurrentPosition (ID, axes);
    bool isNotNearLimit = IsNotNearLimit (axes, maxPosLimits, currentPositions);
    int motionsCounter = 0;
    double currentDistance = 2.0;

    std::vector<std::string> axesList;
    axesList.push_back(axes.substr(0, 1));
    axesList.push_back(axes.substr(axes.size() - 1, 1));

    while (isNotNearLimit)
    {
        currentPositions = GetCurrentPosition (ID, axes);
        isNotNearLimit = IsNotNearLimit (axes, maxPosLimits, currentPositions);

        if (motionsCounter % 2 != 0)
        {
            currentDistance *= -1;
        }
        else
        {
            currentDistance += 0.05 * -1;
        }

        for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
        {
            double targetDistance = (travelRange[idx] * 0.01) + currentDistance;
            std::string current_axis = axesList.at(idx);

            std::cout << "Moving axes " << current_axis << " for " << targetDistance << " units" << std::endl;

            if (!Hydra_MVR (ID, current_axis.c_str (), &targetDistance))
            {
                std::string msg ("MVR failed. Current axes: ");
                msg += current_axis;
                msg += "; Value: ";
                msg += std::to_string ( (long long)targetDistance);

                throw std::runtime_error (msg.c_str ());
            }

            WaitForMotionDone (ID, current_axis);
            Sleep (30);
        }

        motionsCounter++;
    }

    return true;
}

bool WaitForMotionDone (int ID, std::string axes)
{
    BOOL isMoving = TRUE;

    while (isMoving)
    {
        if (!Hydra_IsMoving (ID, axes.c_str (), &isMoving))
        {
            throw std::runtime_error ("Unable to retrieve motion state.");
        }

        Sleep (100);
    }

    return true;
}

void SetServoState (int ID, std::string axes, BOOL state)
{

    BOOL servoState[NUMBER_OF_AXES] = {state, state};

    if (!Hydra_SVO (ID, axes.c_str (), servoState))
    {
        throw std::runtime_error ("SVO failed. Exiting");
    }

    if (!Hydra_qSVO (ID,axes.c_str (), servoState) )
    {
        for (int idx = 0; idx < NUMBER_OF_AXES; idx++)
        {
            std::cout << "Turned servo for axes " << idx+1 << (servoState[idx] ? "on" : "off") << std::endl;
        }
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
