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

enum ReferenceType
{
    ReferenceSwitch,
    NegativeLimitSwitch,
    PositiveLimitSwitch,
};


int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

void StartReferencing (int iD, std::string axis, ReferenceType referenceType);
void SetServoState (int ID, std::string axes, BOOL state);
void ReportError (int iD);
