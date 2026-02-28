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

    This sample connects to a controller using a RS232 connection or a TCP/IP connection.
    It shows how to determine min/max position limits, and how to do absolute and relative moves.
    The stage has to be referenced before starting this sample. Therefore use PIMikroMove or the reference sample.

**/

#pragma once

#include <windows.h>
#include <string>
#include <vector>




const BOOL SERVO_ON = TRUE;
const BOOL SERVO_OFF = FALSE;


enum ConnectionType
{
    RS232,
    TCPIP,
};


int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

double GetMinPositionLimit (int ID, std::string axis);
double GetMaxPositionLimit (int ID, std::string axis);
bool WaitForMotionDone (int ID, std::string axis);
void SetServoState (int ID, std::string axis, BOOL state);
void ReportError (int iD);
