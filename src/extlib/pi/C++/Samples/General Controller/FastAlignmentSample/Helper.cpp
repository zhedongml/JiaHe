#include "Helper.h"
#include "GCSMethods.h"

// ----------------------------------------------------------------------------------------------------------
std::vector <std::string>  ConvertStringToVector (std::string InputString, char Delimiter)
{
    std::vector <std::string> l_Vector;
    std::istringstream iss (InputString);
    std::string token;
    while (std::getline (iss, token, Delimiter))
    {
        l_Vector.push_back (token);
    }
    return l_Vector;
}

// ----------------------------------------------------------------------------------------------------------
void ConvertStringScansToFDRScans (std::map <std::string, StringScanMap> IniScanMaps, FDRScans& FDRScans)
{
    int l_Count = 0;
    std::map <std::string, StringScanMap>::iterator l_ItSystemScan;
    for (l_ItSystemScan = IniScanMaps.begin (); l_ItSystemScan != IniScanMaps.end (); ++l_ItSystemScan)
    {
        StringScanMap::iterator l_ItScans;
        for (l_ItScans = l_ItSystemScan->second.begin (); l_ItScans != l_ItSystemScan->second.end (); ++l_ItScans)
        {
            std::string l_SystemIdentifier = l_ItSystemScan->first;
            std::string l_Scan = l_ItScans->second;

            std::vector <std::string> l_ScanVector = ConvertStringToVector (l_Scan, ' ');

            FDRScan l_FDRScan;
            if (ConvertStringScanToFDRScan (l_ScanVector, l_FDRScan))
            {
                FDRScans[l_SystemIdentifier].insert (std::make_pair (l_Count, l_FDRScan));
                l_Count++;
            }
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void ConvertStringScansToFDGScans (std::map <std::string, StringScanMap> IniScanMaps, FDGScans& FDGScans)
{
    int l_Count = 0;
    std::map <std::string, StringScanMap>::iterator l_ItSystemScan;
    for (l_ItSystemScan = IniScanMaps.begin (); l_ItSystemScan != IniScanMaps.end (); ++l_ItSystemScan)
    {
        StringScanMap::iterator l_ItScans;
        for (l_ItScans = l_ItSystemScan->second.begin (); l_ItScans != l_ItSystemScan->second.end (); ++l_ItScans)
        {
            std::string l_SystemIdentifier = l_ItSystemScan->first;
            std::string l_Scan = l_ItScans->second;

            std::vector <std::string> l_ScanVector = ConvertStringToVector (l_Scan, ' ');

            FDGScan l_FDGScan;
            if (ConvertStringScanToFDGScan (l_ScanVector, l_FDGScan))
            {
                FDGScans[l_SystemIdentifier].insert (std::make_pair (l_Count, l_FDGScan));
                l_Count++;
            }
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConvertStringScanToFDRScan (std::vector <std::string> Scan, FDRScan& FDRScan)
{
    unsigned int ParameterPos = 0;
    std::string l_ScanParameter = Scan[ParameterPos++];
    if ("FDR" == Scan[0])
    {
        while (ParameterPos < static_cast <unsigned int>(Scan.size ()))
        {
            //FDR Pharser
            if (l_ScanParameter == "FDR")
            {
                FDRScan.ScanRoutineName = Scan[ParameterPos++];
                FDRScan.ScanAxis = Scan[ParameterPos++];
                FDRScan.ScanAxisRange = std::stod (Scan[ParameterPos++]);
                FDRScan.StepAxis = Scan[ParameterPos++];
                FDRScan.StepAxisRange = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "L")
            {
                FDRScan.ThresholdLevel = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "A")
            {
                FDRScan.InputChannel = std::stoi (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "F")
            {
                FDRScan.Frequency = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "V")
            {
                FDRScan.Velocity = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "MP1")
            {
                FDRScan.ScanAxisMidPos = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "MP2")
            {
                FDRScan.StepAxisMidPos = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "TT")
            {
                FDRScan.TargetType = std::stoi (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "CM")
            {
                FDRScan.CentroidMethod = std::stoi (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "MIIL")
            {
                FDRScan.MinRelInputLevel = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "MAIL")
            {
                FDRScan.MaxRelInputLevel = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "ST")
            {
                FDRScan.StopPosition = std::stoi (Scan[ParameterPos++]);
            }

            if (ParameterPos < Scan.size ())
            {
                l_ScanParameter = Scan[ParameterPos++];
            }
        }
        return TRUE;
    }
    return FALSE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL ConvertStringScanToFDGScan (std::vector <std::string> Scan, FDGScan& FDGScan)
{
    unsigned int ParameterPos = 0;
    std::string l_ScanParameter = Scan[ParameterPos++];
    if ("FDG" == Scan[0])
    {
        while (ParameterPos < static_cast <unsigned int>(Scan.size ()))
        {
            //FDR Pharser
            if (l_ScanParameter == "FDG")
            {
                FDGScan.ScanRoutineName = Scan[ParameterPos++];
                FDGScan.ScanAxis = Scan[ParameterPos++];
                FDGScan.StepAxis = Scan[ParameterPos++];
            }
            else if (l_ScanParameter == "ML")
            {
                FDGScan.MinimumLevel = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "A")
            {
                FDGScan.InputChannel = std::stoi (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "F")
            {
                FDGScan.Frequency = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "V")
            {
                FDGScan.Velocity = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "MIA")
            {
                FDGScan.MinAmplitude = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "MAA")
            {
                FDGScan.MaxAmplitude = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "SP")
            {
                FDGScan.SpeedFactor = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "SPO")
            {
                FDGScan.SpeedOffset = std::stod (Scan[ParameterPos++]);
            }
            else if (l_ScanParameter == "MDC")
            {
                FDGScan.MaxDirectionChanges = std::stoi (Scan[ParameterPos++]);
            }


            if (ParameterPos < Scan.size ())
            {
                l_ScanParameter = Scan[ParameterPos++];
            }
        }
        return TRUE;
    }
    return FALSE;
}

// ----------------------------------------------------------------------------------------------------------
BOOL GetIniFile (const char* FileName, IniConfs& Conf, StringScans& Scans)
{
    char ExePath[MAX_PATH];
    HMODULE hModule = GetModuleHandle (NULL);
    std::string sExePath;
    if (hModule != NULL)
    {
        GetModuleFileNameA (hModule, ExePath, (sizeof (ExePath)));
        sExePath = ExePath;
        std::size_t PathEnd = sExePath.find_last_of ("/\\");
        sExePath = sExePath.substr (0, PathEnd + 1);
    }

    std::string FilePath = sExePath + "..\\AppData\\" + FileName;

    std::map <int, std::string> l_Keys;

    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eSystemName, "SystemName"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eCommunicationType, "CommunicationType"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eComPortBaudrate, "ComPortBaudrate"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eComPortNr, "ComPortNr"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eIPAdress, "IPAdress"));
    l_Keys.insert (std::pair <int, std::string> (ConfKeys::eIPPort, "IPPort"));

    //Get Config-File
    INIReader reader (FilePath);

    if (reader.ParseError () < 0)
    {
        return FALSE;
    }

    IniConfMap l_IniConfMap;
    StringScanMap l_IniScanMap;

    for (unsigned int l_Count = 1; l_Count < static_cast <unsigned int>(l_Keys.size () + 1); l_Count++)
    {
        l_IniConfMap.insert (std::pair <int, std::string> (l_Count, ""));
    }

    //Get Systems
    BOOL ConfigHasMoreSytems = TRUE;
    int SystemCounter = 1;
    while (ConfigHasMoreSytems)
    {

        std::string l_CurrentSystem = "System" + ConvertToString (SystemCounter);
        std::string l_Val = reader.Get (l_CurrentSystem, l_Keys[ConfKeys::eSystemName], "");
        if (0 < l_Val.size ())
        {
            Conf.insert (std::pair <std::string, IniConfMap> (l_CurrentSystem, l_IniConfMap));
            Scans.insert (std::pair <std::string, StringScanMap> (l_CurrentSystem, l_IniScanMap));
            SystemCounter++;
        }
        else
        {
            ConfigHasMoreSytems = FALSE;
        }
    }

    //Get System-Values
    int KeyCounter = 1;
    std::map <std::string, IniConfMap>::iterator l_ItSystems;
    for (l_ItSystems = Conf.begin (); l_ItSystems != Conf.end (); ++l_ItSystems)
    {
        std::string l_CurrentSystem = l_ItSystems->first;
        KeyCounter = 1;
        std::map <int, std::string>::iterator l_ItSystem;
        for (l_ItSystem = l_ItSystems->second.begin (); l_ItSystem != l_ItSystems->second.end (); ++l_ItSystem)
        {
            l_ItSystem->second = reader.Get (l_CurrentSystem, l_Keys[KeyCounter], "");
            KeyCounter++;
        }
    }

    //Get System-Scans
    BOOL ConfigHasMoreScans = TRUE;
    int ScanCounter = 1;
    std::map <std::string, StringScanMap>::iterator l_ItIniScanMap;
    for (l_ItIniScanMap = Scans.begin (); l_ItIniScanMap != Scans.end (); ++l_ItIniScanMap)
    {
        ConfigHasMoreScans = TRUE;
        ScanCounter = 1;
        while (ConfigHasMoreScans)
        {
            std::string l_CurrentSystem = l_ItIniScanMap->first;
            std::string l_CurrentScan = "ScanRoutine" + ConvertToString (ScanCounter);
            std::string l_Val = reader.Get (l_CurrentSystem, l_CurrentScan, "");
            if (0 < l_Val.size ())
            {
                l_ItIniScanMap->second.insert (std::pair <std::string, std::string> (l_CurrentScan, l_Val));
                ScanCounter++;
            }
            else
            {
                ConfigHasMoreScans = FALSE;
            }
        }
    }
    return TRUE;
}

// ----------------------------------------------------------------------------------------------------------
std::vector <std::string> StdStringToVector (std::string InputString, char Delimiter)
{
    std::vector <std::string> l_Vector;
    std::istringstream iss (InputString);
    std::string token;
    while (std::getline (iss, token, Delimiter))
    {
        l_Vector.push_back (token);
    }
    return l_Vector;
}

// ----------------------------------------------------------------------------------------------------------
void MakeFDRParameters (FDRScan Scan, std::string& Parameters)
{
    Parameters = "L " + ConvertToString (Scan.ThresholdLevel)
        + " A " + ConvertToString (Scan.InputChannel)
        + " F " + ConvertToString (Scan.Frequency)
        + " V " + ConvertToString (Scan.Velocity)
        + " MP1 " + ConvertToString (Scan.ScanAxisMidPos)
        + " MP2 " + ConvertToString (Scan.StepAxisMidPos)
        + " TT " + ConvertToString (Scan.TargetType)
        + " CM " + ConvertToString (Scan.CentroidMethod)
        + " MIIL " + ConvertToString (Scan.MinRelInputLevel)
        + " MAIL " + ConvertToString (Scan.MaxRelInputLevel)
        + " ST " + ConvertToString (Scan.StopPosition);
}

// ----------------------------------------------------------------------------------------------------------
void SetControllerFDRScans (ConnectMap Connections, FDRScans& FDRScans)
{
    std::vector <int> l_IDs;
    ConnectMap::iterator l_It;
    for (l_It = Connections.begin (); l_It != Connections.end (); ++l_It)
    {
        l_IDs.push_back (l_It->first);
    }

    unsigned int l_Count = 0;
    FDRScans::iterator l_ItFDR;
    for (l_ItFDR = FDRScans.begin (); l_ItFDR != FDRScans.end (); ++l_ItFDR)
    {
        //nur geöffnete verbindungen bedienen
        if (l_Count < static_cast<unsigned int> (l_IDs.size ()))
        {
            FDRMap::iterator l_ItSingleScan;
            for (l_ItSingleScan = l_ItFDR->second.begin (); l_ItSingleScan != l_ItFDR->second.end (); ++l_ItSingleScan)
            {
                std::string l_Parameters;
                MakeFDRParameters (l_ItSingleScan->second, l_Parameters);
                FADefineAreaScanRoutine (l_IDs[l_Count]
                    , l_ItSingleScan->second.ScanRoutineName.c_str ()
                    , (char*)l_ItSingleScan->second.ScanAxis.c_str ()
                    , l_ItSingleScan->second.ScanAxisRange
                    , l_ItSingleScan->second.StepAxis.c_str ()
                    , l_ItSingleScan->second.StepAxisRange
                    , l_Parameters.c_str ()
                );
            }
            l_Count++;
        }
    }
}


// ----------------------------------------------------------------------------------------------------------
void MakeFDGParameters (FDGScan Scan, std::string& Parameters)
{
    Parameters = "ML " + ConvertToString (Scan.MinimumLevel)
        + " A " + ConvertToString (Scan.InputChannel)
        + " MIA " + ConvertToString (Scan.MinAmplitude)
        + " MAA " + ConvertToString (Scan.MaxAmplitude)
        + " F " + ConvertToString (Scan.Frequency)
        + " SP " + ConvertToString (Scan.SpeedFactor)
        + " V " + ConvertToString (Scan.Velocity)
        + " MDC " + ConvertToString (Scan.MaxDirectionChanges)
        + " SPO " + ConvertToString (Scan.SpeedOffset);
}

// ----------------------------------------------------------------------------------------------------------
void SetControllerFDGScans (ConnectMap Connections, FDGScans& FDGScans)
{
    std::vector <int> l_IDs;
    ConnectMap::iterator l_It;
    for (l_It = Connections.begin (); l_It != Connections.end (); ++l_It)
    {
        l_IDs.push_back (l_It->first);
    }

    unsigned int l_Count = 0;
    FDGScans::iterator l_ItFDG;
    for (l_ItFDG = FDGScans.begin (); l_ItFDG != FDGScans.end (); ++l_ItFDG)
    {
        //nur geöffnete verbindungen bedienen
        if (l_Count < static_cast<unsigned int>(l_IDs.size ()))
        {
            FDGMap::iterator l_ItSingleScan;
            for (l_ItSingleScan = l_ItFDG->second.begin (); l_ItSingleScan != l_ItFDG->second.end (); ++l_ItSingleScan)
            {
                std::string l_Parameters;
                MakeFDGParameters (l_ItSingleScan->second, l_Parameters);

                FADefineGradientScanRoutine (l_IDs[l_Count]
                    , l_ItSingleScan->second.ScanRoutineName.c_str ()
                    , (char*)l_ItSingleScan->second.ScanAxis.c_str ()
                    , l_ItSingleScan->second.StepAxis.c_str ()
                    , l_Parameters.c_str ()
                );
            }
            l_Count++;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void GetControlerScanNames (std::map <int, ScanNameMap>& ControlerScanNames, ConnectMap Connections, BOOL GetFDRScans)
{
    const int c_READ_TIME_MAX = 25;
    const int c_BUFFER_SIZE_MAX = 4096;

    ConnectMap::iterator l_It;
    for (l_It = Connections.begin (); l_It != Connections.end (); ++l_It)
    {
        int l_ID = l_It->first;
        if (!PI_GcsCommandset (l_ID, "FRR?"))
        {
            return;
        }

        char l_Buffer[c_BUFFER_SIZE_MAX] = { 0 };
        std::string l_Answer;
        int i_AnswerSize = 1;
        while (0 < i_AnswerSize)
        {
            (void)PI_GcsGetAnswer (l_ID, l_Buffer, c_BUFFER_SIZE_MAX);
            l_Answer.append (l_Buffer);
            memset (l_Buffer, 0, c_BUFFER_SIZE_MAX - 1);
            Sleep (c_READ_TIME_MAX);
            (void)PI_GcsGetAnswerSize (l_ID, &i_AnswerSize);
        }

        //get scan names
        std::string l_OldToken;
        std::vector <std::string> l_ScanVector = StdStringToVector (l_Answer.c_str (), '\n');

        int l_Count = 0;
        std::vector <std::string>::iterator l_it;
        for (l_it = l_ScanVector.begin (); l_it != l_ScanVector.end (); ++l_it)
        {
            std::istringstream iss (l_it->c_str ());
            std::string token;
            std::getline (iss, token, ' ');

            if (0 < token.size () && (l_OldToken != token))
            {
                l_OldToken = token;
                int FindFDR = l_ScanVector[l_Count + 3].find ("FDR");
                int FindFDG = l_ScanVector[l_Count + 3].find ("FDG");

                if ((0 < FindFDR) && GetFDRScans)
                {
                    ControlerScanNames[l_ID].push_back (token);
                }
                if ((0 < FindFDG) && !GetFDRScans)
                {
                    ControlerScanNames[l_ID].push_back (token);
                }
            }
            l_Count++;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void GetControllerFDRScans (ConnectMap Connections, FDRScans& FDRScans)
{
    std::map <int, ScanNameMap> l_ControlerScanNames;
    GetControlerScanNames (l_ControlerScanNames, Connections, TRUE);

    std::map <int, ScanNameMap>::iterator l_It;
    for (l_It = l_ControlerScanNames.begin (); l_It != l_ControlerScanNames.end (); ++l_It)
    {
        int ID = l_It->first;
        StringScanMap l_ScanMap;
        FDRMap l_FDRMap;
        ScanNameMap::iterator l_ItScanName;
        int l_Count = 0;
        for (l_ItScanName = l_It->second.begin (); l_ItScanName != l_It->second.end (); ++l_ItScanName)
        {
            std::string ScanRoutineName = *l_ItScanName;
            const unsigned int ResultId = 4;
            char Result[2047] = { 0 };
            int BufferSize = 2047 - 1;

            FAGetRoutineResults (ID
                , ScanRoutineName.c_str ()
                , ResultId
                , Result
                , BufferSize
            );

            std::string l_SingleScan (Result);
            int l_Pos = l_SingleScan.find ("=");
            l_SingleScan = l_SingleScan.substr (l_Pos + 1);
            l_SingleScan.erase (0, l_SingleScan.find_first_not_of (" "));

            FDRScan l_FDRScan;
            ConvertStringScanToFDRScan (ConvertStringToVector (l_SingleScan, ' '), l_FDRScan);
            l_FDRMap.insert (std::pair <int, FDRScan> (l_Count, l_FDRScan));
            l_Count++;
        }
        FDRScans.insert (std::pair <std::string, FDRMap> (ConvertToString (ID), l_FDRMap));
    }
}

// ----------------------------------------------------------------------------------------------------------
void GetControllerFDGScans (ConnectMap Connections, FDGScans& FDGScans)
{
    std::map <int, ScanNameMap> l_ControlerScanNames;
    GetControlerScanNames (l_ControlerScanNames, Connections, FALSE);

    std::map <int, ScanNameMap>::iterator l_It;
    for (l_It = l_ControlerScanNames.begin (); l_It != l_ControlerScanNames.end (); ++l_It)
    {
        int ID = l_It->first;
        StringScanMap l_ScanMap;
        FDGMap l_FDGMap;
        ScanNameMap::iterator l_ItScanName;
        int l_Count = 0;
        for (l_ItScanName = l_It->second.begin (); l_ItScanName != l_It->second.end (); ++l_ItScanName)
        {
            std::string ScanRoutineName = *l_ItScanName;
            const unsigned int ResultId = 4;
            char Result[2047] = { 0 };
            int BufferSize = 2047 - 1;

            FAGetRoutineResults (ID
                , ScanRoutineName.c_str ()
                , ResultId
                , Result
                , BufferSize
            );

            std::string l_SingleScan (Result);
            int l_Pos = l_SingleScan.find ("=");
            l_SingleScan = l_SingleScan.substr (l_Pos + 1);
            l_SingleScan.erase (0, l_SingleScan.find_first_not_of (" "));

            FDGScan l_FDGScan;
            ConvertStringScanToFDGScan (ConvertStringToVector (l_SingleScan, ' '), l_FDGScan);
            l_FDGMap.insert (std::pair <int, FDGScan> (l_Count, l_FDGScan));
            l_Count++;
        }
        FDGScans.insert (std::pair <std::string, FDGMap> (ConvertToString (ID), l_FDGMap));
    }
}

// ----------------------------------------------------------------------------------------------------------
std::string ConvertFDRScanToString (FDRScan FDRScan)
{
    std::string Parameters;
    MakeFDRParameters (FDRScan, Parameters);
    std::string ScanString;

    ScanString = "FDR " + ConvertToString (FDRScan.ScanRoutineName)
        + " " + ConvertToString (FDRScan.ScanAxis)
        + " " + ConvertToString (FDRScan.ScanAxisRange)
        + " " + ConvertToString (FDRScan.StepAxis)
        + " " + ConvertToString (FDRScan.StepAxisRange)
        + " " + Parameters;

    return ScanString;
}

// ----------------------------------------------------------------------------------------------------------
std::string ConvertFDGScanToString (FDGScan FDGScan)
{
    std::string Parameters;
    MakeFDGParameters (FDGScan, Parameters);
    std::string ScanString;

    ScanString = "FDG " + ConvertToString (FDGScan.ScanRoutineName)
        + " " + ConvertToString (FDGScan.ScanAxis)
        + " " + ConvertToString (FDGScan.StepAxis)
        + " " + Parameters;
    return ScanString;
}

// ----------------------------------------------------------------------------------------------------------
void PrintAndReadline (double& Value)
{
    std::string InputLine;
    std::getline (std::cin, InputLine);

    if (!InputLine.empty ())
    {
        try
        {
            Value = std::stod (InputLine);
        }
        catch (const std::invalid_argument&)
        {
           ;
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
void PrintAndReadline (std::string& Value)
{
    std::string InputLine;
    std::getline (std::cin, InputLine);

    if (!InputLine.empty ())
    {
        Value = InputLine;
    }
}

// ----------------------------------------------------------------------------------------------------------
void PrintAndReadline (unsigned int& Value)
{
    std::string InputLine;
    std::getline (std::cin, InputLine);

    if (!InputLine.empty ())
    {
        try
        {
            Value = std::stoi (InputLine);
        }
        catch (const std::invalid_argument&)
        {
            ;
        }
    }
}


