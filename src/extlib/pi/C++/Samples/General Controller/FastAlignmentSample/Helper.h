#pragma once

#include <windows.h>
#include "Declarations.h"
#include "INIReader.h"


//Configuration
BOOL GetIniFile (const char* FileName, IniConfs&, StringScans&);

//Connectivität
BOOL ControllerConnect (IniConfs Conf, int* pID, ConnectMap& Connections);

//Prepair
void ConvertStringScansToFDRScans (std::map <std::string, StringScanMap> StringScanMap, FDRScans& FDRScans);
void ConvertStringScansToFDGScans (std::map <std::string, StringScanMap> StringScanMap, FDGScans& FDGScans);

BOOL ConvertStringScanToFDRScan (std::vector <std::string> Scan, FDRScan& FDRScan);
BOOL ConvertStringScanToFDGScan (std::vector <std::string> Scan, FDGScan& FDGScan);

std::string ConvertFDRScanToString (FDRScan FDRScan);
std::string ConvertFDGScanToString (FDGScan FDGScan);

void MakeFDGParameters (FDGScan Scan, std:: string& Parameters);
void MakeFDRParameters (FDRScan Scan, std:: string& Parameters);

//Informations
void GetControlerScanNames (std::map <int, ScanNameMap>& ControlerScanNames, ConnectMap Connections, BOOL GetFDRScans);

void WaitForAxisToStop (int ID, char * Axis, BOOL* Res, const int MaxLen);


//SCANS
void GetControllerFDRScans (ConnectMap Connections, FDRScans& FDRScans);
void GetControllerFDGScans (ConnectMap Connections, FDGScans& FDGScans);

void SetControllerFDRScans (ConnectMap Connections, FDRScans& FDRScans);
void SetControllerFDGScans (ConnectMap Connections, FDGScans& FDGScans);


//helper
template <typename T>
std::string ConvertToString(T const & value)
{
    std::stringstream l_ss;
    l_ss  << value;
    return l_ss.str();
}

template <typename T>
std::string ConvertToStringFixed(T const & value)
{
    std::stringstream l_ss;
    l_ss << std::fixed << value;
    return l_ss.str();
}

std::vector <std::string> StdStringToVector (std::string InputString, char Delimiter);
std::vector <std::string> ConvertStringToVector (std::string InputString, char Delimiter);

void PrintAndReadline (double& Value);
void PrintAndReadline (std::string& Value);
void PrintAndReadline (unsigned int& Value);
