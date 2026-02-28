// Copyright 2025 MLOptic

#ifndef MLSPECBOS_H_
#define MLSPECBOS_H_
#define NOMINMAX
#include <Windows.h>

namespace ML {
namespace MLSpecbos {
#define PIXEL_QUANTITY_VALUE 2048
#define END_WAVELENGTH 1000
#define START_WAVELENGTH 350
#define WAVESTEPS 1
#define RADIOMETRIC_SPECTRUM_COUNT (END_WAVELENGTH - START_WAVELENGTH) / WAVESTEPS + 1
#define MAX_CALIBRATION_FILE_NUM 12

enum class ConnectionType { NotConnected, SerialCOMPort, USB_FTDI, LAN, Bluetooth };

enum class DeviceType {
    GenericJETIDevice,
    SpecbosDevice_xx01,
    Specbos_1211,
    spectrava_l1501_Or_1511,
    Specbos_2501,
};

enum class MeasHead { None, CosineCorrectorHead_Piec, IntegratingSphere, Tube };

struct SpecbosDeviceInfo {
    BYTE bConnType;
    BYTE bDeviceType;
    char cDeviceSerial[16];
    WORD wPort;
    DWORD dwBaudrate;
    char cIPAddress[16];
    char cUSBSerial[16];
    unsigned long long btAddress;
    char cVersionString[256];
    DWORD_PTR dwDevice;
    DWORD dwPixelCount;
    SpecbosDeviceInfo() {
        dwPixelCount = 2048;
        dwDevice = NULL;
        dwBaudrate = 0;
    }
};

struct PeripheralControl {
    BYTE bMeasHead;
    BOOL boLaserStat;
    PeripheralControl() {
        bMeasHead = static_cast<int>(MeasHead::None);
        boLaserStat = FALSE;
    }
};

struct MeasurementConfig {
    float fPrevTint;
    float fConfTint;
    float fTint;
    float fMinTint;
    float fMaxTint;
    bool bFiexdTint;
    WORD wPrevAver;
    WORD wConfAver;
    WORD wAver;
    DWORD dwBegin;
    DWORD dwEnd;
    DWORD dwStep;
    MeasurementConfig() {
        fPrevTint = 100;
        fConfTint = 100;
        fTint = 100;
        fMinTint = 0.1;
        fMaxTint = 6000;
        bFiexdTint = false;
        wPrevAver = 1;
        wConfAver = 1;
        wAver = 1;
        dwBegin = 0;
        dwEnd = 1000;
        dwStep = 1;
    }
};

struct MeasurementResults {
    BOOL boStatus;
    float fX;
    float fY;
    float fZ;
    float fRadio;
    float fPhoto;
    float fChromx;
    float fChromy;
    float fChromu;
    float fChromv;
    float fCCT;
    float fCRI[17];
    INT32 iDark[PIXEL_QUANTITY_VALUE];
    INT32 iLight[PIXEL_QUANTITY_VALUE];
    INT32 iRefer[PIXEL_QUANTITY_VALUE];
    INT32 iTransRefl[PIXEL_QUANTITY_VALUE];
    float fSprad[1000];
    INT32 iCountOfSprad;

    MeasurementResults() {
        boStatus = false;
        fX = 0;
        fY = 0;
        fZ = 0;
        fRadio = 0;
        fPhoto = 0;
        fChromx = 0;
        fChromy = 0;
        fChromu = 0;
        fChromv = 0;
        fCCT = 0;
        memset(fCRI, 0, 17);
        memset(iDark, 0, PIXEL_QUANTITY_VALUE * sizeof(INT32));
        memset(iLight, 0, PIXEL_QUANTITY_VALUE * sizeof(INT32));
        memset(iRefer, 0, PIXEL_QUANTITY_VALUE * sizeof(INT32));
        memset(iTransRefl, 0, PIXEL_QUANTITY_VALUE * sizeof(INT32));
        memset(fSprad, 0, 1000 * sizeof(float));
        iCountOfSprad = 0;
    }
};

struct CalibrationFile {
    DWORD dwCalibNr;
    char cMode[100];
    char cRemark[100];
    DWORD dwBegin;
    DWORD dwEnd;
    DWORD dwStep;
    DWORD dwTint;
    double dValue[1000];
    CalibrationFile() {
        dwCalibNr = 0;
        memset(cMode, 0, 100 * sizeof(char));
        memset(cRemark, 0, 100 * sizeof(char));
        dwBegin = 0;
        dwEnd = 0;
        dwStep = 1;
        dwTint = 0;
        memset(dValue, 0, 1000 * sizeof(double));
    }
};

struct CalibrationInfo {
    BYTE bCurCalibNr;
    BYTE bHead;
    CalibrationFile calibrationFiles[MAX_CALIBRATION_FILE_NUM];
    CalibrationInfo() {
        bCurCalibNr = 0;
        bHead = 0;
    }
};
}  // namespace MLSpecbos
}  // namespace ML

#endif  // MLSPECBOS_H_
