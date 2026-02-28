#pragma once

#include "PI_GCS2_DLL.h" // Include header of PI GCS 2 DLL

#include <map>
#include <string>
#include <vector>
#include "conio.h"
#include "stdio.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>


//key-definitions
struct ConfKeys 
{
    enum EConfKeys
    {
        eSystemName         = 1,
        eCommunicationType  = 2,
        eComPortBaudrate    = 3,
        eComPortNr          = 4,
        eIPAdress           = 5,
        eIPPort             = 6,
    };
};

struct ComType 
{
    enum EComType
    {
        eTCPIP      = 1,
        eUSB        = 2,
        eRS232      = 3,
    };
};

//Scan-definitions
struct FDRScan
{
    FDRScan ()
        :ScanAxisRange    (0.0),
        StepAxisRange     (0.0),
        ThresholdLevel    (0.0),
        InputChannel      (0),
        Frequency         (0.0),
        Velocity          (0.0),
        ScanAxisMidPos    (0.0),
        StepAxisMidPos    (0.0),
        TargetType        (0),
        CentroidMethod    (0),
        MinRelInputLevel  (0.0),
        MaxRelInputLevel  (0.0),
        StopPosition      (0)
    {
    }
        std::string     ScanRoutineName;
        std::string     ScanAxis;
        double          ScanAxisRange;
        std::string     StepAxis;
        double          StepAxisRange;
        double          ThresholdLevel;
        unsigned int    InputChannel;
        double          Frequency;
        double          Velocity;
        double          ScanAxisMidPos;
        double          StepAxisMidPos;
        unsigned int    TargetType;
        unsigned int    CentroidMethod;
        double          MinRelInputLevel;
        double          MaxRelInputLevel;
        unsigned int    StopPosition;
};


struct FDGScan
{
    FDGScan ()
        :MinimumLevel       (0.0),
        InputChannel        (0),
        Frequency           (0.0),
        Velocity            (0.0),
        MinAmplitude        (0.0),
        MaxAmplitude        (0.0),
        SpeedFactor         (0.0),
        SpeedOffset         (0.0),
        MaxDirectionChanges (0)
    {
    }
        std::string     ScanRoutineName;
        std::string     ScanAxis;
        std::string     StepAxis;
        double          MinimumLevel;
        unsigned int    InputChannel;
        double          Frequency;
        double          Velocity;
        double          MinAmplitude;
        double          MaxAmplitude;
        double          SpeedFactor;
        double          SpeedOffset;
        unsigned int     MaxDirectionChanges;
};

#define UNUSED(x)

typedef std::map <int, std::string> IniConfMap;
typedef std::map <std::string, IniConfMap> IniConfs;

typedef std::map <std::string, std::string> StringScanMap;
typedef std::map <std::string, StringScanMap> StringScans;

typedef std::map <int, FDRScan> FDRMap;
typedef std::map <std::string, FDRMap> FDRScans;

typedef std::map <int, FDGScan> FDGMap;
typedef std::map <std::string, FDGMap> FDGScans;

typedef std::map <int, std::string> ConnectMap;
typedef std::vector <std::string> ScanNameMap;
