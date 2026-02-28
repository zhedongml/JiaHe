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

    This sample shows how to load the PI_HYDRAPOLLUX_GCS2_DLL and its methods dynamically,
    connect to a controller using a connection dialog and reads the device identification.

**/

#pragma once

#include <windows.h>
#include <string>
#include <iostream>

#include <shlobj.h>


// function pointer definitions
typedef BOOL (WINAPI *FP_ConnectRS232)( int , int);
typedef int  (WINAPI *FP_ConnectTCPIP)(const char* szHostname, int port);
typedef BOOL (WINAPI *FP_CloseConnection)( int );
typedef BOOL (WINAPI *FP_qIDN)( int , char*, int);
typedef BOOL (WINAPI *FP_GcsCommandset)( int , char*);
typedef int ( WINAPI *FP_GetError)(int ID);
typedef BOOL (WINAPI *FP_TranslateError) (int errNr, char* szBuffer, int iBufferSize);

// Function Pointer Variables
FP_ConnectRS232 pConnectRS232;
FP_ConnectTCPIP pConnectTcpip;
FP_qIDN pqIDN;
FP_GetError pGetError;
FP_CloseConnection pCloseConnection;
FP_TranslateError pTranslateError;

static const std::string GCS_TRANSLATOR_SUB_DIRECTORY ("\\PI\\GCSTranslator\\");
static const std::string GCS_DLLNAME ("PI_HYDRAPOLLUX_GCS2_DLL.dll");
static const std::string GCS_FUNC_PREFIX ("Hydra_");

enum ConnectionType
{
    RS232,
    TCPIP,
};


HINSTANCE OpenDLL ();
int OpenConnection (ConnectionType type, std::string hostName = "localhost");
void CloseConnectionWithComment (int iD, std::string comment);

std::string GetPathToDLL ();
void ReportError (int iD);
