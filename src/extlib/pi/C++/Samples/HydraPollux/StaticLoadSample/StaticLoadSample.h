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

#pragma once

#include <string>
#include <iostream>


enum ConnectionType
{
    RS232,
    TCPIP,
};

int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void PrintControllerIdentification (int iD);
void CloseConnectionWithComment (int iD, std::string comment);

void ReportError (int iD);
