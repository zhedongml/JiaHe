#ifdef __cplusplus                //-
extern "C" {                    //-
#endif                            //-

#ifndef LSAPI_H_74948D10C9EE4B3090CE3B2851C7F0B6    //-
#define LSAPI_H_74948D10C9EE4B3090CE3B2851C7F0B6    //-

#pragma pack(push,8)    // Ensure known packing

/** @file     
*  @brief   This header defines the interface to lsapi_win32.dll
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Items that are common to both Device and Math.

typedef unsigned char byte;                                 ///< 'byte' is not in C, so make sure it's defined.

// Handles - Abstract, unique 32 bit values that refer to objects managed internally in the API
typedef size_t lsapi_HandleT;                               ///< A handle to a generic API object. \n More-specific handles will be based upon this.
typedef lsapi_HandleT lsapi_DeviceHandleT;                  ///< A device handle, which is returned by the API when you connect to a device.
typedef lsapi_HandleT lsapi_SpectrometerScanHandleT;        ///< A handle to a spectrometer scan, which is returned by the API when you instruct a Spectrometer to initiate a scan.
typedef lsapi_HandleT lsapi_CalibrationHandleT;             ///< A calibration handle, which is returned by the API when you create or load a calibration.
typedef lsapi_HandleT lsapi_ProcessedScanHandleT;           ///< A handle to a Processed Scan, which is returned by the API when you process a lsapi_SpectrometerScanHandleT.
typedef lsapi_HandleT lsapi_ReconstitutedScanHandleT;       ///< A handle to a Reconstituted Scan, which is returned by the API when you reconstitute a scan via lsapi_scan_CtlFcn_Reconstitute.
typedef lsapi_HandleT lsapi_SpectrumHandleT;                ///< A handle to a Spectrum
typedef lsapi_HandleT lsapi_ScalarTransientHandleT;         ///< A handle to a Scalar Transient [scan].

#define lsapi_TEXTBUF_SIZE                  1024            ///< Size of various text buffers used in the API
#define lsapi_LAMPBUF_SIZE                  4096            ///< Size of buffer used to import lamp files. (used in lsapi Labview wrapper)
#define lsapi_INVALID_HANDLE_VALUE             0            ///< Value for an invalid API handle

typedef enum    ///< Defines the values returned by the Labsphere API
{
    // Range <1000 are general codes
    lsapi_Result_Success                        = 0,        ///< The API successfully performed/completed the requested operation
    lsapi_Result_Fail                           = 1,        ///< General, nonspecific failure. \n If this is returned, please report the circumstances to Labsphere so we can amend the API to return a specific error code.
    lsapi_Result_ApiNotOpen                     = 2,        ///< You requested an API function before successfully opening the API.
    lsapi_Result_ApiAlreadyOpen                 = 3,        ///< You attempted to open the API when it was already open. \n This is a "soft error" in that you may continue to use the already-open API.
    lsapi_Result_InvalidArgument                = 4,        ///< One of the arguments you supplied is not valid.
    lsapi_Result_InsufficientBuffer             = 5,        ///< An array passed to the API is too small to hold all the required data.
    lsapi_Result_UserAbort                      = 6,        ///< The user (i.e. API client code) aborted an operation.
    lsapi_Result_FailedToLoadDependantDll       = 7,        ///< One of the DLLs required by the API was not found. \n The full path to the missing DLL is provided in the error detail text.
    lsapi_Result_MissingDeviceSupportFile       = 8,        ///< A support file required by the device was not found. \n  The full path to the missing file is provided in the error detail text.
    lsapi_Result_InternalError                  = 9,        ///< The API encountered an unexpected problem. \n If this is returned, please report the circumstances to Labsphere so we can amend the API to return a specific error code.
    lsapi_Result_InvalidStructureSize           = 10,       ///< A structure passed to the API had an incorrect StructSize value. \n Be sure to properly initialize the StructSize member to sizeof(struct you are passing).
    lsapi_Result_IncompleteData                 = 12,       ///< You requested device information before the information is available. \n One example is requesting a spectrometer's scan data before the scan has completed.
    lsapi_Result_ParameterOutOfRange            = 13,       ///< A passed-in parameter was outside the device's valid range. \n One example is requesting a power supply to source 10 amps when it's maximum current is 5 amps.
    lsapi_Result_LockFailed                     = 14,       ///< The API failed to share an internal object. \n If this is returned, please report the circumstances to Labsphere so we can amend the API to return a specific error code.
    lsapi_Result_RequestTimeout                 = 15,       ///< The device did not responed within the expected time. \n Confirm that the device is connected, powered on, and functioning properly.
    lsapi_Result_FileNotFound                   = 17,       ///< A file-path passed to the API was not found. \n Verify that the file does exist in the location you specified.
    lsapi_Result_UnknownException               = 18,       ///< An unexpected exception occurred.
    lsapi_Result_InvalidApiKey                  = 19,       ///< The API Key supplied to the API-Open function was not valid. \n Verify that the key being passed is identical to the Key that was supplied to you by Labsphere.
    lsapi_Result_InvalidApiVersion              = 20,       ///< The Labsphere API modules have differeing version numbers. \n All modules must have the same version.
    lsapi_Result_InvalidOperation               = 21,       ///< An invalid operation was requested. \n For example, the calling code requested to set a read-only value.
    lsapi_Result_FileAlreadyExists              = 22,       ///< The API attempted to overwrite a file that already exists.
    lsapi_Result_FailedToCreateObject           = 23,       ///< The API failed to create a memory object.
    lsapi_Result_ObjectNotSerializable          = 24,       ///< Attempted to save an object to file but the object does not support saving.
    lsapi_Result_UnknownFileFormat              = 25,       ///< Attempted to read a file whose format is unknown to the API.
    lsapi_Result_InvalidFileFormat              = 26,       ///< Attempted to read a file whose format is unknown to the API.
    lsapi_Result_UnknownHandle                  = 27,       ///< The handle passed to the API is unknown to the API.
    lsapi_Result_ReleasedHandle                 = 28,       ///< Attempted to use a handle that has been released.
    lsapi_Result_WrongHandleType                = 29,       ///< Attempted to use a handle of type A when the operation requires type B.
    lsapi_Result_UnknownCommand                 = 30,       ///< The calling code supplied a command that is not recognized by the API.
    lsapi_Result_MismatchedCommand              = 31,       ///< The command does not agree with the function (eg. a Math API command was processed by the Device API).
    lsapi_Result_XmlParseError                  = 32,       ///< There was an error parsing one of the API's XML-based datafiles
    lsapi_Result_XmlComposeError                = 33,       ///< There was an error composing XML within the API
    lsapi_Result_FileWriteError                 = 34,       ///< The API encountered an error in writing a file
    lsapi_Result_DirectoryDoesntExist           = 35,       ///< A directory passed to the API does not exist.
    lsapi_Result_NotFound                       = 36,       ///< The indicated item was not found.
    lsapi_Result_FunctionNotImplemented         = 37,       ///< Function not implemented by API, e.g. work in progress 

    // Range 1000...1999 are Math-specific codes
    lsapi_Result_UnsupportedWavelengthInterval  = 1000,     ///< Wavelength interval used for spectral data is not supported by MathAPI
    lsapi_Result_SensitivityAlreadySpecified    = 1001,     ///< Client already specified System Sensitivity for this ScanCalibrator
    lsapi_Result_SensitivityNotSpecified        = 1002,     ///< Client did not yet specify System Sensitivity for this ScanCalibrator
    lsapi_Result_AbsorptionCorrectionNotSpecified = 1003,   ///< Client requested Absorption Correction but did not provide the AuxWithCal and/or AuxWithDut scans to the ScanCalibrator
    lsapi_Result_DeviceScanNotSpecified         = 1004,     ///< Client did not yet specify Device Scan for this ScanCalibrator
    lsapi_Result_FileUnexpectedInput            = 1005,     ///< Unexpected input in data file
    lsapi_Result_FileBadData                    = 1006,     ///< Bad data in file, such as inconsistent wavelength interval
    lsapi_Result_InvalidIntegrationTime         = 1007,     ///< A Scan contained an invalid integration time
    lsapi_Result_InvalidScanData                = 1008,     ///< A Scan contained invalid data (0 or negative)
    lsapi_Result_MathOperationFailed            = 1009,     ///< A math operation failed; check the error detail text.
    lsapi_Result_SpectrumAnalysisFailed         = 1011,     ///< Spectrum analysis failed; check the error detail text.
    lsapi_Result_ScanCalibrationFailed          = 1010,     ///< Scan calibration failed; check the error detail text.

    // Range 2000...2999 are Device-specific codes
    lsapi_Result_BadSerialNumber                = 2000,     ///< Device connect request specified a serial number that does not correlate to any known device .
    lsapi_Result_DeviceNotConnected             = 2001,     ///< The Device API attempted to communicate with a device that is not connected.
    lsapi_Result_ConnectFailed                  = 2002,     ///< The Device API attempted to connect to a device and failed.
    lsapi_Result_DeviceBusy                     = 2004,     ///< A request cannot be completed because the device is busy. \n One example is requesting a spectrometer to start a scan when it is already busy scanning.
    lsapi_Result_DeviceInUse                    = 2005,     ///< The Device API attempted to connect to a device that has already been connected.
    lsapi_Result_UnknownDeviceType              = 2006,     ///< The calling code supplied a device type that is not recognized by the Device API.
    lsapi_Result_NoDeviceFound                  = 2007,     ///< The calling code specified a device that is unknown to the API. \n  One example is requesting to connect to a device that is not connected to the system.
    lsapi_Result_WrongDeviceType                = 2008,     ///< The calling code specified a command that the device doesn't support. \n One example would be requesting a power supply to start a scan.
    lsapi_Result_FeatureNotSupported            = 2009,     ///< The calling code requested an operation not supported by the device. \n One example would be attempting to close the shutter on a shutterless spectrometer.
    lsapi_Result_DeviceAlreadyInstalled         = 2010,     ///< The calling code attemped to re-install an already-installed Virtual Device.
    lsapi_Result_DeviceMismatch                 = 2011,     ///< An operation was requested that uses data from different devices. \ One example would be applying a dark scan from spectrometer A to a DUT scan from spectrometer B.
    lsapi_Result_WrongSerialNumber              = 2012,     ///< A device reported a serial number that was different from what the API expected.
    lsapi_Result_IoError                        = 2013,     ///< Failed to communicate with a device. \n One example would be a failure of an RS232 connection to a device.
    lsapi_Result_UnexpectedResponse             = 2014,     ///< The API received a response from a device that was different than expected.
    lsapi_Result_DeviceSpecificError            = 2015,     ///< The API received a response from a device that that is specific to the device. Retrieve the last error detail to view the device's response.
    lsapi_Result_PortNotOpen                    = 2016,     ///< The API attempted to send data to a device that is not properly connected.
    lsapi_Result_DeviceNotReady                 = 2017,     ///< The API attempted to send data to a device that is not prepared to receive data.
    lsapi_Result_DeviceNotFound                 = 2018,     ///< The API was unable to locate a requested device.
    lsapi_Result_DeviceNotInstalled             = 2019,     ///< The API attempted to connect to a device that is not installed, or partially installed. \n One example would be attempting to connect to a CDS30x0 before installing its wavelength calibration file.
    lsapi_Result_ConnectDisallowed              = 2020,     ///< Connecting to physical devices is disallowed in Demo mode
    lsapi_Result_DeviceOperationFailed          = 2021,     ///< The device reported that the attempted operation failed, for example due to unsatisfied safety conditions.
    lsapi_Result_FilterNotFound                 = 2022,     ///< A filter was specificed that does not exist in the device
    lsapi_Result_BaselineOffsetNotSupported     = 2023,     ///< The device does not support baseline offset correction (only supported by shutterless spectrometers)
    lsapi_Result_ReadOnly                       = 2024,     ///< The indicated item is read-only

    // Range 21xx are spectrometer-specific codes
    lsapi_Result_NoAutoExpData                  = 2100,     ///< The calling code requested Autoexposure information from a spectrometer that does not have any autoexposure data.
    lsapi_Result_NoBaselineOffsetCorrectionData = 2101,     ///< The calling code requested dark offset correction for a spectrometer that does not have dark correction data.
    lsapi_Result_SpectrometerTECTempOutOfRange  = 2102,     ///< The spectrometer TEC temperature is not in range.
    // Range 22xx are power-supply-specific codes
    lsapi_Result_OutputNotEnabled               = 2200,     ///< You requested an operation that is only valid when the power supply's output is enabled
    // Range 23xx are temperature-controller-specific codes
    lsapi_Result_SensorFault                    = 2300,     ///< The TEC's sensor is not connected

    // Range 3000...3099 are Scan-specific codes
    lsapi_Result_IntegrationTimeMismatch        = 3000,     ///< The calling code requested dark offset correction for a scan, but the DUT scan and the Dark scan have different integration times.
    lsapi_Result_FilterMismatch                 = 3001,     ///< The calling code requested dark offset correction for a scan, but the DUT scan and the Dark scan were taken with different filters.
    lsapi_Result_ScanProcessingFailed           = 3002,     ///< The calling code requested scan processing option(s) which failed. This can include requesting non-linearity correction without a valid dark scan. 
    lsapi_Result_ScanIncomplete                 = 3003,     ///< Calling code requested data from an incomplete scan (e.g. lsapi_scan_State_InProcess or lsapi_scan_State_AwaitingTrigger)
    lsapi_Result_ScanAborted                    = 3005,     ///< Calling code requested data from an aborted scan 
    lsapi_Result_ScanFailed                     = 3004,     ///< Calling code requested data from a failed scan 

    // Range 3100...3199 are Calibration-specific codes
    lsapi_Result_CalibrationNameAlreadyExists   = 3100,
    lsapi_Result_CalibrationDoesntExist         = 3101,
    lsapi_Result_CalibrationWrongFilterCount    = 3102,     ///< The calling code did not supply a scan for each spectrometer filter
    lsapi_Result_CalibrationLampSerialNumberAlreadyExists = 3103, ///< The calling code specified a Calibration Lamp serial number that is already installed in the API.
    lsapi_Result_CalibrationLampNotInstalled    = 3104,     ///< The calling code specified a Calibration Lamp that has not been installed into the API.
    lsapi_Result_CalibrationLoadError           = 3105,     ///< Calibration Load error
    lsapi_Result_CalibrationScanDataMismatch    = 3106,     ///< Calibration and/or DUT scans' range or processing options do not match

    // Range 4000...4099 are database-specific codes
    lsapi_Result_DbOpenFail                     = 4000,     ///< The API failed to open its database
    lsapi_Result_DbNotOpen                      = 4001,     ///< A database function was attempted when the database is not open
    lsapi_Result_DbErr                          = 4002,     ///< A database operation failed
    lsapi_Result_DbRecordNotFound               = 4003,     ///< The requested record was not found in the database
    lsapi_Result_DbQueryFailed                  = 4004,     ///< A database query failed

    // Range 4100...4199 are license-specific codes
    lsapi_Result_LicenseDataNotValid            = 4100,     ///< The license data is not valid
    lsapi_Result_LicenseAlreadyInstalled        = 4101,     ///< A license was requested to be re-installed
    lsapi_Result_LicenseSystemNotValid          = 4102,     ///< The System ID in the license does not match the current system
    lsapi_Result_LicenseNotAllowed              = 4103,     ///< The system does not have a license for the specified feature or function

} lsapi_ResultT;

/*! A helper macro for determining a successful result */
#define LSAPISUCCESS(_rslt_)    (lsapi_Result_Success == (_rslt_))

/** Contains the API's version information. \n
This struct mimics the Win32's VERSIONINFO (which uses WORD for the fields) and .NET's System.Version (which uses int). \n
We use unsigned short (aka WORD) because our fields are 16 bit unsigned values.
*/
typedef struct lsapi_Version    ///< Defines API version
{
    size_t StructSize;          ///< must == sizeof(lsapi_Version)
    unsigned short Major;       ///< Indicates the major featureset of the API. Changes rarely.
    unsigned short Minor;       ///< Indicates the minor featureset of the API. Changes when any API interface or functionality is altered.
    unsigned short Build;       ///< Labsphere-custom version field. Indicates the build date as a count of days since January 1, 2016.
    unsigned short Revision;    ///< Labsphere-custom version field. Indicates the the time of day that the build occurred.
} lsapi_Version;

/*! Defines the basic functions that the API can perform. \n This is a field in ::lsapi_ApiCallHdr, which is passed to ::lsapi_ApiCall() */
typedef enum
{
    lsapi_ApiCall_Unknown=0,                        ///< Default/uninitialized value. This is never used, but rather provides a placeholding value for when a containing struct is zero'ed.

    // General calls
    lsapi_ApiCall_Api_Open,                         ///< Open the API. Used with ::lsapi_ApiCall_Api_Open_Params
    lsapi_ApiCall_Api_Close,                        ///< Close the API. Used with ::lsapi_ApiCallHdr
    lsapi_ApiCall_Api_Utilities,                    ///< When combined with one of the ::lsapi_api_UtilityTypeT values, this performs general API utilities. Used with ::lsapi_ApiCall_Api_UtilitiesHdr

    lsapi_ApiCall_License,                          ///< When combined with one of the ::lsapi_api_LicenseFcnT values, this performs License-related functions. Used with ::lsapi_ApiCall_LicenseHdr

    // Device calls
    lsapi_ApiCall_Device_CatalogDevices=100,        ///< Catalog supported and/or Connectable devices. Used with ::lsapi_ApiCall_Device_CatalogDevices_Params
    lsapi_ApiCall_Device_CatalogDeviceClass,        ///< Catalog supported and/or Connectable devices based on Device Class. Used with ::lsapi_ApiCall_Device_CatalogDeviceClass_Params.
    lsapi_ApiCall_Device_ConnectDevice,             ///< Connect to a specified device. Used with ::lsapi_ApiCall_Device_ConnectDevice_Params
    lsapi_ApiCall_Device_ControlDevice,             ///< Invokes a supported operation (specified by DeviceClass and SubCmd values in ::lsapi_ApiCall_Device_ControlDeviceHdr) on a connected device.
    lsapi_ApiCall_Device_Utilities,                 ///< When combined with one of the ::lsapi_device_UtilityTypeT values, this performs device utilities not covered by general API utilities. Used with ::lsapi_ApiCall_Device_UtilitiesHdr

    // Scan control
    lsapi_ApiCall_Scan_Control,                     ///< When combined with one of the ::lsapi_scan_CtlFcnT values, this controls a scan. Used with ::lsapi_ApiCall_Scan_ControlHdr
    
    // Calibration
    lsapi_ApiCall_Calibration,                      ///< When combined with one of the ::lsapi_calibration_FunctionT values, this manages calibrations. Used with ::lsapi_ApiCall_Calibration_FunctionHdr
 
    // Spectrum Analysis
    lsapi_ApiCall_Spectrum,                         ///< When combined with one of the ::lsapi_Spectrum_FunctionT values, this manages Spectrum objects. Used with ::lsapi_ApiCall_Spectrum_FunctionHdr

    // ScalarTransient control
    lsapi_ApiCall_ScalarTransient_Control,          ///< When combined with one of the ::lsapi_ScalarTransient_CtlFcnT values, this manages ScalarTransient [scan] objects. Used with ::lsapi_ApiCall_ScalarTransient_ControlHdr

} lsapi_ApiCallT;

typedef struct lsapi_StructHdr              ///< Base header for all structs passed to the API. 
{
    size_t StructSize;                      ///< Prior to calling the API, this value <b>must</b> be initialized to the sizeof() the outermost struct.
} lsapi_StructHdr;

typedef struct lsapi_ApiCallHdr             ///< Base struct for for all ApiCall structs.
{
    lsapi_StructHdr hdr;                    ///< Prior to calling the API, hdr.StructSize <b>must</b> be initialized to the sizeof() the outermost struct.
    lsapi_ApiCallT ApiCall;                 ///< The type of call being made
} lsapi_ApiCallHdr;

/** \fn lsapi_ResultT __stdcall lsapi_ApiCall(lsapi_ApiCallHdr* pHdr)
    \brief This is the single interface to the Labsphere API
    \param pHdr Address of an ::lsapi_ApiCallHdr struct, or a larger struct that contains lsapi_ApiCallHdr as its first element.
    \return An ::lsapi_ResultT value that indicates success or failure
*/
lsapi_ResultT __stdcall lsapi_ApiCall(lsapi_ApiCallHdr* pHdr);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scan functionality, part 1

// As dependencies, we must declare 'lsapi_scan_TypeT' before Spectrometer, and 'lsapi_scan_TriggerModeT' before TransientDetector
/*! Type of scan you wish to perform. \n Passed in the Scantype member of ::lsapi_device_ScanParams with the ::lsapi_device_Control_Spectrometer_StartScan command. */
typedef enum
{
    lsapi_scan_Type_Unknown                     = 0,     ///< Illegal/uninitialized value
    lsapi_scan_Type_Idle                        = 70,    ///< This is an undocumented library-internal value and should not be used.
    lsapi_scan_Type_DutScan                     = 71,    ///< Request an asynchronous software-initiated Device-Under-Test scan.
    lsapi_scan_Type_SyncDutScan                 = 72,    ///< Request a synchronous software-initiated Device-Under-Test scan. API Blocks until scan completes. Scan can NOT be aborted. (Not supported on all spectrometers; check for lsapi_device_spectrometer_FeatureFlag_SyncScan)
    lsapi_scan_Type_TriggeredDutScan            = 73,    ///< Request an asynchronous hardware-triggered Device-Under-Test scan.
    lsapi_scan_Type_SyncTriggeredDutScan        = 74,    ///< Request a synchronous hardware-triggered Device-Under-Test scan. API Blocks until scan completes. Scan can NOT be aborted. (Not supported on all spectrometers; check for lsapi_device_spectrometer_FeatureFlag_SyncScan)
    lsapi_scan_Type_DarkScan                    = 75,    ///< Request a Dark Scan (valid only for spectrometers with a shutter).
    lsapi_scan_Type_BaselineOffset              = 76,    ///< Request a Baseline Offset (valid only for spectrometers <b>without</b> shutter).
    lsapi_scan_Type_AutoExposure                = 77,    ///< Request an Autoexposure.
} lsapi_scan_TypeT;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! Indicates the state of a scan that you have initiated, for both ScalarTransient and Spectrometer scans \n Scan State may be obtained via ::lsapi_ScalarTransient_CtlFcn_GetStatus or ::lsapi_scan_CtlFcn_GetInfo */
typedef enum
{
    lsapi_scan_State_Unknown                    = 0,     ///< Illegal/uninitialized value
    lsapi_scan_State_Starting                   = 80,    ///< Scan is starting, but has not actually started.
    lsapi_scan_State_InProcess                  = 81,    ///< Scan is in progress. \n If you have set an ::lsapi_device_EventCallback function, progress percentage is reported via the ::lsapi_device_EventId_Spectrometer_Scan_Progress event
    lsapi_scan_State_Succeeded                  = 82,    ///< Scan has completed successfully. Scan data is valid.
    lsapi_scan_State_Error                      = 83,    ///< Scan has completed with an error. Scan data is not valid.
    lsapi_scan_State_Aborted                    = 84,    ///< Scan has been aborted (via ::lsapi_scan_CtlFcn_Abort). Scan data is not valid.
    lsapi_scan_State_AwaitingTrigger            = 85,    ///< Scan is awaiting HW trigger
} lsapi_scan_StateT;

/*! Specifies trigger mode for scanning Devices, namely TransientDetector and [TODO/FUTURE] Spectrometer \n Input parameter to ::lsapi_device_Control_TransientDetector_StartScan_Params */
typedef unsigned long lsapi_scan_TriggerModeT;                                          ///< A bitmask for 
const lsapi_scan_TriggerModeT lsapi_scan_TriggerMode_None           = 0x00000000;       ///< Client initiates scan through software, not otherwise synchronized with DUT
const lsapi_scan_TriggerModeT lsapi_scan_TriggerMode_ExternalEdge   = 0x00000001;       ///< Scan starts on hardware edge trigger (such as TTL) to scanning Device


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Device-specific functionality
// lsapi_device_*

// Typedefs
typedef unsigned long lsapi_device_CatalogOptionsT;    ///< Type definition for the bitmask used in retrieving Device catalogs.

struct lsapi_device_EventHeader;    // Forward declaration to allow callback typedef

/** Function signature definition for the event callback provided to the API \n
\param pHdr Address of an lsapi_device_EventHeader struct, or a larger struct that contains lsapi_device_EventHeader as its first element.\n
See the description of the lsapi_device_EventHeader struct for details. \n
\param pContext User-supplied value. This is typically a pointer to an object in the user code. 
*/
typedef void (__stdcall *lsapi_device_EventCallback)(lsapi_device_EventHeader* pHdr, void* pContext);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrometer
typedef unsigned long lsapi_device_spectrometer_FeatureFlagsT;                  ///< A bitmask of features supported by a Spectrometer.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Power Supply
typedef unsigned long lsapi_device_powersupply_FeatureFlagsT;                   ///< A bitmask of features supported by a Power Supply.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TEC
typedef unsigned long lsapi_device_tempcontroller_FeatureFlagsT;                ///< A bitmask of features supported by a Temperature Controller.
// Relay Controller
typedef unsigned long lsapi_device_relaycontroller_FeatureFlagsT;               ///< A bitmask of features supported by a Relay Controller.
// Power Meter
typedef unsigned long lsapi_device_powermeter_FeatureFlagsT;                    ///< A bitmask of features supported by a Power Meter.
// Multimeter
typedef unsigned long lsapi_device_multimeter_FeatureFlagsT;                    ///< A bitmask of features supported by a Multimeter.
//Photometry Module
typedef unsigned long lsapi_device_photometrymodule_FeatureFlagsT;              ///< A bitmask of features supported by a Photometry Module.
//Detector
typedef unsigned long lsapi_device_detector_FeatureFlagsT;						///< A bitmask of features supported by a Detector.

//===========================================================================================
//===========================================================================================
// #defined values
//===========================================================================================

#define lsapi_device_MAX_STRINGLENGTH            128        ///< Maximum string length used in the Device API's various structures.

typedef unsigned __int64 lsapi_device_TypeT;                ///< indicates what specific type/model a particular device is.

const lsapi_device_TypeT lsapi_device_Type_None                = 0;           ///< Value to indicate the absence of any particular device type

const lsapi_device_TypeT lsapi_device_Type_CDS600              = 0x0000000000000001;    ///< A CDS600 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS610              = 0x0000000000000002;    ///< A CDS610 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS1100             = 0x0000000000000004;    ///< A CDS1100 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS2100             = 0x0000000000000008;    ///< A CDS2100 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS3000             = 0x0000000000000010;    ///< A CDS3000 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS3010             = 0x0000000000000020;    ///< A CDS3010 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS3020             = 0x0000000000000040;    ///< A CDS3020 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS3030             = 0x0000000000000080;    ///< A CDS3030 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_SMS500              = 0x0000000000000100;    ///< An SMS500 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_SMS510              = 0x0000000000000200;    ///< An SMS510 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_ILT960UVLS          = 0x0000000000000201;    ///< An ILT960 low straylight UV Spectrometer
const lsapi_device_TypeT lsapi_device_Type_ILT960UV            = 0x0000000000000202;    ///< An ILT960 UV Spectrometer
const lsapi_device_TypeT lsapi_device_Type_ILT960VIS           = 0x0000000000000203;    ///< An ILT960 UV-VIS Spectrometer
const lsapi_device_TypeT lsapi_device_Type_ILT960BB            = 0x0000000000000204;    ///< An ILT960 broadband Spectrometer
const lsapi_device_TypeT lsapi_device_Type_ILT960NIR           = 0x0000000000000205;    ///< An ILT960 NIR Spectrometer
const lsapi_device_TypeT lsapi_device_Type_Keithley2400        = 0x0000000000000400;    ///< A Keithley 2400 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2401        = 0x0000000000000401;    ///< A Keithley 2401 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2410        = 0x0000000000000800;    ///< A Keithley 2410 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2420        = 0x0000000000001000;    ///< A Keithley 2420 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2425        = 0x0000000000002000;    ///< A Keithley 2425 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2440        = 0x0000000000004000;    ///< A Keithley 2440 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley6485        = 0x0000000000004001;    ///< A Keithley 6485 Picoammeter, used to read a detector
const lsapi_device_TypeT lsapi_device_Type_Keithley6514        = 0x0000000000004002;    ///< A Keithley 6514 Electrometer, used to read a detector
const lsapi_device_TypeT lsapi_device_Type_Keithley2460        = 0x0000000000004003;    ///< A Keithley 2460 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2461        = 0x0000000000004004;    ///< A Keithley 2461 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2602        = 0x0000000000004005;    ///< A Keithley 2602B 2-channel Power Supply
const lsapi_device_TypeT lsapi_device_Type_AgilentE3632A       = 0x0000000000008000;    ///< An Agilent E3632A Power Supply
const lsapi_device_TypeT lsapi_device_Type_AgilentE3633A       = 0x0000000000010000;    ///< An Agilent E3633A Power Supply
const lsapi_device_TypeT lsapi_device_Type_AgilentE3634A       = 0x0000000000020000;    ///< An Agilent E3634A Power Supply
const lsapi_device_TypeT lsapi_device_Type_AgilentN5751A       = 0x0000000000040000;    ///< An Agilent N5751A USB Power Supply
const lsapi_device_TypeT lsapi_device_Type_AgilentN5749A       = 0x0000000000040001;    ///< An Agilent N5749A USB Power Supply
const lsapi_device_TypeT lsapi_device_Type_Arroyo5305          = 0x0000000000080000;    ///< An Arroyo 5305 Thermo-electric cooler
const lsapi_device_TypeT lsapi_device_Type_Arroyo585           = 0x0000000000080001;    ///< An Arroyo 585 TECPak
const lsapi_device_TypeT lsapi_device_Type_Arroyo5300          = 0x0000000000080002;    ///< An Arroyo 5300 Thermo-electric cooler
const lsapi_device_TypeT lsapi_device_Type_Arroyo485           = 0x0000000000080003;    ///< An Arroyo 485 LaserPak power supply
const lsapi_device_TypeT lsapi_device_Type_LairdTEC            = 0x0000000000080004;    ///< A Laird Thermo-electric cooler
const lsapi_device_TypeT lsapi_device_Type_Arroyo586           = 0x0000000000080005;    ///< An Arroyo 586 TECPak
const lsapi_device_TypeT lsapi_device_Type_Arroyo5240          = 0x0000000000080006;    ///< An Arroyo 5240 Thermo-electric cooler
const lsapi_device_TypeT lsapi_device_Type_MeerstetterTEC      = 0x0000000000080007;    ///< A Meerstetter Thermo-electric cooler
const lsapi_device_TypeT lsapi_device_Type_ArroyoRC1           = 0x0000000000100000;    ///< An Arroyo RC-1 Relay Controller
const lsapi_device_TypeT lsapi_device_Type_Chroma61603         = 0x0000000000200000;    ///< A Chroma61603 AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_Chroma61604         = 0x0000000000200001;    ///< A Chroma61604 AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_YokogawaWT210       = 0x0000000000400000;    ///< A Yokogawa WT210 AC Power Meter
const lsapi_device_TypeT lsapi_device_Type_TETP62201           = 0x0000000000800000;    ///< A Tet P62201 AC Power Meter
const lsapi_device_TypeT lsapi_device_Type_PhotalLE7700        = 0x0000000001000000;    ///< A Photal LE7700 Spectrometer 
const lsapi_device_TypeT lsapi_device_Type_MaynuoM8811         = 0x0000000002000000;    ///< A Maynuo M8811 DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_TDKLambda_GEN40_19  = 0x0000000004000000;    ///< A TDK-Lambda GEN40-19 DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_TDKLambda_GEN150_10 = 0x0000000008000000;    ///< A TDK-Lambda GEN150-10 DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_TDKLambda_ZSeries   = 0x0000000008000001;    ///< A TDK-Lambda Z Series DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_TDKLambda_GenPlus   = 0x0000000008000002;    ///< A TDK-Lambda GeneSys+ Series DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_Keithley2000        = 0x0000000010000000;    ///< A Keithley 2000 MultiMeter
const lsapi_device_TypeT lsapi_device_Type_KeithleyDMM6500     = 0x0000000010000001;    ///< A Keithley DMM6500 Digital MultiMeter
const lsapi_device_TypeT lsapi_device_Type_Quadtech31015       = 0x0000000020000000;    ///< A Quadtech 31015 AC power supply, which is identical to a Chroma61603
const lsapi_device_TypeT lsapi_device_Type_ParwaAPS6000        = 0x0000000040000000;    ///< A Parwa APS6000 AC power supply, 
const lsapi_device_TypeT lsapi_device_Type_Keithley2430        = 0x0000000080000000;    ///< A Keithley 2430 Power Supply
const lsapi_device_TypeT lsapi_device_Type_Xitron2801          = 0x0000000100000000;    ///< A Xitron 2801 AC Power Meter
const lsapi_device_TypeT lsapi_device_Type_XitronXT2640        = 0x0000000100000001;    ///< A Xitron XT2640 AC Power Meter
const lsapi_device_TypeT lsapi_device_Type_OmegaMDSSi8         = 0x0000000200000000;    ///< An Omega MDSSi8 temperature monitor
const lsapi_device_TypeT lsapi_device_Type_LabsphereRC6        = 0x0000000400000000;    ///< A Labsphere RC6 relay controller, when it's NOT part of a PM
const lsapi_device_TypeT lsapi_device_Type_OmegaTC08           = 0x0000000800000000;    ///< An Omega TC08 temperature monitor
const lsapi_device_TypeT lsapi_device_Type_LabspherePM100      = 0x0000001000000000;    ///< A Labsphere PM-100 Photometry Module, really just an RC6 plus an LPS-100
const lsapi_device_TypeT lsapi_device_Type_LabspherePM150      = 0x0000002000000000;    ///< A Labsphere PM-150 Photometry Module, really just an RC6 plus an LPS-150
const lsapi_device_TypeT lsapi_device_Type_LabsphereICM500     = 0x0000002000000001;    ///< A Labsphere ICM-500 Photometry Module
const lsapi_device_TypeT lsapi_device_Type_LabsphereATC82      = 0x0000004000000000;    ///< A Labsphere ATC-82 Module
const lsapi_device_TypeT lsapi_device_Type_Chroma61602         = 0x0000008000000000;    ///< A Chroma61602 AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_QINGZHI8775C1       = 0x0000010000000000;    ///< A Qingzhi 8775C1 AC Power Meter
const lsapi_device_TypeT lsapi_device_Type_YokogawaWT310       = 0x0000020000000000;    ///< A Yokogawa WT310 AC Power Meter
const lsapi_device_TypeT lsapi_device_Type_YokogawaWT3000      = 0x0000020000000001;    ///< A Yokogawa WT3000 AC Power Meter
const lsapi_device_TypeT lsapi_device_Type_Agilent6811B        = 0x0000040000000000;    ///< An Agilent 6811B AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_Agilent6812B        = 0x0000080000000000;    ///< An Agilent 6812B AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_BKP9801             = 0x0000080000000001;    ///< A BK Precision 9801 AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_CAS140              = 0x0000100000000000;    ///< An InstrumentSystems CAS140B/CT-PCI spectrometer
const lsapi_device_TypeT lsapi_device_Type_SMS500ULS           = 0x0000200000000000;    ///< An SMS500ULS Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS800              = 0x0000200000000001;    ///< A CDS-800 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_AvaSpecMini         = 0x0000200000000002;    ///< An AvaSpec-Mini Spectrometer from Avantes
const lsapi_device_TypeT lsapi_device_Type_AvaSpecEvo          = 0x0000200000000003;    ///< An AvaSpec-EVO Spectrometer from Avantes
const lsapi_device_TypeT lsapi_device_Type_Chroma61601         = 0x0000400000000000;    ///< A Chroma61601 AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_LabsphereAS82       = 0x0000800000000000;    ///< A Labsphere AS-82 Module
const lsapi_device_TypeT lsapi_device_Type_Chroma61605         = 0x0001000000000000;    ///< A Chroma61605 AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_CDS2600             = 0x0002000000000000;    ///< A CDS2600 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_CDS2400             = 0x0002000000000001;    ///< A CDS2400 Spectrometer
const lsapi_device_TypeT lsapi_device_Type_QEProGeneric        = 0x0002000000000002;    ///< A QEProGeneric Spectrometer
const lsapi_device_TypeT lsapi_device_Type_NIRQuest512         = 0x0002000000000003;    ///< An OceanOptics NIRQuest512 Spectrometer (512 pixels)
const lsapi_device_TypeT lsapi_device_Type_FlameNIR128         = 0x0002000000000004;    ///< An Ocean Insight FlameNIR Spectrometer (128 pixels)
const lsapi_device_TypeT lsapi_device_Type_CDS2600UV           = 0x0002000000000005;    ///< A CDS2600-UV Spectrometer
const lsapi_device_TypeT lsapi_device_Type_NIRQuest512_22      = 0x0002000000000006;    ///< An OceanOptics NIRQuest512-2.2 Spectrometer (512 pixels)
const lsapi_device_TypeT lsapi_device_Type_GentecIntegraSiLo   = 0x0004000000000000;    ///< A Gentec Integra low level silicon detector
const lsapi_device_TypeT lsapi_device_Type_GentecIntegraSiHi   = 0x0008000000000000;    ///< A Gentec Integra high level silicon detector
const lsapi_device_TypeT lsapi_device_Type_GentecIntegraSiUV   = 0x0008000000000001;    ///< A Gentec Integra UV silicon detector
const lsapi_device_TypeT lsapi_device_Type_TDKLambda_GEN100_7_5= 0x0010000000000000;    ///< A TDK-Lambda GEN100-7.5 DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_TDKLambda_GEN30_25  = 0x0010000000000001;    ///< A TDK-Lambda GEN30-25 DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_TDKLambda_GEN150_5  = 0x0010000000000002;    ///< A TDK-Lambda GEN150-5 DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_Ametek1501ix        = 0x0020000000000000;    ///< An Ametek 1501ix AC Power Supply
const lsapi_device_TypeT lsapi_device_Type_LabsphereVA         = 0x0040000000000000;    ///< A variable attenuator controlled by a Labsphere Teensy board
const lsapi_device_TypeT lsapi_device_Type_XIMC_Iris           = 0x0040000000000001;    ///< A motorized iris controlled by an XIMC
const lsapi_device_TypeT lsapi_device_Type_LabsphereLCUSS_VA   = 0x0040000000000002;    ///< A variable attenuator controlled by Labsphere LCUSS BeagleBoneBlack
const lsapi_device_TypeT lsapi_device_Type_LabsphereLCUSS_FS   = 0x0040000000000003;    ///< A filter slide controlled by Labsphere LCUSS BeagleBoneBlack
const lsapi_device_TypeT lsapi_device_Type_LuximSTA40          = 0x0080000000000000;    ///< A Luxim STA40 plasma lamp controller
const lsapi_device_TypeT lsapi_device_Type_AmetekXG            = 0x0100000000000000;    ///< An Ametek/Sorensen XG series DC Power Supply
const lsapi_device_TypeT lsapi_device_Type_LabsphereHFW        = 0x0200000000000000;    ///< A Labsphere Helios Filter Wheel
const lsapi_device_TypeT lsapi_device_Type_LabsphereFXC        = 0x0200000000000001;    ///< A Labsphere "FilterFan" controlled by an LSFXC
const lsapi_device_TypeT lsapi_device_Type_NewportUSFW         = 0x0200000000000002;    ///< A Newport/Oriel USFW (filterwheel)
const lsapi_device_TypeT lsapi_device_Type_AtikEFW             = 0x0200000000000003;    ///< An Atik Cameras EFW (filterwheel)
const lsapi_device_TypeT lsapi_device_Type_GentecIntegraInGaAs      = 0x0400000000000000;   ///< A Gentec Integra InGaAs detector, Extended (Model PH3-In-2.6T-IN) with TEC
const lsapi_device_TypeT lsapi_device_Type_GentecIntegraInGaAs1_7T  = 0x0400000000000001;   ///< A Gentec Integra InGaAs detector, Standard (Model PH3-In-1.7T-IN) with TEC
const lsapi_device_TypeT lsapi_device_Type_GentecIntegraFK10BSiL    = 0x0400000000000002;   ///< A Gentec Integra FK10BSi flickermeter
const lsapi_device_TypeT lsapi_device_Type_GentecIntegraFK3BInGaAs  = 0x0400000000000003;   ///< A Gentec Integra FK3BInGaAs flickermeter
const lsapi_device_TypeT lsapi_device_Type_XeC_Lamp            = 0x0800000000000000;    ///< A Lamp connected to a Labsphere XeC
const lsapi_device_TypeT lsapi_device_Type_CSZ_CP              = 0x0800000000000001;    ///< A Cincinnati Sub-Zero Temperature control chamber used as an LM-82 controller
const lsapi_device_TypeT lsapi_device_Type_LabsphereATS        = 0x0800000000000002;    ///< An LED driver board, used in CCS units
const lsapi_device_TypeT lsapi_device_Type_LabsphereHFS        = 0x0800000000000003;    ///< An automated filter slide used in HELIOS
const lsapi_device_TypeT lsapi_device_Type_LabsphereLPS        = 0x0800000000000004;    ///< Labsphere Power Supply
const lsapi_device_TypeT lsapi_device_Type_NumatoRelay_Lamp    = 0x0800000000000005;    ///< A Lamp connected to a Numato relay module
const lsapi_device_TypeT lsapi_device_Type_VektrexSSM          = 0x0800000000000006;    ///< Vektrex SpikeSafe MiniPower Supply
const lsapi_device_TypeT lsapi_device_Type_LabsphereMultiLPS   = 0x0800000000000007;    ///< Labsphere multi-channel Power Supply, typically for Blue Sun systems


//============================================================
// Commands for lsapi_ApiCall_Device_ControlDevice
/*! Indicates the general catagory a particular device falls into, and thus how it is used  */
typedef enum
{
    lsapi_device_DeviceClass_Generic            = 0,    // Special value, analog of OLD 'lsapi_CmdClass_Device'. NOTE: To Catalog all Devices, use "CatalogDeviceClass" with 'ClassToFind = lsapi_device_DeviceClass_Generic' 
    lsapi_device_DeviceClass_Spectrometer       = 1,
    lsapi_device_DeviceClass_PowerSupply        = 2,
    lsapi_device_DeviceClass_TempController     = 3,
    lsapi_device_DeviceClass_RelayController    = 4,
    lsapi_device_DeviceClass_PowerMeter         = 5,
    lsapi_device_DeviceClass_MultiMeter         = 6,
    lsapi_device_DeviceClass_TempMonitor        = 7,
    lsapi_device_DeviceClass_PhotometryModule   = 8,
    lsapi_device_DeviceClass_LM82Controller     = 9,
    lsapi_device_DeviceClass_Detector           = 10,
    lsapi_device_DeviceClass_VariableAttenuator = 11,
    lsapi_device_DeviceClass_PlasmaLamp         = 12,
    lsapi_device_DeviceClass_FilterWheel        = 13,
    lsapi_device_DeviceClass_Lamp               = 14,
    lsapi_device_DeviceClass_LED_Driver         = 15,
    lsapi_device_DeviceClass_TransientDetector  = 16,
} lsapi_device_DeviceClassT;



/*! Defines the lsapi_ApiCall_Device_ControlDeviceHdr.SubCmd values for ::lsapi_ApiCall_Device_ControlDevice */
typedef unsigned long lsapi_DeviceControlT;
    
const lsapi_DeviceControlT lsapi_device_Control_Unknown                     = 0; ///< This value is a placeholder and is not used.
// Device "base class" commands
const lsapi_DeviceControlT lsapi_device_Control_Generic_GetDeviceInfo       = 1; ///< Retrieve information about the specified device. \n Used with ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params
const lsapi_DeviceControlT lsapi_device_Control_Generic_SetDeviceInfo       = 2; ///< Retrieve information for the specified device. \n Used with ::lsapi_ApiCall_Device_ControlDevice_SetDeviceInfo_Params
const lsapi_DeviceControlT lsapi_device_Control_Generic_GetDeviceState      = 3; ///< Retrieve the subset of GetDeviceInfo_Params for mutable Device state information. \n Used with ::lsapi_ApiCall_Device_ControlDevice_GetDeviceState_Params
// Spectrometer commands
const lsapi_DeviceControlT lsapi_device_Control_Spectrometer_StartScan      = 1; ///< Start a scan. \n Used with ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_StartScan_Params
const lsapi_DeviceControlT lsapi_device_Control_Spectrometer_AutoExposureSaturationLimits = 2; ///< Set or retrieve the saturation limits for an Autoexposure scan. \n Used with ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_AutoExposureSaturationLimits_Params
const lsapi_DeviceControlT lsapi_device_Control_Spectrometer_TEC            = 3; ///< Control the Spectrometer's Thermo Electric Cooler. \n Only supported for spectrometers that incorporate a TEC. \n Used with ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_TEC_Params
const lsapi_DeviceControlT lsapi_device_Control_Spectrometer_BaselineOffset = 4; ///< Check the availability of, and/or clear, a spectrometer's Baseline Offset data. \n Only supported for spectrometers that do <b>not</b> have a shutter.\n Used with ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_BaselineOffset_Params
const lsapi_DeviceControlT lsapi_device_Control_Spectrometer_ShutterMode    = 5; ///< Set the Spectrometer's shutter mode to one of the ::lsapi_device_spectrometer_ShutterModeT values. \n Only supported for spectrometers that incorporate a Shutter. \n Used with ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_ShutterMode_Params
// Power Supply commands
const lsapi_DeviceControlT lsapi_device_Control_PowerSupply_Item            = 1; ///< Get and/or Set a Power Supply's setting. \n Used with ::lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params
// Temperature Controller commands
const lsapi_DeviceControlT lsapi_device_Control_TempController_Item         = 1; ///< Get and/or Set a Temperature Controller's setting. \n Used with ::lsapi_ApiCall_Device_ControlDevice_TempController_Item_Params
// Relay Controller commands
const lsapi_DeviceControlT lsapi_device_Control_RelayController_Item        = 1; ///< Set or Read a measured value from the Relay Controller. \n Used with ::lsapi_ApiCall_Device_ControlDevice_RelayController_Item_Params
// Power Meter commands
const lsapi_DeviceControlT lsapi_device_Control_PowerMeter_Item             = 1; ///< Get measurement data from a Power Meter. \n Used with ::lsapi_ApiCall_Device_ControlDevice_PowerMeter_Item_Params
// Multimeter commands
const lsapi_DeviceControlT lsapi_device_Control_MultiMeter_Item             = 1; ///< Get measurement data from a Multimeter. \n Used with ::lsapi_ApiCall_Device_ControlDevice_MultiMeter_Item_Params
// Temperature Monitor commands
const lsapi_DeviceControlT lsapi_device_Control_TempMonitor_Item            = 1; ///< Get and/or Set a Temperature Monitor's setting. \n Used with ::lsapi_ApiCall_Device_ControlDevice_TempMonitor_Item_Params
// Photometry Module commands
const lsapi_DeviceControlT lsapi_device_Control_PhotometryModule_Item       = 1; ///< Set or Read a measured value from the Photometry Module. \n Used with ::lsapi_ApiCall_Device_ControlDevice_PhotometryModule_Item_Params
// LM-82 Module commands
const lsapi_DeviceControlT lsapi_device_Control_LM82Controller_Item         = 1; ///< Set or Read a measured value from the LM82 Controller. \n Used with ::lsapi_ApiCall_Device_ControlDevice_LM82Controller_Item_Params
// Detector commands
const lsapi_DeviceControlT lsapi_device_Control_Detector_Item               = 1; ///< Set or Read a measured value from the Detector. \n Used with ::lsapi_ApiCall_Device_ControlDevice_Detector_Item_Params
// Variable Attenuator commands
const lsapi_DeviceControlT lsapi_device_Control_VA_Item                     = 1; ///< Get and/or Set a Variable Attenuator's setting. \n Used with ::lsapi_ApiCall_Device_ControlDevice_VA_Item_Params
// Plasma Lamp commands
const lsapi_DeviceControlT lsapi_device_Control_PlasmaLamp_Cmd              = 1; ///< Send command to plasma controller. \n Used with ::lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Cmd_Params
const lsapi_DeviceControlT lsapi_device_Control_PlasmaLamp_Status           = 2; ///< Get status data. \n Used with ::lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Status_Params
// Filter Wheel commands
const lsapi_DeviceControlT lsapi_device_Control_FW_Item                     = 1; ///< Get and/or Set a FilterWheel's setting. \n Used with ::lsapi_ApiCall_Device_ControlDevice_FW_Item_Params
// Lamp Commands
const lsapi_DeviceControlT lsapi_device_Control_Lamp_TurnOn                 = 1; ///< Turn on a lamp device. \n  Used with ::lsapi_ApiCall_Device_ControlDeviceHdr.
const lsapi_DeviceControlT lsapi_device_Control_Lamp_TurnOff                = 2; ///< Turn off a lamp device. \n  Used with ::lsapi_ApiCall_Device_ControlDeviceHdr.
const lsapi_DeviceControlT lsapi_device_Control_Lamp_Status                 = 3; ///< Check the current on/off status of a lmap device.  \n Used with ::lsapi_ApiCall_Device_ControlDevice_Lamp_Status_Params
// LED Driver Commands
const lsapi_DeviceControlT lsapi_device_Control_LED_Driver_CalLamp_Enable   = 1; ///< Enable/disable Cal Lamp Mode, which allows one of the channels to run at a higher current that can drive a calibration lamp. \n Used with ::lsapi_ApiCall_Device_ControlDevice_LED_Driver_EnableMode_Params
const lsapi_DeviceControlT lsapi_device_Control_LED_Driver_SetChannels      = 2; ///< Set the current of the specified channels. \n Used with ::lsapi_ApiCall_Device_ControlDevice_LED_Driver_Channels_Params
const lsapi_DeviceControlT lsapi_device_Control_LED_Driver_ReadChannels     = 3; ///< Read the current of the specified channels. \n Used with ::lsapi_ApiCall_Device_ControlDevice_LED_Driver_Channels_Params
const lsapi_DeviceControlT lsapi_device_Control_LED_Driver_Trigger_Enable   = 4; ///< Enable/disable trigger mode. \n Used with ::lsapi_ApiCall_Device_ControlDevice_LED_Driver_EnableMode_Params
// Transient Detector commands
const lsapi_DeviceControlT lsapi_device_Control_TransientDetector_StartScan = 1; ///< Start a scan; equivalent to Arm-Fire sequence.\n Used with ::lsapi_device_Control_TransientDetector_StartScan_Params
const lsapi_DeviceControlT lsapi_device_Control_TransientDetector_Arm       = 2; ///< Stop background scanning, freeze auto-ranging at the current radiativity level, set device for params specfied by ::lsapi_device_Control_TransientDetector_Arm_Params.  Use AbortScan to return to normal operation.
const lsapi_DeviceControlT lsapi_device_Control_TransientDetector_Fire      = 3; ///< Start a scan following Arm. Arm-Fire sequence is useful for freezing range before a triggered scan synchronized with DUT; also, Fire latency is less than StartScan\n Used with ::lsapi_device_Control_TransientDetector_Fire_Params
const lsapi_DeviceControlT lsapi_device_Control_TransientDetector_AbortScan = 4; ///< Abort current scan or exit the Arm state. \n Used with ::lsapi_ApiCall_Device_ControlDeviceHdr

//===========================================================================================
// Device States
/*! A device's state. Retrieved via lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. */
typedef enum
{
    lsapi_device_DeviceState_Unknown                    = 0,    ///< Illegal/uninitialized value
    lsapi_device_DeviceState_Error                     = 90,    ///< The device is in an error state. Typical remedy is to disconnect then reconnect to the device. Cycling the device's power may also be required.
    lsapi_device_DeviceState_Idle                      = 91,    ///< The device is idle and is able to begin an operation.
    lsapi_device_DeviceState_Busy                      = 92,    ///< The device is busy (e.g. Spectrometer in the midst of a scan). Wait for the device to become Idle.
} lsapi_device_DeviceStateT;

//===========================================================================================
// Device Cataloging option flags
#define lsapi_device_CatalogOption_Supported                ((lsapi_device_CatalogOptionsT)0x00000001)  ///< Specify this flag (bitwise OR) to have Device catalogs return a list of Supported devices.
#define lsapi_device_CatalogOption_Connectable              ((lsapi_device_CatalogOptionsT)0x00000002)  ///< Specify this flag (bitwise OR) to have Device catalogs return a list of Connectable devices.

/** \def lsapi_device_CatalogOption_ForceRefresh
Specify this flag (bitwise OR) to force the Device catalog to refresh. \n
Because cataloging devices can be slow (depending upon their connection methods), the API caches its internal master list of connected devices. \n
This is suitable for most cataloging operations, but there are times (for example when a new USB device has been attached) when you explicity want to refresh the master list. \n
Specifying this flag will cause the the master list to be completely refreshed (and you incur the delay inherent in such a refresh).
*/
#define lsapi_device_CatalogOption_ForceRefresh             ((lsapi_device_CatalogOptionsT)0x00000004)
/** \def lsapi_device_CatalogOption_ClearComPortCache
Specify this flag (bitwise OR) to clear the api's cache file of what device was last seen on each serial port number. \n
Keeping this cache causes cataloging to complete much faster as the api doesn't need to check each port for several different device types. \n
This flag is mostly only used during development to test how cataloging will behave the first time a customer uses it.
*/
#define lsapi_device_CatalogOption_ClearComPortCache        ((lsapi_device_CatalogOptionsT)0x00000008)

//===========================================================================================
// Event IDs
/*! Identifiers for Events sent by a connected device to your ::lsapi_device_EventCallback. */
typedef enum
{
    lsapi_device_EventId_Unknown                                = 0,    ///< Unknown EventId
    // Generic Device Events
    lsapi_device_EventId_DeviceState_Change                     = 1,    ///< DeviceState (i.e. Busy|Idle|Error) changed; see also ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params DeviceState field.
    // Spectrometer Events
    lsapi_device_EventId_Spectrometer_Scan_Start                = 100,    ///< A requested scan has started
    lsapi_device_EventId_Spectrometer_Scan_Progress             = 101,    ///< Scan progress. \n Percent-done is contained in the ::lsapi_device_EventHeader's Value field. \n The pHdr parameter passed to your Callback is a ::lsapi_spectrometer_ScanEventData struct.
    lsapi_device_EventId_Spectrometer_Scan_Completed            = 102,    ///< A Scan has completed. \n The pHdr parameter passed to your Callback is a ::lsapi_spectrometer_ScanEventData struct; this contains the handle of the completed scan.
    lsapi_device_EventId_Spectrometer_Scan_Aborted              = 103,    ///< A Scan has aborted on request (via ::lsapi_scan_CtlFcn_Abort). \n The pHdr parameter passed to your Callback is a ::lsapi_spectrometer_ScanEventData struct; this contains the handle of the aborted scan.
    lsapi_device_EventId_Spectrometer_AutoExposure_InProgress   = 104,    ///< An Autoexposure is testing an integation time for saturation. \n The ::lsapi_device_EventHeader's Value field contains the integation time in milliseconds.
    lsapi_device_EventId_Spectrometer_AutoExposure_Completed    = 105,    ///< An Autoexposure has completed. \n Autoexposure results may be obtained via ::lsapi_scan_CtlFcn_GetAutoExposureInfo.
    lsapi_device_EventId_Spectrometer_AutoExposure_Failed       = 106,    ///< An Autoexposure has failed. \n Failure reason may be obtained via ::lsapi_scan_CtlFcn_GetAutoExposureInfo AexResult field.
    lsapi_device_EventId_Spectrometer_Scan_Failed               = 107,    ///< A Scan has failed due to error. Scan data is not valid.\n The pHdr parameter passed to your Callback is a ::lsapi_spectrometer_ScanEventData struct; this contains the handle of the failed scan.

    // Power Supply Events
    // ... (tbd)
    // PlasmaLamp Events
    lsapi_device_EventId_PlasmaLamp_Transition                  = 120,    ///< PlasmaLamp's state changed.
    // TransientDetector Events 
    lsapi_device_EventId_ScalarTransientScan_Transition         = 130,      ///< This lsapi_device_EventHeader::EventId is used with lsapi_transientdetector_ScalarTransientEventData
} lsapi_device_EventIdT;

//===========================================================================================
// Spectrometer stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define lsapi_device_spectrometer_FeatureFlag_Shutter                    ((lsapi_device_spectrometer_FeatureFlagsT)0x00000001)    ///< Spectrometer has a shutter
#define lsapi_device_spectrometer_FeatureFlag_TEC                        ((lsapi_device_spectrometer_FeatureFlagsT)0x00000002)    ///< Spectrometer has a TEC
#define lsapi_device_spectrometer_FeatureFlag_Filter                     ((lsapi_device_spectrometer_FeatureFlagsT)0x00000004)    ///< Spectrometer has a filter
#define lsapi_device_spectrometer_FeatureFlag_NonLinearityCorrection     ((lsapi_device_spectrometer_FeatureFlagsT)0x00000008)    ///< Spectrometer offers non-linearity correction
#define lsapi_device_spectrometer_FeatureFlag_ThermalDriftCorrection     ((lsapi_device_spectrometer_FeatureFlagsT)0x00000010)    ///< Spectrometer has dark pixels (for correcting thermal drift)
#define lsapi_device_spectrometer_FeatureFlag_StrayLightCorrection       ((lsapi_device_spectrometer_FeatureFlagsT)0x00000020)    ///< Spectrometer has stray light correction
#define lsapi_device_spectrometer_FeatureFlag_ExternalTrigger            ((lsapi_device_spectrometer_FeatureFlagsT)0x00000040)    ///< Spectrometer has an external (TTL) trigger
#define lsapi_device_spectrometer_FeatureFlag_SyncScan                   ((lsapi_device_spectrometer_FeatureFlagsT)0x00000080)    ///< Spectrometer can perfom synchronous (blocking) scans
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*! Virtual light sources. \n Used when installing a Virtual Spectrometer via ::lsapi_device_UtilityType_InstallVirtualDevice*/
typedef enum
{
    lsapi_device_spectrometer_VirtualLightSource_None              = 0,    ///< Illegal/uninitialized value
    lsapi_device_spectrometer_VirtualLightSource_WhiteLED          = 1,    ///< Data from an ideal spectrum
    lsapi_device_spectrometer_VirtualLightSource_QTH               = 2,    ///< Data from an ideal spectrum
    lsapi_device_spectrometer_VirtualLightSource_Fluorescent       = 3,    ///< Data hand-measured using a CDS30x0
} lsapi_device_spectrometer_VirtualLightSourceT;

/*! Shutter modes. \n Get and Set the shutter mode using ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_ShutterMode_Params and ::lsapi_device_Control_Spectrometer_ShutterMode */
typedef enum
{
    lsapi_device_spectrometer_ShutterMode_Query                    = 0,    ///< Shutter state will remain unchanged; current state will be returned
    lsapi_device_spectrometer_ShutterMode_Auto                     = 1,    ///< API will determine when it needs to open/close the shutter (i.e closed for Dark scans, open for DUT scans)
    lsapi_device_spectrometer_ShutterMode_Open                     = 2,    ///< Shutter will remain open until user spec's Closed or Auto (i.e Dark scans become DUT scans)
    lsapi_device_spectrometer_ShutterMode_Closed                   = 3,    ///< Shutter will remain closed until user spec's Open or Auto (i.e DUT scans become Dark scans)
} lsapi_device_spectrometer_ShutterModeT;

/*! Spectrometer filter values. \n Pass these in the Filter member of ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_StartScan_Params */
typedef enum
{
    lsapi_device_spectrometer_Filter_Unknown      =  0,     ///< Initial setting of lsapi_spectrometer_Filter.
    lsapi_device_spectrometer_Filter_None         = -1,     ///< Legal value for spectros that have no filter
    lsapi_device_spectrometer_Filter_Auto         = -2,     ///< Not an actual filter value; this is used as an API flag to signify "The API may/should select the filter"
    lsapi_device_spectrometer_Filter_CDS30x0_0    =  10,    ///< CDS30x0 Filter 0 = 100% transmission
    lsapi_device_spectrometer_Filter_CDS30x0_1    =  11,    ///< CDS30x0 Filter 1 = 10% transmission
    lsapi_device_spectrometer_Filter_CDS30x0_2    =  12,    ///< CDS30x0 Filter 2 = 1% transmission
} lsapi_device_spectrometer_FilterT;

//===========================================================================================
// Power Supply stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define lsapi_device_powersupply_FeatureFlag_RearTerminals             ((lsapi_device_powersupply_FeatureFlagsT)0x00000001)    ///< Power Supply has front/rear terminals. \n Used in the Attributes.PowerSupply member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_powersupply_FeatureFlag_RemoteSensing             ((lsapi_device_powersupply_FeatureFlagsT)0x00000002)    ///< Power Supply can do 4-wire sensing. \n Used in the Attributes.PowerSupply member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_powersupply_FeatureFlag_VoltageRange              ((lsapi_device_powersupply_FeatureFlagsT)0x00000004)    ///< Power Supply has multiple voltage ranges. \n Used in the Attributes.PowerSupply member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_powersupply_FeatureFlag_TriggerOutput             ((lsapi_device_powersupply_FeatureFlagsT)0x00000008)    ///< Power Supply has a trigger output. \n Used in the Attributes.PowerSupply member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_powersupply_FeatureFlag_CurrentSource             ((lsapi_device_powersupply_FeatureFlagsT)0x00000010)    ///< Power Supply can source current
#define lsapi_device_powersupply_FeatureFlag_ACFrequency               ((lsapi_device_powersupply_FeatureFlagsT)0x00000020)    ///< Power Supply has variable AC frequency
#define lsapi_device_powersupply_FeatureFlag_ComplianceCurrent         ((lsapi_device_powersupply_FeatureFlagsT)0x00000040)    ///< Power Supply supports compliance current
#define lsapi_device_powersupply_FeatureFlag_ACDC                      ((lsapi_device_powersupply_FeatureFlagsT)0x00000080)    ///< Power Supply supports switching between AC and DC modes
#define lsapi_device_powersupply_FeatureFlag_Inhibit                   ((lsapi_device_powersupply_FeatureFlagsT)0x00000100)    ///< Power Supply supports setting an inhibit line
#define lsapi_device_powersupply_FeatureFlag_RampRate                  ((lsapi_device_powersupply_FeatureFlagsT)0x00000200)    ///< Power Supply supports setting the ramp rate for voltage and current
#define lsapi_device_powersupply_FeatureFlag_ReverseBias               ((lsapi_device_powersupply_FeatureFlagsT)0x00000400)    ///< Power Supply supports setting negative values for voltage and current
#define lsapi_device_powersupply_FeatureFlag_Pulsing                   ((lsapi_device_powersupply_FeatureFlagsT)0x00000800)    ///< Power Supply supports pulsing
#define lsapi_device_powersupply_FeatureFlag_MultiChannel              ((lsapi_device_powersupply_FeatureFlagsT)0x00001000)    ///< Power Supply has multiple output channels
#define lsapi_device_powersupply_FeatureFlag_SourceModes               ((lsapi_device_powersupply_FeatureFlagsT)0x00002000)    ///< Power Supply has separate source modes for voltage and current

/*! Values that you can get/set to/from a Power Supply. \n Passed as the Item member of ::lsapi_device_Control_PowerSupply_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_powersupply_Item_Unknown=0,                   ///< Illegal/uninitialized value.
    lsapi_device_powersupply_Item_OutputEnabled,               ///< Enable/disable the power supply's output
    lsapi_device_powersupply_Item_Voltage_Output,              ///< Get/Set the power supply's output voltage
    lsapi_device_powersupply_Item_Voltage_Measured,            ///< Read the power supply's measured voltage
    lsapi_device_powersupply_Item_Voltage_Compliance,          ///< Get/Set the power supply's compliance (limiting) voltage
    lsapi_device_powersupply_Item_Current_Output,              ///< Get/Set the power supply's output current 
    lsapi_device_powersupply_Item_Current_Measured,            ///< Read the power supply's measured current
    lsapi_device_powersupply_Item_Current_Compliance,          ///< Get/Set the power supply's compliance (limiting) current
    lsapi_device_powersupply_Item_RemoteSensingEnabled,        ///< Enable/disable the power supply's remote (4 wire) sensing feature. \n Only valid for power supplies that offer remote sensing.
    lsapi_device_powersupply_Item_VoltageRange,                ///< Get/Set the power supply's voltage range. \n Only valid for power supplies that feature multiple ranges. \n See ::lsapi_device_powersupply_VoltageRangeT for valid values.
    lsapi_device_powersupply_Item_UseRearTerminals,            ///< Get/Set the power supply's usage of front or rear terminals. \n Only valid for power supplies that have multiple terminal sets.
    lsapi_device_powersupply_Item_AcFrequency,                 ///< Get/Set the power supply's AC Frequency. \n Only valid for power supplies that source AC and have settable frequency.
    lsapi_device_powersupply_Item_SoakTriggerMeasure,          ///< Perform a Soak/Trigger/Measure sequence. \n Pass a pointer to a lsapi_device_powersupply_Item_SoakTriggerMeasure_Value as 'Value'
    lsapi_device_powersupply_Item_ACDC,                        ///< Switch an AC supply to DC mode, or back to AC
    lsapi_device_powersupply_Item_Inhibit,                     ///< Set the inhibit line for a supply
    lsapi_device_powersupply_Item_VoltageRampRate,             ///< Set the rate at which the voltage ramps up when the output is turned on. \n Only valid for power supplies with lsapi_device_powersupply_FeatureFlag_RampRate
    lsapi_device_powersupply_Item_CurrentRampRate,             ///< Set the rate at which the current ramps up when the output is turned on. \n Only valid for power supplies with lsapi_device_powersupply_FeatureFlag_RampRate
    lsapi_device_powersupply_Item_IsLocked,                    ///< Read Only.  True if the powersupply is locked, and will not allow settings to be changed other than turning output on/off
    lsapi_device_powersupply_Item_StartPulsing,                ///< Set the power supply to ouput a continuous train of pulses. \n Only valid for power supplies with lsapi_device_powersupply_FeatureFlag_Pulsing
    lsapi_device_powersupply_Item_PulseTriggerMeasure,         ///< Perform a Soak/Trigger/Measure sequence, but with pulsed output \n Pass a pointer to a lsapi_device_powersupply_Item_PulseTriggerMeasure_Value as 'Value'
    lsapi_device_powersupply_Item_OutputChannel,               ///< Get/Set the current output channel of a multichannel powersupply.  All commands will operate on that channel until OutputChannel is changed again. \n Only valid for power supplies with lsapi_device_powersupply_FeatureFlag_MultiChannel
    lsapi_device_powersupply_Item_ChannelMaxVolts,             ///< Get the maximum volts for the current output channel of a multichannel power supply.
    lsapi_device_powersupply_Item_ChannelMaxAmps               ///< Get the maximum amps for the current output channel of a multichannel power supply.

} lsapi_device_powersupply_ItemT;

/*! Voltage ranges, for devices that support them. \n Used with ::lsapi_device_powersupply_Item_VoltageRange */
typedef enum
{
    lsapi_device_powersupply_VoltageRange_Unknown=0,   ///< The power supply's voltage range is unknown.
    lsapi_device_powersupply_VoltageRange_Default=1,   ///< This is the value for single-range devices
    lsapi_device_powersupply_VoltageRange_Low=1001,    ///< Low range for those devices that support low/high ranges. \n Actual range voltages vary by device; After setting range, retrieve ::lsapi_device_DeviceAttributes and inspect PowerSupply.MaxVolts for actual value.
    lsapi_device_powersupply_VoltageRange_High=1002,   ///< High range for those devices that support low/high ranges. \n Actual range voltages vary by device; After setting range, retrieve ::lsapi_device_DeviceAttributes and inspect PowerSupply.MaxVolts for actual value.
} lsapi_device_powersupply_VoltageRangeT;


/*! Virtual lamps. \n Used when installing a Virtual Power Supply via ::lsapi_device_UtilityType_InstallVirtualDevice*/
typedef enum
{
    lsapi_device_powersupply_VirtualLamp_None = 0,              ///< Illegal/uninitialized value
    lsapi_device_powersupply_VirtualLamp_SCL050,                ///< SCL-050 lamp: 4.4W, 4.2V, 1.05A
    lsapi_device_powersupply_VirtualLamp_SCL600,                ///< SCL-600 lamp: 35W, 12.8V, 2.73A
    lsapi_device_powersupply_VirtualLamp_SCL1400,               ///< SCL-1400 lamp: 75W, 28V, 2.68A
    lsapi_device_powersupply_VirtualLamp_AUX10,                 ///< AUX-10 lamp: 10W, 6V, 1.67A
    lsapi_device_powersupply_VirtualLamp_AUX35,                 ///< AUX-35 lamp: 35W, 12V, 2.92A
    lsapi_device_powersupply_VirtualLamp_AC_Incandescent_60W,   ///< "Standard" 60W incandescent tungesten filament AC bulb; 17 lm/W @ 120VAC
    lsapi_device_powersupply_VirtualLamp_AC_CFL_14W,            ///< "Typical" 14W CFL AC bulb; 60 lm/W
    lsapi_device_powersupply_VirtualLamp_AC_LED_10W,            ///< "Typical" 10W LED AC bulb; 100 lm/W
} lsapi_device_powersupply_VirtualLampT;

//===========================================================================================
// Temperature Controller stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define lsapi_device_tempcontroller_FeatureFlag_OutputPower		((lsapi_device_tempcontroller_FeatureFlagsT)0x00000001)    ///< Temperature Controller can read output voltage and current. \n Used in the Attributes.TempController member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo

/*! Values that you can get/set to/from a Temperature Controller. \n Passed as the Item member of ::lsapi_device_Control_TempController_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_tempcontroller_Item_Unknown = 0,      ///< Illegal/uninitialized value
    lsapi_device_tempcontroller_Item_OutputEnabled,    ///< Enable/disable the temperature controller's output.
    lsapi_device_tempcontroller_Item_SetpointTemp,     ///< Get/Set the temperature controller's setpoint temperature.
    lsapi_device_tempcontroller_Item_MeasuredTemp,     ///< Read the temperature controller's measured temperature.
	lsapi_device_tempcontroller_Item_OutputVoltage,    ///< Read the temperature controller's output voltage.
	lsapi_device_tempcontroller_Item_OutputCurrent,    ///< Read the temperature controller's output current.
	lsapi_device_tempcontroller_Item_Channel,          ///< Get/Set the temperature controller's channel.  All subsequent commands will be directed to that channel.
} lsapi_device_tempcontroller_ItemT;


//===========================================================================================
// Relay Controller stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// No lsapi_relaycontroller_FeatureFlag_* currently defined

/*! Values that you can get/set to/from a Relay Controller. \n Passed as the Item member of ::lsapi_device_Control_RelayController_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_relaycontroller_Item_Unknown = 0,              ///< Illegal/uninitialized value
    lsapi_device_relaycontroller_Item_Route,                    ///< Get/Set the relay controller's route and routing options. Passed Value contains the 32-bit address of a lsapi_device_relaycontroller_Item_Route_Value struct
    lsapi_device_relaycontroller_Item_Output_Voltage,           ///< Read the relay controller's measured output voltage, in Volts.
    lsapi_device_relaycontroller_Item_Output_Current,           ///< Read the relay controller's measured output current, in Amps.
    lsapi_device_relaycontroller_Item_Temperature,              ///< Read the relay controller's internal heat sink temperature, in degrees C.
    lsapi_device_relaycontroller_Item_Clean,                    ///< Perform a Clean operation on a channel. Passed Value contains the 32-bit address of a lsapi_device_relaycontroller_Item_Clean_Value struct
} lsapi_device_relaycontroller_ItemT;

#define lsapi_device_relaycontroller_RouteOption_EnableLoad1Resistor    0x00000001    ///< Enable/disable the relay controller's load resistor on input channel #1. Used only if RelayController has a Load Resistor.
#define lsapi_device_relaycontroller_RouteOption_Force                  0x00000002    ///< (Set-only) Attempts to force the route, regardless of currents that might be present in the controller. CAUTION: "force" may cause damaging transients to any connected DUTs.
#define lsapi_device_relaycontroller_RouteOption_VerifyResult           0x00000004    ///< (Set-only) When this option is set, the apicall blocks until the Route operation completes, and returns the result. When this option is NOT set, the apicall returns immediately, but operation can fail if circuits are active.

typedef struct lsapi_device_relaycontroller_Item_Route_Value  ///< Struct to be passed as the "Value" param to ::lsapi_device_relaycontroller_Item_Route
{
    lsapi_StructHdr hdr;                                        ///< hdr.StructSize must == sizeof(lsapi_device_relaycontroller_Item_Route_Value)
    size_t InputChannel;                                        ///< In|Out: The requested InputChannel. Note: "0" means the inputs are disconnected; to disconnect, use <b>Set=true</b> with <b>InputChannel=OutputChannel=0</b>.
    size_t OutputChannel;                                       ///< In|Out: The requested OutputChannel: Note: "0" means the outputs are disconnected; to disconnect, use <b>Set=true</b> with <b>InputChannel=OutputChannel=0</b>.
    unsigned long Options;                                      ///< In|Out: Bitmask of lsapi_device_relaycontroller_RouteOption_* values
} lsapi_device_relaycontroller_Item_Route_Value;

typedef struct lsapi_device_relaycontroller_Item_Clean_Value  ///< Struct to be passed as the "Value" param to ::lsapi_device_relaycontroller_Item_Clean
{
    lsapi_StructHdr hdr;                                        ///< hdr.StructSize must == sizeof(lsapi_device_relaycontroller_Item_Clean_Value)
    size_t Channel;                                             ///< The Channel to be cleaned.
    size_t NumCycles;                                           ///< The number of cycles. Pass 0 (zero) to use the device's default.
    size_t OpenTime;                                            ///< In miliseconds, relay-open time. Pass 0 (zero) to use the device's default.
    size_t CloseTime;                                           ///< In miliseconds, relay-close time. Pass 0 (zero) to use the device's default.
} lsapi_device_relaycontroller_Item_Clean_Value;

//===========================================================================================
// Power Meter stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define lsapi_device_powermeter_FeatureFlag_Frequency             ((lsapi_device_powermeter_FeatureFlagsT)0x00000001)    ///< Power Meter can measure AC Frequency \n Used in the Attributes.PowerMeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_powermeter_FeatureFlag_RangeVoltage          ((lsapi_device_powermeter_FeatureFlagsT)0x00000002)    ///< Power Meter supports voltage auto-range and manual ranges. \n Used in the Attributes.PowerMeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_powermeter_FeatureFlag_RangeCurrent          ((lsapi_device_powermeter_FeatureFlagsT)0x00000004)    ///< Power Meter supports current auto-range and manual ranges. \n Used in the Attributes.PowerMeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo

/*! Values that you can get from a Power Meter. \n Passed as the Item member of ::lsapi_device_Control_PowerMeter_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_powermeter_Item_Unknown = 0,                   ///< Illegal/uninitialized value
    lsapi_device_powermeter_Item_ManualRangeVoltage,            ///< Get/Set the power meter's voltage range, in Volts. \n Set to the voltage you expect to measure, then Get the power meter's resulting range.
    lsapi_device_powermeter_Item_UseAutoRangeVoltage,           ///< Get/Set whether the power meter should auto-range voltage measurements.
    lsapi_device_powermeter_Item_ManualRangeCurrent,            ///< Get/Set the power meter's current range, in Amps. \n Set to the current you expect to measure, then Get the power meter's resulting range.
    lsapi_device_powermeter_Item_UseAutoRangeCurrent,           ///< Get/Set whether the power meter should auto-range current measurements.
    lsapi_device_powermeter_Item_GetACData,                     ///< Get AC power data
} lsapi_device_powermeter_ItemT;


/*! Data available from lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value. */
typedef unsigned long lsapi_device_powermeter_GetACDataFlagsT;                                                  ///< A bitmask of AC Data values supplied by a Power Meter,  the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_None        = 0x00000000;       ///< Request PowerMeter to return no fields via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_Voltage     = 0x00000001;       ///< Request PowerMeter to return Voltage via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_Current     = 0x00000002;       ///< Request PowerMeter to return Current via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_Power       = 0x00000004;       ///< Request PowerMeter to return Power via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_Frequency   = 0x00000008;       ///< Request PowerMeter to return Frequency via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_Powerfactor = 0x00000010;       ///< Request PowerMeter to return Powerfactor via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_THD			= 0x00000020;       ///< Request PowerMeter to return THD via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.
const lsapi_device_powermeter_GetACDataFlagsT lsapi_device_powermeter_GetACData_All         = (lsapi_device_powermeter_GetACData_Voltage | lsapi_device_powermeter_GetACData_Current | lsapi_device_powermeter_GetACData_Power | lsapi_device_powermeter_GetACData_Frequency | lsapi_device_powermeter_GetACData_Powerfactor | lsapi_device_powermeter_GetACData_THD);///< Request PowerMeter to return all available data via ::lsapi_device_powermeter_Item_GetACData. \n Used in the RequestedFields member of ::lsapi_device_powermeter_Item_GetACData_Value.

typedef struct lsapi_device_powermeter_Item_GetACData_Value  ///< Struct to be passed as the "Value" param to ::lsapi_device_powermeter_Item_GetACData
{
    lsapi_StructHdr hdr;                                        ///< hdr.StructSize must == sizeof(lsapi_device_powermeter_Item_GetACData_Value)
    lsapi_device_powermeter_GetACDataFlagsT RequestedFields;    ///< In: Set bits for data fields to retrieve; see ::lsapi_device_powermeter_GetACDataFlagsT. \n <b>Note:</b> returns FeatureNotSupported if a requested field is not supported by this powermeter.
    double Voltage;                                             ///< Out: Voltage value (Volts, RMS)
    double Current;                                             ///< Out: Current value (Amps, RMS)
    double Power;                                               ///< Out: Power value (Watts)
    double Frequency;                                           ///< Out: Frequency value (Hz)
    double Powerfactor;                                         ///< Out: Powerfactor value (dimensionless)
    double ITHD;                                                ///< Out: Total Current Harmonic Distortion value (%)
    double VTHD;                                                ///< Out: Total Voltage Harmonic Distortion value (%)
} lsapi_device_powermeter_Item_GetACData_Value;


//===========================================================================================
// Multimeter stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define lsapi_device_multimeter_FeatureFlag_RearTerminals           ((lsapi_device_multimeter_FeatureFlagsT)0x00000001)    ///< Multimeter has front/rear terminals. \n Used in the Attributes.Multimeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_multimeter_FeatureFlag_RangeVDC                ((lsapi_device_multimeter_FeatureFlagsT)0x00000002)    ///< Multimeter has multiple voltage ranges. \n Used in the Attributes.Multimeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_multimeter_FeatureFlag_Current                 ((lsapi_device_multimeter_FeatureFlagsT)0x00000004)    ///< Multimeter can measure DC current. \n Used in the Attributes.Multimeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_multimeter_FeatureFlag_RangeIDC                ((lsapi_device_multimeter_FeatureFlagsT)0x00000008)    ///< Multimeter has multiple current ranges. \n Used in the Attributes.Multimeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_multimeter_FeatureFlag_Trigger                 ((lsapi_device_multimeter_FeatureFlagsT)0x00000010)    ///< Multimeter can accept an external trigger. \n Used in the Attributes.Multimeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo

/*! Values that you can get from a Multimeter. \n Passed as the Item member of ::lsapi_device_Control_MultiMeter_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_multimeter_Item_Unknown = 0,                   ///< Illegal/uninitialized value
    lsapi_device_multimeter_Item_VoltageDC,                     ///< Read the multimeter's measured DC voltage, in Volts
    lsapi_device_multimeter_Item_ManualRangeVDC,                ///< Get/Set the multimeter's DC voltage range. \n Set to the voltage you expect to measure, then Get the multimeter's resulting range.
    lsapi_device_multimeter_Item_UseAutoRangeVDC,               ///< Get/Set whether the multimeter should auto-range for DC voltage measurements.
    lsapi_device_multimeter_Item_UseRearTerminals,              ///< Get/Set the multimeter's usage of front or rear terminals. \n Only valid for multimeters that have multiple terminal sets.
    lsapi_device_multimeter_Item_CurrentDC,                     ///< Read the multimeter's measured DC current, in Amps
    lsapi_device_multimeter_Item_ManualRangeIDC,                ///< Get/Set the multimeter's DC current range. \n Set to the current you expect to measure, then Get the multimeter's resulting range.
    lsapi_device_multimeter_Item_UseAutoRangeIDC,               ///< Get/Set whether the multimeter should auto-range for DC current measurements.
    lsapi_device_multimeter_Item_PrepTrigVDC,                   ///< Prepare the multimeter for a triggered DC voltage reading, with an optional ms delay after receiving trigger.  Obtain reading with lsapi_device_multimeter_Item_VoltageDC once completed.
    lsapi_device_multimeter_Item_PrepTrigIDC,                   ///< Prepare the multimeter for a triggered DC current reading, with an optional ms delay after receiving trigger.  Obtain reading with lsapi_device_multimeter_Item_CurrentDC once completed.
    lsapi_device_multimeter_Item_AbortTrig,                     ///< Abort a triggered reading.
    lsapi_device_multimeter_Item_Averaging,                     ///< Get/Set the number of readings to use in a repeating average.  Set 1 to turn averaging off.
    lsapi_device_multimeter_Item_MeasAperture,                  ///< Get/Set the measurement aperture for the device in ms.  Higher values reduce noise but increase measurement time.
} lsapi_device_multimeter_ItemT;

//===========================================================================================
// Temperature Monitor stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define lsapi_device_tempmonitor_FeatureFlag_FCompliant           ((lsapi_device_multimeter_FeatureFlagsT)0x00000001)    ///< Temperature Monitor reads F¡ãvalues. \n Used in the Attributes.Multimeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo
#define lsapi_device_tempmonitor_FeatureFlag_CCompliant           ((lsapi_device_multimeter_FeatureFlagsT)0x00000002)    ///< Temperature Monitor reads C¡ãvalues. \n Used in the Attributes.Multimeter member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo

/*! Values that you can get from a TemperatureMonitor. \n Passed as the Item member of ::lsapi_device_Control_TempMonitor_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_tempmonitor_Item_Unknown = 0,                   ///< Illegal/uninitialized value
    lsapi_device_tempmonitor_Item_Temperature,                   ///< Read the temperature monitor temp reading
    lsapi_device_tempmonitor_Item_ChannelType,                   ///< Set channel type
} lsapi_device_tempmonitor_ItemT;

/*! Indicates the type of sensor connected to a TemperatureMonitor's channel. */
typedef enum
{
    lsapi_device_tempmonitor_ChannelType_Unknown = 0,
    lsapi_device_tempmonitor_ChannelType_B,
    lsapi_device_tempmonitor_ChannelType_E,
    lsapi_device_tempmonitor_ChannelType_J,
    lsapi_device_tempmonitor_ChannelType_K,
    lsapi_device_tempmonitor_ChannelType_N,
    lsapi_device_tempmonitor_ChannelType_R,
    lsapi_device_tempmonitor_ChannelType_S,
    lsapi_device_tempmonitor_ChannelType_T,
} lsapi_device_tempmonitor_ChannelTypeT;


//==============================================================
// Photometry Module stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define lsapi_device_photometrymodule_FeatureFlag_LampSupply           ((lsapi_device_photometrymodule_FeatureFlagsT)0x00000001)    ///< Photometry Module has an internal lamp power supply. \n Used in the Attributes.PhotometryModule member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo

/*! Values that you can get from a Photometry Module. \n Passed as the Item member of ::lsapi_device_Control_PhotometryModule_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_photometrymodule_Item_Unknown = 0,                 ///< Illegal/uninitialized value
    lsapi_device_photometrymodule_Item_AllOff,                      ///< Turn all LEDs off, turn off internal LPS, and open all ports (Value is ignored)
    lsapi_device_photometrymodule_Item_OpenPort,                    ///< Close the specified port (Value: One of the lsapi_device_photometrymodule_PortT values)
    lsapi_device_photometrymodule_Item_ClosePort,                   ///< Open the specified port (Value: One of the lsapi_device_photometrymodule_PortT values)
    lsapi_device_photometrymodule_Item_CleanPort,                   ///< Clean the specified port (Value: One of the lsapi_device_photometrymodule_PortT values)
    lsapi_device_photometrymodule_Item_IndicatorOff,                ///< Turn the specified indicator (front-panel LED) off (Value: One of the lsapi_device_photometrymodule_IndicatorT values)
    lsapi_device_photometrymodule_Item_IndicatorOn,                 ///< Turn the specified indicator (front-panel LED) on (Value: One of the lsapi_device_photometrymodule_IndicatorT values)
    lsapi_device_photometrymodule_Item_LampPower,                   ///< Turn the Lamp Power (internal LPS) off (Value: 0=off, non-zero=on)
} lsapi_device_photometrymodule_ItemT;

typedef enum                                                        ///< Value identifying a port on a photometry module
{
    lsapi_device_photometrymodule_port_Unknown = 0,                 ///< Illegal/uninitialized value
    lsapi_device_photometrymodule_port_src_LampPower,               ///< The Lamp Power source (internal LPS) port
    lsapi_device_photometrymodule_port_src_AC,                      ///< The AC power supply source port
    lsapi_device_photometrymodule_port_src_User,                    ///< The User (external source) source port
    lsapi_device_photometrymodule_port_load_2pi,                    ///< The 2 pi Cal lamp load port
    lsapi_device_photometrymodule_port_load_auxlamp,                ///< The Aux lamp load port
    lsapi_device_photometrymodule_port_load_4pi,                    ///< The 4 pi Cal/DUT lamp (post) load port
} lsapi_device_photometrymodule_PortT;

typedef enum                                                        ///< Value identifying an LED on a photometry module
{
    lsapi_device_photometrymodule_indicator_Unknown = 0,            ///< Illegal/uninitialized value
    lsapi_device_photometrymodule_indicator_OutputOn,               ///< The blue "Output" LED
    lsapi_device_photometrymodule_indicator_PM,                     ///< The green "PM" (Lamp Power / internal LPS) LED
    lsapi_device_photometrymodule_indicator_AC,                     ///< The green "AC" LED
    lsapi_device_photometrymodule_indicator_User,                   ///< The green "User" LED
    lsapi_device_photometrymodule_indicator_Aux,                    ///< The green "Aux" LED
    lsapi_device_photometrymodule_indicator_Cal,                    ///< The green "Cal" LED
    lsapi_device_photometrymodule_indicator_DUT,                    ///< The green "DUT" LED
} lsapi_device_photometrymodule_IndicatorT;

//==============================================================
// LM-82 stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*! Values that you can get from an LM-82 Controller. \n Passed as the Item member of ::lsapi_device_Control_LM82Controller_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_LM82_Item_Unknown = 0,                             ///< Illegal/uninitialized value
    lsapi_device_LM82_Item_State,                                   ///< Set device state to one of the lsapi_device_lm82controller_StateT values
    lsapi_device_LM82_Item_MeasureTemperature,                      ///< Measure one of the lsapi_device_lm82controller_SensorT values
    lsapi_device_LM82_Item_TargetTemperature,                       ///< Set the temperature control target temperature
    lsapi_device_LM82_Item_TargetSensor,                            ///< Set the target sensor for temperature control to one of the lsapi_device_lm82controller_SensorT values
} lsapi_device_lm82controller_ItemT;

typedef enum                                                        ///< Value identifying operating states of an LM82 Controller
{
    lsapi_device_lm82_state_unknown = 0,                            ///< Illegal/uninitialized value
    lsapi_device_lm82_state_standby,                                ///< Device is in shutdown (display & TEC off)
    lsapi_device_lm82_state_idle,                                   ///< Device is idle (display on, TEC off)
    lsapi_device_lm82_state_active,                                 ///< Device is active (display & TEC on)
} lsapi_device_lm82controller_StateT;

typedef enum                                                        ///< Value used to identify a particular sensor of an LM82 Controller
{
    lsapi_device_lm82_sensor_unknown = 0,                           ///< Illegal/uninitialized value
    lsapi_device_lm82_sensor_td,                                    ///< Td
    lsapi_device_lm82_sensor_tb,                                    ///< Tb
    lsapi_device_lm82_sensor_tint,                                  ///< Can-internal temp
    lsapi_device_lm82_sensor_text,                                  ///< Can-external (sphere) temp
} lsapi_device_lm82controller_SensorT;

//==============================================================
// Detector stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define lsapi_device_detector_FeatureFlag_VoltageMode				((lsapi_device_detector_FeatureFlagsT)0x00000001)    ///< Detector can switch between current and voltage readings. \n Used in the Attributes.Detector member of ::lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params. \n Retrieved via ::lsapi_device_Control_Generic_GetDeviceInfo

/*! Values that you can get from a Detector. \n Passed as the Item member of ::lsapi_device_Control_Detector_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_Detector_Item_Unknown = 0,                         ///< Illegal/uninitialized value
    lsapi_device_Detector_Item_Read,                                ///< Retrieve the detector's (averaged) reading. Get-only
    lsapi_device_Detector_Item_AveragingPeriod,                     ///< Get/Set the averaging period (seconds). Note: Set "0" to clear samples, and Read only most-recent sample (without averaging).
    lsapi_device_Detector_Item_ZeroDarkInput,                       ///< Get: Value of 1 returned if detector is "zeroed", 0 if not. Set: "zero" the detector with darkened input; Value returns estimated time (sec). \n RE Set: monitor DeviceState Busy/Idle for completion (via ::lsapi_device_Control_Generic_GetDeviceInfo).
	lsapi_device_Detector_Item_VoltageMode,							///< Get/Set whether the detector is reading voltage instead of current. 0=current, 1=voltage. \n Only valid for detectors with lsapi_device_detector_FeatureFlag_VoltageMode.
} lsapi_device_detector_ItemT;

//==============================================================
// VariableAttenuator stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*! Values that you can get from a Variable Attenuator. \n Passed as the Item member of ::lsapi_device_Control_VA_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_VA_Item_Unknown = 0,      ///< Illegal/uninitialized value
    lsapi_device_VA_Item_MaxCounts,        ///< Get the VA's maximum counts.
    lsapi_device_VA_Item_PositionCounts,   ///< Get/Set the VA's position in counts.
    lsapi_device_VA_Item_PositionPercent,  ///< Get/Set the VA's position in percent of max counts.
    lsapi_device_VA_Item_Reset,            ///< Command the VA to reinitialize
    lsapi_device_VA_Item_Error,            ///< Get the last error code.
} lsapi_device_VA_ItemT;

//==============================================================
// FilterWheel stuff
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*! Values that you can get from a FilterWheel. \n Passed as the Item member of ::lsapi_device_Control_FW_Item in a ::lsapi_ApiCall_Device_ControlDevice call. */
typedef enum
{
    lsapi_device_FW_Item_Unknown = 0,      ///< Illegal/uninitialized value
    lsapi_device_FW_Item_Home,             ///< Commands the FW to Home
    lsapi_device_FW_Item_MinPosition,      ///< Get the FW's minimum position setting
    lsapi_device_FW_Item_MaxPosition,      ///< Get the FW's maximum position setting
    lsapi_device_FW_Item_Position,         ///< Get/Set the FW's position seting
} lsapi_device_FW_ItemT;


//===========================================================================================
// 
/*! Utility command types. \n Valid values for the Utilitytype member of ::lsapi_ApiCall_Device_Utilities */
typedef enum    ///< Values for lsapi_ApiCall_Device_UtilitiesHdr.UtilityType
{
    lsapi_device_UtilityType_Unknown = 0,                   ///< Illegal/uninitialized value
    lsapi_device_UtilityType_InstallCDS30x0WaveCalFile,     ///< Install a Wave Calibration file for a CDS30x0 spectrometer. \n Used with ::lsapi_ApiCall_Device_Utilities_InstallCDS30x0WaveCal_Params
    lsapi_device_UtilityType_InstallVirtualDevice,          ///< Install a Virtual Device. \n Used with ::lsapi_ApiCall_Device_Utilities_InstallVirtualDevice_Params
    lsapi_device_UtilityType_GetBaselineOffsetInfo,         ///< Retrieve Baseline Offset info for a spectrometer. \n Used with ::lsapi_ApiCall_Device_Utilities_GetBaselineOffsetInfo_Params
    lsapi_device_UtilityType_SetStrayLightFactor,           ///< Set the Stray Light correction factor for a spectrometer. \n Used with ::lsapi_ApiCall_Device_Utilities_SetStrayLightFactor_Params
    lsapi_device_UtilityType_GetStrayLightFactors,          ///< Get all known the Stray Light correction factors for all spectrometers. \n Used with ::lsapi_ApiCall_Device_Utilities_GetStrayLightFactors_Params
} lsapi_device_UtilityTypeT;

//===========================================================================================
//===========================================================================================
// Structs
//===========================================================================================

//---------------------
// Simple "supporting" structs - these don't have headers and are used inside more complex structs
//---------------------

// Used by many other structs
typedef struct lsapi_device_DeviceInfo ///< Contains information about a specific Device. \n Member of several other structs.
{
    // No header - this is a simple support struct
    lsapi_device_TypeT DeviceType;                          ///< Indicates the type of device
    lsapi_device_DeviceClassT DeviceClass;                  ///< Indicates the DeviceClass of the device
    lsapi_device_DeviceClassT DeviceClassSecondary;         ///< Indicates the Secondary DeviceClass, if any; none if lsapi_device_DeviceClass_Generic
    wchar_t SerialNumber[lsapi_device_MAX_STRINGLENGTH];    ///< The device-specific serial number
} lsapi_device_DeviceInfo;

typedef struct lsapi_WaveCount    ///< Helper struct for holding an associated wavelength and count.
{
    // No header - this is a simple support struct
    double Wavelength;      ///< Wavelength value
    double Count;           ///< Normalized count, in the range 0.0 .. 1.0
} lsapi_WaveCount;

typedef struct lsapi_Harmonic    ///< Helper struct for holding an associated frequency, amplitude, and phase.
{
    // No header - this is a simple support struct
    double FrequencyHz;     ///< Frequency value in cycles/second
    double Amplitude;       ///< Amplitude value
    double PhaseRad;        ///< Phase Angle value in radians
} lsapi_Harmonic;

typedef struct lsapi_device_AutoExposureSaturationLimits    ///< Helper struct for holding the low/high values of an Autoexposure range. \n Used by ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_AutoExposureSaturationLimits_Params
{
    // No header - this is a simple support struct
    double Low;                                             ///< This is the low end of the range. Values are in the range 0.0 .. 1.0
    double High;                                            ///< This is the high end of the range. Values are in the range 0.0 .. 1.0
} lsapi_device_AutoExposureSaturationLimits;

typedef struct lsapi_device_VirtualDeviceInfo   ///< Helper struct for installing a Virtual device. \n Used by lsapi_ApiCall_Device_Utilities_InstallVirtualDevice_Params
{
    // No header - this is a simple support struct
    lsapi_device_DeviceInfo DvcInfo;            ///< Specifies the ::lsapi_device_TypeT and serial number for the to-be-installed device. NOTE: DvcInfo members 'DeviceClass|DeviceClassSecondary' are ignored.
    struct
    {
        double UnityIntegrationMs;                          ///< The integration time that produces 75% saturation
        lsapi_device_spectrometer_VirtualLightSourceT LightSource; ///< This determines the waveform that is simulated
    } Spectrometer;    ///< These parameters are used when installing a virtual spectrometer.
    struct
    {
        lsapi_device_powersupply_VirtualLampT Lamp;    ///< The lamp to emulate.
    } PowerSupply;    ///< These parameters are used when installing a virtual power supply.
    struct
    {
        double AmbientTempOffset;       ///< The ambient temperature's offset from 22 degrees Celsius. \n Set to 0 for an ambient temp of 22C (72F, room temp); set to 10 for an ambient temp of 32C (90F); set to -22 for an ambient temp of 0C (32F, freezing point), etc.
    } TempController; ///< These parameters are used when installing a virtual temperature controller.
} lsapi_device_VirtualDeviceInfo;

#define LSAPI_MAX_FILTERS        10     ///< The maximum number of filters supported by any spectrometer

typedef struct lsapi_device_SpectrometerAttributes    ///< Spectrometer-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    lsapi_device_spectrometer_FeatureFlagsT FeatureFlags;    ///< Features supported by this spectrometer
    struct
    {
        double Min;         ///< Minimum integration time, in milliseconds
        double Max;         ///< Maximum integration time, in milliseconds
        double Increment;   ///< Integration time increment, in milliseconds
    } IntegrationMs;        ///< Integration time Attributes
    
    struct
    {
        int Start;          ///< The first (lowest) wavelength measured by the spectrometer.
        int End;            ///< The last (highest) wavelength measured by the spectrometer.
    } Wavelengths;          ///< Wavelength Attributes
    struct
    {
        size_t Count;                                                       ///< # of valid entries in 'Supported'
        lsapi_device_spectrometer_FilterT Supported[LSAPI_MAX_FILTERS];     ///< List of filters supported by this spectrometer.
        lsapi_device_spectrometer_FilterT Default;                          ///< The default filter for the spectrometer. May be Auto.
        lsapi_device_spectrometer_FilterT DefaultNonAuto;                   ///< The default filter for the spectrometer when Auto is NOT allowed.
    } Filter;   ///< Filter Attributes
} lsapi_device_SpectrometerAttributes;

typedef struct lsapi_device_PowerSupplyAttributes    ///< PowerSupply-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    lsapi_device_powersupply_FeatureFlagsT FeatureFlags;    ///< Features supported by this power supply
    double MaxVolts;        ///< Maximum voltage supported by the power supply.
    double MaxAmps;         ///< Maximum current supported by the power supply.
    struct
    {
        double Min;         ///< The minimum AC Frequency (Hertz) supported by this power supply. Only applicable for AC power supplies that support settable frequencies.
        double Max;         ///< The maximum AC Frequency (Hertz) supported by this power supply. Only applicable for AC power supplies that support settable frequencies.
    } ACFrequency;          ///< AC Frequency Attributes
    int NumChannels;        ///< The number of output channels of this power supply.  Only applicable for power supplies with lsapi_device_powersupply_FeatureFlag_MultiChannel.
} lsapi_device_PowerSupplyAttributes;

typedef struct lsapi_device_TempControllerAttributes    ///< TempController-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    lsapi_device_tempcontroller_FeatureFlagsT FeatureFlags;    ///< Features supported by this temperature controller
    double MinSetpoint;     ///< Minimum setpoint supported by this temperature controller.
    double MaxSetpoint;     ///< Maximum setpoint supported by this temperature controller.
    int NumChannels;        ///< Number of channels supported by this temperature controller.
} lsapi_device_TempControllerAttributes;

typedef struct lsapi_device_RelayControllerAttributes    ///< RelayController-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    lsapi_device_relaycontroller_FeatureFlagsT FeatureFlags;    ///< Features supported by this relay controller
    size_t NumInputChannels;   ///< Number of input channels supported by this relay controller.
    size_t NumOutputChannels;  ///< Number of output channels supported by this relay controller.
} lsapi_device_RelayControllerAttributes;

typedef struct lsapi_device_PowerMeterAttributes    ///< PowerMeter-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    lsapi_device_powermeter_FeatureFlagsT FeatureFlags;    ///< Features supported by this power meter
} lsapi_device_PowerMeterAttributes;

typedef struct lsapi_device_MultiMeterAttributes    ///< Multimeter-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    lsapi_device_multimeter_FeatureFlagsT FeatureFlags;    ///< Features supported by this multimeter
} lsapi_device_MultiMeterAttributes;

typedef struct lsapi_device_TemperatureMonitorAttributes    ///< TemperatureMonitor-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    size_t NumberOfChannels;                                ///< Number of channels supported by this temperature monitor
} lsapi_device_TemperatureMonitorAttributes;

typedef struct lsapi_device_TransientDetectorAttributes     ///< TransientDetector-specific attributes. Used by ::lsapi_device_DeviceAttributes.
{
    lsapi_scan_TriggerModeT TriggerModesSupported;          ///< Set of TriggerModes supported by this TransientDetector, bitwise OR'd together
} lsapi_device_TransientDetectorAttributes;

typedef struct lsapi_device_PhotometryModuleAttributes      ///< PhotometryModule-specific attributes.  Used by ::lsapi_device_DeviceAttributes.
{
	lsapi_device_photometrymodule_FeatureFlagsT FeatureFlags; ///< Features supported by this PhotometryModule
} lsapi_device_PhotometryModuleAttributes;

typedef struct lsapi_device_DetectorAttributes				///< Detector-specific attributes.  Used by ::lsapi_device_DeviceAttributes.
{
	lsapi_device_detector_FeatureFlagsT FeatureFlags; ///< Features supported by this Detector
} lsapi_device_DetectorAttributes;

typedef struct lsapi_device_LED_DriverAttributes            ///< LED_Driver-specific attributes.  Used by ::lsapi_device_DeviceAttributes.
{
    double MaxLEDAmps;                                      ///< normal maximum current setting in amps
    double MaxCalLampAmps;                                  ///< maximum current setting in amps for cal lamp channels when CalLampMode is enabled
    size_t NumChannels;                                     ///< number of channels supported by this device
    bool calLampModeSupported;                              ///< true if board can be set to Cal Lamp Mode, which allows channel 1 to be set to a higher current.
    bool triggerModeSupported;                              ///< true if board can be set to accept an external trigger
} lsapi_device_LED_DriverAttributes;

typedef struct lsapi_device_DeviceAttributes    ///< Attributes for a selected device. \n This is a general struct intended to support all device types, so the caller must know which set of fields (Spectrometer, PowerSupply, TempController) are valid.
{
    // No header - this is a support struct
    lsapi_device_SpectrometerAttributes Spectrometer;               ///< This member is populated when the device is a spectrometer.
    lsapi_device_PowerSupplyAttributes PowerSupply;                 ///< This member is populated when the device is a power supply.
    lsapi_device_TempControllerAttributes TempController;           ///< This member is populated when the device is a temperature controller.
    lsapi_device_RelayControllerAttributes RelayController;         ///< This member is populated when the device is a relay controller.
    lsapi_device_PowerMeterAttributes PowerMeter;                   ///< This member is populated when the device is a power meter.
    lsapi_device_MultiMeterAttributes MultiMeter;                   ///< This member is populated when the device is a multimeter.
    lsapi_device_TemperatureMonitorAttributes TemperatureMonitor;   ///< This member is populated when the device is a temperature monitor.
    lsapi_device_LED_DriverAttributes LED_Driver;                   ///< This member is populated when the device is an LED Driver
    lsapi_device_TransientDetectorAttributes TransientDetector;     ///< This member is populated when the device is a TransientDetector.
    lsapi_device_PhotometryModuleAttributes PhotometryModule;       ///< This member is populated when the device is a PhotometryModule.
	lsapi_device_DetectorAttributes Detector;						///< This member is populated when the device is a Detector.
} lsapi_device_DeviceAttributes;

typedef struct lsapi_ScanData               ///< Data read out of a scan (lsapi_scan_CtlFcn_GetData) or for reconstituting (lsapi_scan_CtlFcn_Reconstitute) a scan
{
    // No header - this is a simple support struct
    lsapi_scan_TypeT ScanType;                          ///< The type of scan for this data
    lsapi_WaveCount* pData;                             ///< Array of count-for-wavelength data. Must be pre-allocated by caller.
    size_t ElementCount;                                ///< # of WaveCount structs in 'pData'.
    lsapi_device_DeviceInfo SpectrometerInfo;           ///< DeviceType and Serial Number of the spectrometer that created this scan.
    lsapi_device_spectrometer_FilterT Filter;           ///< Spectrometer-model-specific filter, if any, that was used in creating this scan.
    double IntegrationMs;                               ///< Integration time used to obtain scandata
    double Timestamp;                                   ///< Time the originating scan was started, expressed as a number of seconds since Jan 1, 1970 (time_t)
} lsapi_ScanData;


//---------------------
// Compound structs - these all have headers
//---------------------

/*! Base struct passed to ::lsapi_device_EventCallback \n
Depending upon the EventId, this may be the complete set of data passed, or it may be the header for a larger struct. \n 
To be certain, check that the StructSize agrees with the expected size for the EventId before dereferencing. */
typedef struct lsapi_device_EventHeader
{
    lsapi_StructHdr Hdr;                    ///< Hdr.StructSize must == sizeof(actual struct); This will likely be greater than the sizeof(lsapi_device_EventHeader)
    lsapi_device_EventIdT EventId;          ///< Indicates the reason the callback was invoked
    lsapi_DeviceHandleT hDevice;            ///< Handle of the device that generated the event
    unsigned long Value;                    ///< EventId-specific value. See ::lsapi_device_EventIdT docs for usage.
} lsapi_device_EventHeader;

typedef struct lsapi_spectrometer_ScanEventData    ///< Event-data sent by a scan. \n ::lsapi_device_EventCallback's pHdr param points to the lsapi_spectrometer_ScanEventData struct's <b>hdr</b> member.
{
    lsapi_device_EventHeader hdr;           ///< Verify that hdr.StructSize equals sizeof(lsapi_spectrometer_ScanEventData) before casting the ::lsapi_device_EventCallback's pHdr param as a lsapi_spectrometer_ScanEventData*
    lsapi_SpectrometerScanHandleT hScan;    ///< Handle of the scan that generated the event.
} lsapi_spectrometer_ScanEventData;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Structs passed to lsapi_ApiCall_Device
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define LSAPI_MAX_CATALOG_DEVICES        1000     ///< The size of the arrays in ::lsapi_ApiCall_Device_CatalogDevices_Params.

typedef struct lsapi_ApiCall_Device_CatalogDevices_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Device_CatalogDevices 
{
    lsapi_ApiCallHdr ApiCallHdr;                                                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_device_CatalogOptionsT Options;                                       ///< In: Bitmask of ::lsapi_device_CatalogOptionsT flags
    lsapi_device_TypeT DevicesToFind[LSAPI_MAX_CATALOG_DEVICES];                ///< In: Array of devices to be found (0 terminated)
    lsapi_device_DeviceInfo SupportedDeviceTypes[LSAPI_MAX_CATALOG_DEVICES];    ///< Out: An array of Supported device types. Check SupportedDeviceCount to determine how many valid entries this array contains.
    size_t SupportedDeviceCount;                                                ///< Out: The number of valid entries in SupportedDeviceTypes[]
    lsapi_device_DeviceInfo ConnectableDevices[LSAPI_MAX_CATALOG_DEVICES];      ///< Out: An array of Connectable device types. Check ConnectableDeviceCount to determine how many valid entries this array contains.
    size_t ConnectableDeviceCount;                                              ///< Out: The number of valid entries in ConnectableDevices[]
} lsapi_ApiCall_Device_CatalogDevices_Params;

typedef struct lsapi_ApiCall_Device_CatalogDeviceClass_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Device_CatalogDeviceClass 
{
    lsapi_ApiCallHdr ApiCallHdr;                                                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_device_CatalogOptionsT Options;                                       ///< In: Bitmask of ::lsapi_device_CatalogOptionsT flags
    lsapi_device_DeviceClassT ClassToFind;                                      ///< In: Class of devices to be found.  Use lsapi_device_DeviceClass_Generic to find all devices.
    lsapi_device_DeviceInfo SupportedDeviceTypes[LSAPI_MAX_CATALOG_DEVICES];    ///< Out: An array of Supported device types. Check SupportedDeviceCount to determine how many valid entries this array contains.
    size_t SupportedDeviceCount;                                                ///< Out: The number of valid entries in SupportedDeviceTypes[]
    lsapi_device_DeviceInfo ConnectableDevices[LSAPI_MAX_CATALOG_DEVICES];      ///< Out: An array of Connectable device types. Check ConnectableDeviceCount to determine how many valid entries this array contains.
    size_t ConnectableDeviceCount;                                              ///< Out: The number of valid entries in ConnectableDevices[]
} lsapi_ApiCall_Device_CatalogDeviceClass_Params;

typedef struct lsapi_ApiCall_Device_ConnectDevice_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Device_ConnectDevice 
{
    lsapi_ApiCallHdr ApiCallHdr;                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_device_DeviceInfo DvcInfo;            ///< In: The type & serial number of the device you wish to connect to. NOTE: DvcInfo members 'DeviceClass|DeviceClassSecondary' are ignored.
    lsapi_DeviceHandleT hDevice;                ///< Out: Handle of device, or 0 on any error/failure
} lsapi_ApiCall_Device_ConnectDevice_Params;

/*! Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Device_ControlDevice. \n
Use DeviceClass and SubCmd values to specify the Control operation.
Most Control operations have a custom struct which includes this one as member 'lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr' \n
See the individual lsapi_ApiCall_Device_ControlDevice_* structs for details. */
typedef struct lsapi_ApiCall_Device_ControlDeviceHdr
{
    lsapi_ApiCallHdr ApiCallHdr;                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_device_DeviceClassT DeviceClass;      ///< In: Specify Device Class. Use lsapi_device_DeviceClass_Generic for Control operations which apply to all Device Classes, for example GetDeviceInfo.
    lsapi_DeviceControlT SubCmd;                ///< In: Specify Control operation
    lsapi_DeviceHandleT hDevice;                ///< In: Handle of device to control
} lsapi_ApiCall_Device_ControlDeviceHdr;

typedef struct lsapi_ApiCall_Device_UtilitiesHdr    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Device_Utilities
{
    lsapi_ApiCallHdr ApiCallHdr;                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_device_UtilityTypeT UtilityType;      ///< The type of utility to be performed.
} lsapi_ApiCall_Device_UtilitiesHdr;

//-------------------------------
// Structs based on lsapi_ApiCall_Device_ControlDeviceHdr

typedef struct lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params ///< Please see \link dox_w32_dvc_fcns_ctrl_generic_GetDeviceInfo GetDeviceInfo\endlink for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_DeviceInfo DvcInfo;                            ///< Out: The type of device & serial number associated with 'hDevice'
    wchar_t DeviceTypeString[lsapi_device_MAX_STRINGLENGTH];    ///< Out: A textual representation of DvcInfo.DeviceType
    wchar_t UniqueId[lsapi_device_MAX_STRINGLENGTH];            ///< Out: The API's (internal) unique ID for the device
    wchar_t DisplayName[lsapi_device_MAX_STRINGLENGTH];         ///< Out: The user-specified display name (set via ::lsapi_ApiCall_Device_ControlDevice_SetDeviceInfo_Params)
    lsapi_device_DeviceStateT DeviceState;                      ///< Out: The device's current state
    lsapi_device_DeviceAttributes Attributes;                   ///< Out: Device-specific attributes
} lsapi_ApiCall_Device_ControlDevice_GetDeviceInfo_Params;

typedef struct lsapi_ApiCall_Device_ControlDevice_GetDeviceState_Params ///< Retrieve the subset of GetDeviceInfo_Params for mutable Device state information. \n Used with ::lsapi_ApiCall_Device_ControlDevice_GetDeviceState_Params
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_DeviceStateT DeviceState;                      ///< Out: The device's current state
    lsapi_ResultT ErrorCode;                                    ///< Out: Error code if DeviceState==lsapi_device_DeviceState_Error
    wchar_t Details[lsapi_device_MAX_STRINGLENGTH];             ///< Out: Error details if DeviceState==lsapi_device_DeviceState_Error, else state details (if any)
} lsapi_ApiCall_Device_ControlDevice_GetDeviceState_Params;

typedef struct lsapi_ApiCall_Device_ControlDevice_SetDeviceInfo_Params ///< Please see \link dox_w32_dvc_fcns_ctrl_generic_SetDeviceInfo SetDeviceInfo\endlink for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;       ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.

    struct
    {
        bool Set;                                           ///< When set to <b>true</b> the API will set the device's display name to <i>Value</i>.
        wchar_t Value[lsapi_device_MAX_STRINGLENGTH];       ///< New value for the device's display name. \n Ignored unless <i>Set</i> is <b>true</b>.
    } DisplayName;            ///< A user-supplied name for the device

    struct  
    {
        bool Set;                                           ///< When set to <b>true</b> the API will set the device's callback function to <i>pCallback</i> and its context to <i>pContext</i>.
        lsapi_device_EventCallback pCallback;               ///< Callback function for receiving device event notifications. \n Ignored unless <i>Set</i> is <b>true</b>.
        void* pContext;                                     ///< Optional context for pCallback. Can be any user-supplied 32 bit value. \n Ignored unless <i>Set</i> is <b>true</b>.
    } EventCallback;        ///< A user-supplied Callback function for receiving device event notifications

} lsapi_ApiCall_Device_ControlDevice_SetDeviceInfo_Params;

typedef struct lsapi_device_ScanParams    ///< Defines parameters for a requested scan. \n Member of ::lsapi_ApiCall_Device_ControlDevice_Spectrometer_StartScan_Params
{
    // No header - this is a simple support struct
    lsapi_scan_TypeT Scantype;                              ///< Type of scan to perform, i.e. DUT, Dark, or AutoExp
    double IntegrationMs;                                   ///< Integration time in milliseconds
    lsapi_device_spectrometer_FilterT Filter;               ///< Spectrometer-model-specific filter, if any
    unsigned long NumberOfScansToAverage;                   ///< 0 will be treated as 1, i.e. no multi-scan averaging
} lsapi_device_ScanParams;

typedef struct lsapi_ApiCall_Device_ControlDevice_Spectrometer_StartScan_Params ///< Please see \link dox_w32_dvc_fcns_ctrl_sm_StartScan Start a Scan\endlink for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;           ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_ScanParams Params;                         ///< In: Scan parameters
    lsapi_SpectrometerScanHandleT hScan;                    ///< Out: Handle of started scan, or 0 on fail/error
    double EstimatedDurationMs;                             ///< Out: Estimated duration before scan completes, in milliseconds; 0 if unknown.
} lsapi_ApiCall_Device_ControlDevice_Spectrometer_StartScan_Params;

typedef struct lsapi_ApiCall_Device_ControlDevice_Spectrometer_AutoExposureSaturationLimits_Params ///< Please see \link dox_w32_dvc_fcns_ctrl_sm_AutoExposureSaturationLimits AutoExposure Saturation Limits\endlink for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;           ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    struct  
    {
        bool Set;                                           ///< Set to <b>true</b> to have the API process these parameters
        lsapi_device_AutoExposureSaturationLimits Strict;   ///< New values for strict range: the close-tolerance values
        lsapi_device_AutoExposureSaturationLimits Loose;    ///< New values for loose range: less restrictive tolerance values
    } Set; ///< In, if the 'Set' is <b>true</b>
    struct  
    {
        lsapi_device_AutoExposureSaturationLimits Strict;   ///< Current strict range
        lsapi_device_AutoExposureSaturationLimits Loose;    ///< Current loose range
    } Get; ///< Out, always populated
} lsapi_ApiCall_Device_ControlDevice_Spectrometer_AutoExposureSaturationLimits_Params;           

typedef struct lsapi_ApiCall_Device_ControlDevice_Spectrometer_TEC_Params  ///< Please see \link dox_w32_dvc_fcns_ctrl_sm_TEC Spectrometer TEC\endlink for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    struct
    {
        bool Set;                                               ///< In: pass <b>true</b> for API to process <i>Value</i>
        double Value;                                           ///< In: New setpoint value
    } Setpoint;        ///< The device's setpoint temperature
    double CurrentTemp;                                         ///< Out: Current measured TEC temperature. Always returned.
} lsapi_ApiCall_Device_ControlDevice_Spectrometer_TEC_Params;

typedef struct lsapi_ApiCall_Device_ControlDevice_Spectrometer_BaselineOffset_Params  ///< Please see \link dox_w32_dvc_fcns_ctrl_sm_BaselineOffset Spectrometer Baseline Offset\endlink for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    bool Clear;                                                 ///< In: Setting this to <b>true</b> will clear any available data
    unsigned long Timestamp;                                    ///< Out: Date/time the baseline was created, expressed as a number of seconds since Jan 1, 1970 (time_t)
    size_t DurationMs;                                          ///< Out: Duration for performing a baseline offset scan, in milliseconds
} lsapi_ApiCall_Device_ControlDevice_Spectrometer_BaselineOffset_Params;

typedef struct lsapi_ApiCall_Device_ControlDevice_Spectrometer_ShutterMode_Params///< Please see \link dox_w32_dvc_fcns_ctrl_sm_ShutterMode Spectrometer Shutter Mode\endlink for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_spectrometer_ShutterModeT RequestedMode;       ///< In: The requested shutter mode
    lsapi_device_spectrometer_ShutterModeT CurrentMode;         ///< Out: The shutter mode after 'RequestedMode' has been applied
} lsapi_ApiCall_Device_ControlDevice_Spectrometer_ShutterMode_Params;

//*************************************************************************
// ** POWER SUPPLY **

typedef struct lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params ///< Please see \ref dox_w32_dvc_ControlPwrSupp for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;   ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_powersupply_ItemT Item;            ///< The item to get or set; see the ::lsapi_device_powersupply_ItemT enum for options.
    bool Set;                                       ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                   ///< Get: Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_PowerSupply_Item_Params;

typedef struct lsapi_device_powersupply_Item_SoakTriggerMeasure_Value  ///< Struct to be passed as the "Value" param to ::lsapi_device_powersupply_Item_SoakTriggerMeasure
{
    lsapi_StructHdr hdr;                ///< hdr.StructSize must == sizeof(lsapi_device_powersupply_Item_SoakTriggerMeasure_Value)
    struct
    {
        double Volts;                   ///< In: Soak Voltage (V)
        double Amps;                    ///< In: Soak Current (A)
        double Ms;                      ///< In: The number of milliseconds to soak before triggering.
    } Soak;         ///< Soak values
    struct
    {
        double Volts;                   ///< Out: Measured voltage (V)
        double Amps;                    ///< Out: Measured current (A)
    } Measured;     ///< Measured values, taken after trigger
} lsapi_device_powersupply_Item_SoakTriggerMeasure_Value;

typedef struct lsapi_device_powersupply_Item_StartPulsing_Value     ///< Struct to be passed as the "Value" param to ::lsapi_device_powersupply_Item_StartPulsing
{
    lsapi_StructHdr hdr;                ///< hdr.StructSize must == sizeof(lsapi_device_powersupply_Item_StartPulsing_Value)
    bool SourceVolts;                   ///< true to source voltage, false to source current
    double BiasLevel;                   ///< the output level between pulses. (typically 0)  Value is in Volts if SourceVolts is true, Amps if false.
    double PulseLevel;                  ///< the output level of each pulse.  Value is in Volts if SourceVolts is true, Amps if false.
    double PulseWidthMs;                ///< the amount of time, in milliseconds, to output PulseLevel for each pulse
    double DutyCycle;                   ///< the ratio of on-time (when output is at PulseLevel) over off-time (when output is at BiasLevel)
    double BiasCompliance;              ///< the compliance level between pulses.  Value is in Amps if SourceVolts is true, Volts if false.
    double PulseCompliance;             ///< the compliance level of each pulse.  Value is in Amps if SourceVolts is true, Volts if false.
    int LoadComp;                       ///< the load/cable compensation factor to use, if supported by the supply
    int RiseTimeComp;                   ///< the rise time compensation factor to use, if supported by the supply
} lsapi_device_powersupply_Item_StartPulsing_Value;

typedef struct lsapi_device_powersupply_Item_PulseTriggerMeasure_Value  ///< Struct to be passed as the "Value" param to ::lsapi_device_powersupply_Item_PulseTriggerMeasure
{
    lsapi_StructHdr hdr;                ///< hdr.StructSize must == sizeof(lsapi_device_powersupply_Item_PulseTriggerMeasure_Value)
    struct
    {
        bool SourceVolts;               ///< In: true to source voltage, false to source current
        double BiasLevel;               ///< In: the output level between pulses. (typically 0)  Value is in Volts if SourceVolts is true, Amps if false.
        double PulseLevel;              ///< In: the output level of each pulse.  Value is in Volts if SourceVolts is true, Amps if false.
        double PulseWidthMs;            ///< In: the amount of time, in milliseconds, to output PulseLevel for each pulse
        double DutyCycle;               ///< In: the ratio of on-time (when output is at PulseLevel) over off-time (when output is at BiasLevel)
        double BiasCompliance;          ///< In: the compliance level between pulses.  Value is in Amps if SourceVolts is true, Volts if false.
        double PulseCompliance;         ///< In: the compliance level of each pulse.  Value is in Amps if SourceVolts is true, Volts if false.
        int LoadComp;                   ///< In: the load/cable compensation factor to use, if supported by the supply
        int RiseTimeComp;               ///< In: the rise time compensation factor to use, if supported by the supply
        double Ms;                      ///< In: The number of milliseconds to soak before triggering.
    } Soak;         ///< Soak values
    struct
    {
        double Volts;                   ///< Out: Measured voltage (V)
        double Amps;                    ///< Out: Measured current (A)
    } Measured;     ///< Measured values, taken after trigger
} lsapi_device_powersupply_Item_PulseTriggerMeasure_Value;


//*************************************************************************
// ** TEC **

typedef struct lsapi_ApiCall_Device_ControlDevice_TempController_Item_Params ///< Please see \ref dox_w32_dvc_ControlTempController for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_tempcontroller_ItemT Item;                     ///< The item to get or set; see the ::lsapi_device_tempcontroller_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_TempController_Item_Params;

//*************************************************************************
// ** RelayController **

typedef struct lsapi_ApiCall_Device_ControlDevice_RelayController_Item_Params ///< Please see \ref dox_w32_dvc_ControlRelayController for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_relaycontroller_ItemT Item;                    ///< The item to get or set; see the ::lsapi_device_relaycontroller_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_RelayController_Item_Params;


//*************************************************************************
// ** PowerMeter **

typedef struct lsapi_ApiCall_Device_ControlDevice_PowerMeter_Item_Params ///< Please see \ref dox_w32_dvc_ControlPowerMeter for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_powermeter_ItemT Item;                         ///< The item to get; see the ::lsapi_device_powermeter_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_PowerMeter_Item_Params;

//*************************************************************************
// ** Multimeter **

typedef struct lsapi_ApiCall_Device_ControlDevice_MultiMeter_Item_Params ///< Please see \ref dox_w32_dvc_ControlMultiMeter for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_multimeter_ItemT Item;                         ///< The item to get; see the ::lsapi_device_multimeter_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_MultiMeter_Item_Params;

//*************************************************************************
// ** Temperature Monitor **

typedef struct lsapi_ApiCall_Device_ControlDevice_TempMonitor_Item_Params ///< Please see \ref dox_w32_dvc_TemperatureMonitor for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_tempmonitor_ItemT Item;                        ///< The item to get; see the ::lsapi_device_tempmonitor_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< In/Out: The item-value to set or get.
    size_t ChannelNumber;                                       ///< Channel # to be controlled.
} lsapi_ApiCall_Device_ControlDevice_TempMonitor_Item_Params;


//*************************************************************************
// ** Photometry Module **

typedef struct lsapi_ApiCall_Device_ControlDevice_PhotometryModule_Item_Params ///< Please see \ref dox_w32_dvc_PhotometryModule for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_photometrymodule_ItemT Item;                   ///< The item to get; see the ::lsapi_device_photometrymodule_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_PhotometryModule_Item_Params;


//*************************************************************************
// ** LM-82 **

typedef struct lsapi_ApiCall_Device_ControlDevice_LM82Controller_Item_Params ///< Please see \ref dox_w32_dvc_LM82 for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_lm82controller_ItemT Item;                     ///< The item to get or set; see the ::lsapi_device_lm82controller_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_LM82Controller_Item_Params;

typedef struct lsapi_device_lm82Controller_Item_MeasureTemperature_Value  ///< Struct to be passed as the "Value" param to ::lsapi_device_LM82_Item_MeasureTemperature
{
    lsapi_StructHdr hdr;                                        ///< hdr.StructSize must == sizeof(lsapi_device_lm82Controller_Item_MeasureTemperature_Value)
    lsapi_device_lm82controller_SensorT Sensor;                 ///< In: The sensor to measure
    double Temperature;                                         ///< Out: The sensor's measured temperature
} lsapi_device_lm82Controller_Item_MeasureTemperature_Value;


//*************************************************************************
// ** Detector **

typedef struct lsapi_ApiCall_Device_ControlDevice_Detector_Item_Params ///< Please see \ref dox_w32_dvc_Detector for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_detector_ItemT Item;                           ///< The item to get; see the ::lsapi_device_detector_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value. 
} lsapi_ApiCall_Device_ControlDevice_Detector_Item_Params;

//*************************************************************************
// ** VariableAttenuator **

typedef struct lsapi_ApiCall_Device_ControlDevice_VA_Item_Params ///< Please see \ref dox_w32_dvc_VA for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_VA_ItemT Item;                                 ///< The item to get or set; see the ::lsapi_device_VA_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< In/Out: The value to set, or the item's current value.
    bool async;                                                 ///< In: set true to return immediately without waiting for move/reset to finish.  State will be lsapi_device_DeviceState_Busy until operation is complete.
} lsapi_ApiCall_Device_ControlDevice_VA_Item_Params;


//*************************************************************************
// ** PlasmaLamp **

typedef enum                                                    ///< Value for possible commands to a plasma lamp.  Used in ::lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Cmd_Params
{
    lsapi_device_PlasmaLamp_Cmd_Unknown = 0,                    ///< Illegal/uninitialized value
    lsapi_device_PlasmaLamp_Cmd_Energize,                       ///< Energize maps to "Start"
    lsapi_device_PlasmaLamp_Cmd_Stabilize,                      ///< Stabilize maps to "Unlock Optimize"
    lsapi_device_PlasmaLamp_Cmd_Optimize,                       ///< Optimize maps to "Lock Optimize"
    lsapi_device_PlasmaLamp_Cmd_DeEnergize,                     ///< DeEnergize maps to "Stop"
} lsapi_device_PlasmaLamp_CmdT;                                 ///< Command enum, used by ::lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Cmd_Params

typedef struct lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Cmd_Params ///< Please see PlasmaLamp \ref dox_w32_dvc_plasma_command for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_PlasmaLamp_CmdT Command;                       ///< In: Command enum; see the ::lsapi_device_PlasmaLamp_CmdT enum for command list.
} lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Cmd_Params;

typedef enum                                                    ///< Value for possible operating states of a plasma lamp.  Returned by ::lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Status_Params
{
    lsapi_device_PlasmaLamp_State_Erred = 0,                    ///< Plasma lamp experienced an error, such as comm failure or programming error
    lsapi_device_PlasmaLamp_State_Idle,                         ///< Plasma lamp is Idle, i.e. de-energized
    lsapi_device_PlasmaLamp_State_Energizing,                   ///< Plasma lamp is Energizing, i.e. warming up
    lsapi_device_PlasmaLamp_State_Optimizing,                   ///< Plasma lamp is Optimizing: energized with unstable output
    lsapi_device_PlasmaLamp_State_Stable,                       ///< Plasma lamp is Stable: energized with stable output
    lsapi_device_PlasmaLamp_State_DeEnergizing,                 ///< Plasma lamp is DeEnergizing, i.e. safely stopping output
} lsapi_device_PlasmaLamp_StateT;                               ///< State enum, used by ::lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Status_Params

typedef struct lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Status_Params ///< Please see PlasmaLamp \ref dox_w32_dvc_plasma_status for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;             ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_PlasmaLamp_StateT State;                     ///< Out: State enum; see enum ::lsapi_device_PlasmaLamp_StateT for list of states.
} lsapi_ApiCall_Device_ControlDevice_PlasmaLamp_Status_Params;

typedef struct lsapi_PlasmaLamp_TransitionData       ///< Event-data sent by a PlasmaLamp. \n ::lsapi_device_EventCallback's pHdr param points to the lsapi_PlasmaLamp_TransitionData struct's <b>hdr</b> member.
{
    lsapi_device_EventHeader hdr;               ///< Verify that hdr.StructSize equals sizeof(lsapi_PlasmaLamp_TransitionData) before casting the ::lsapi_device_EventCallback's pHdr param as a lsapi_PlasmaLamp_TransitionData*
    lsapi_device_PlasmaLamp_StateT NewState;    ///< The PlasmaLamp's new state (after transition).
    lsapi_device_PlasmaLamp_StateT OldState;    ///< The PlasmaLamp's old state (before transition).
} lsapi_PlasmaLamp_TransitionData;

//*************************************************************************
// ** FilterWheel **

typedef struct lsapi_ApiCall_Device_ControlDevice_FW_Item_Params ///< Please see \ref dox_w32_dvc_fw for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_device_FW_ItemT Item;                                 ///< The item to get or set; see the ::lsapi_device_FW_ItemT enum for options.
    bool Set;                                                   ///< <b>true</b> to set the item to <i>Value</i>; <b>false</b> to retrieve the current item into <i>Value</i>
    double Value;                                               ///< Out: The item's current value.
    bool async;                                                 ///< In: set true to return immediately without waiting for move/reset to finish.  State will be lsapi_device_DeviceState_Busy until operation is complete.
} lsapi_ApiCall_Device_ControlDevice_FW_Item_Params;


//*************************************************************************
// ** Lamp **

typedef struct lsapi_ApiCall_Device_ControlDevice_Lamp_Status_Params ///< Please see Lamp \ref dox_w32_dvc_lamp_status for usage
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    bool State;                                                 ///< Out: true = on, false = off
} lsapi_ApiCall_Device_ControlDevice_Lamp_Status_Params;

//*************************************************************************
// ** LED_Driver **

typedef struct lsapi_LED_Driver_ChannelSetting                  ///< A channel and setting pair used in lsapi_device_Control_LED_Driver_SetChannels
{
    int channelNumber;                                          ///< The channel to be set.
    double ampSetting;                                          ///< The current setting for the channel, in A.
} lsapi_LED_Driver_ChannelSetting;

typedef struct lsapi_ApiCall_Device_ControlDevice_LED_Driver_Channels_Params    ///< struct used to set/read the current settings of the LED Driver's channels
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_LED_Driver_ChannelSetting *channels;                  ///< In/out: An array of the channels to set/read and their respective current in Amps.
    int numChannels;                                            ///< In: The number of elements in the channels array
} lsapi_ApiCall_Device_ControlDevice_LED_Driver_Channels_Params;

typedef struct lsapi_ApiCall_Device_ControlDevice_LED_Driver_EnableMode_Params               ///< struct used to enable/disable cal lamp mode or trigger mode on an LED Driver Board
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    bool enable;                                                ///< In: true to enable, false to disable.
} lsapi_ApiCall_Device_ControlDevice_LED_Driver_EnableMode_Params;

//*************************************************************************
// ** TransientDetector **

typedef struct lsapi_device_Control_TransientDetector_StartScan_Params ///< Used with lsapi_device_Control_TransientDetector_StartScan
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    bool IsScanForPstLM;                                        ///< In: Request scan for flicker metric PstLM (NEMA 77-2017), which requires longer duration (60-180 sec) than other metrics.
    double SampleRateHz;                                        ///< In: Sample Rate, Hz
    unsigned int NumSamples;                                    ///< In: Requested number of samples for time-series. NOTE: Duration (sec) = NumSamples / SampleRateHz
    lsapi_scan_TriggerModeT TriggerMode;                        ///< In: Trigger mode
    double DetectorLevel;                                       ///< In: Expected raw output level of detector, used to set measurement scale.  Pass 0 to use the current scale.
    lsapi_ScalarTransientHandleT hScalarTransient;              ///< Out: Handle of new ScalarTransient scan. Note: Until it reaches completion, ScalarTransient is owned by the TransientDetector.
} lsapi_device_Control_TransientDetector_StartScan_Params;

typedef struct lsapi_device_Control_TransientDetector_Arm_Params ///< Used with lsapi_device_Control_TransientDetector_Arm
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    bool IsScanForPstLM;                                        ///< In: Request scan for flicker metric PstLM (NEMA 77-2017), which requires longer duration (60-180 sec) than other metrics.
    double SampleRateHz;                                        ///< In: Sample Rate, Hz
    unsigned int NumSamples;                                    ///< In: Requested number of samples for time-series. NOTE: Duration (sec) = NumSamples / SampleRateHz
    lsapi_scan_TriggerModeT TriggerMode;                        ///< In: Trigger mode
    double DetectorLevel;                                       ///< In: Expected raw output level of detector, used to set measurement scale.  Pass 0 to use the current scale.
} lsapi_device_Control_TransientDetector_Arm_Params;

typedef struct lsapi_device_Control_TransientDetector_Fire_Params ///< Used with lsapi_device_Control_TransientDetector_Fire
{
    lsapi_ApiCall_Device_ControlDeviceHdr CtlHdr;               ///< See \ref dox_w32_dvc_ControlDeviceHdr_prep for details on preparation of this member.
    lsapi_ScalarTransientHandleT hScalarTransient;              ///< Out: Handle of new ScalarTransient scan. Note: Until it reaches completion, ScalarTransient is owned by the TransientDetector.
} lsapi_device_Control_TransientDetector_Fire_Params;

typedef struct lsapi_transientdetector_ScalarTransientEventData    ///< Event-data sent by a TransientDetector device for its associated ScalarTransient scan. \n ::lsapi_device_EventCallback's pHdr param points to the lsapi_transientdetector_ScalarTransientEventData struct's <b>hdr</b> member.
{                                                               // EventId = lsapi_device_EventId_ScalarTransientScan_Transition
    lsapi_device_EventHeader hdr;                               ///< Verify that hdr.StructSize equals sizeof(lsapi_transientdetector_ScalarTransientEventData) before casting the ::lsapi_device_EventCallback's pHdr param as a lsapi_transientdetector_ScalarTransientEventData*
    lsapi_ScalarTransientHandleT hScalarTransient;              ///< Handle of the ScalarTransient scan, associated with the TransientDetector device, which had state transition
    // ESB 7/17/2018 PROPOSAL/WIP: Consolidate ScalarTransient scan transition Events (a la PlasmaLamp_Transition), to avoid proliferation of lsapi_device_EventIdT enums
    lsapi_scan_StateT NewScanState;                             ///< New state of ScalarTransient scan (after transition).
    lsapi_scan_StateT OldScanState;                             ///< Old state of ScalarTransient scan (before transition).
} lsapi_transientdetector_ScalarTransientEventData;


//-------------------------------
// Structs based on lsapi_ApiCall_Device_UtilitiesHdr

typedef struct lsapi_StrayLightFactorData                       ///< Helper struct for getting & setting stray light data
{
    lsapi_device_DeviceInfo DvcInfo;                            ///< The type of device & serial number for which to set the SL factor
    double Factor;                                              ///< The Stray Light factor for the device
    unsigned long Timestamp;                                    ///< The date/time the factor was added to the API, expressed as a number of seconds since Jan 1, 1970 (time_t)
} lsapi_StrayLightFactorData;

typedef struct lsapi_ApiCall_Device_Utilities_InstallCDS30x0WaveCal_Params ///< Please see the \link dox_w32_dvc_fcns_installcds30x0wavecalfile CDS30x0 Wavelength Calibration File Utility\endlink for usage
{
    lsapi_ApiCall_Device_UtilitiesHdr UtilsHdr;                 ///< See UtilsHdr Preparation above for details on preparation of this member.
    wchar_t WavecalFilepath[lsapi_TEXTBUF_SIZE];                ///< In: The path to the to-be-installed file
    lsapi_device_DeviceInfo DvcInfo;                            ///< Out: The type of device & serial # that the API found in the file
} lsapi_ApiCall_Device_Utilities_InstallCDS30x0WaveCal_Params;

typedef struct lsapi_ApiCall_Device_Utilities_InstallVirtualDevice_Params ///< Please see the \link dox_w32_dvc_fcns_installvirtualdevice Virtual Device Utility\endlink for usage
{
    lsapi_ApiCall_Device_UtilitiesHdr UtilsHdr;                 ///< See UtilsHdr Preparation above for details on preparation of this member.
    lsapi_device_VirtualDeviceInfo Info;                        ///< In: Attributes for the virtual device.
} lsapi_ApiCall_Device_Utilities_InstallVirtualDevice_Params;

typedef struct lsapi_ApiCall_Device_Utilities_GetBaselineOffsetInfo_Params ///< Please see the \link dox_w32_dvc_fcns_getboc Utility\endlink for usage
{
    lsapi_ApiCall_Device_UtilitiesHdr UtilsHdr;                 ///< See UtilsHdr Preparation above for details on preparation of this member.
    lsapi_device_DeviceInfo DvcInfo;                            ///< In: The type of device & serial number for which to retrieve BOC info
    bool Supported;                                             ///< Out: Indicates if the given device supports BOC (true for non-shuttered spectrometer, false for anything else)
    unsigned long Timestamp;                                    ///< Out: Date/time the baseline was created, expressed as a number of seconds since Jan 1, 1970 (time_t)
    size_t DurationMs;                                          ///< Out: Duration for performing a baseline offset scan, in milliseconds
} lsapi_ApiCall_Device_Utilities_GetBaselineOffsetInfo_Params;

typedef struct lsapi_ApiCall_Device_Utilities_SetStrayLightFactor_Params ///< Please see the \link dox_w32_dvc_fcns_setslfactor Utility\endlink for usage
{
    lsapi_ApiCall_Device_UtilitiesHdr UtilsHdr;                 ///< See UtilsHdr Preparation above for details on preparation of this member.
    lsapi_StrayLightFactorData data;                            ///< The stray light factor, and the device which the factor pertains to. Note that data.timestamp will be set by the API; any passed-in value is ignored.
} lsapi_ApiCall_Device_Utilities_SetStrayLightFactor_Params;

typedef struct lsapi_ApiCall_Device_Utilities_GetStrayLightFactors_Params ///< Please see the \link dox_w32_dvc_fcns_getslfactors Utility\endlink for usage
{
    lsapi_ApiCall_Device_UtilitiesHdr UtilsHdr;                 ///< See UtilsHdr Preparation above for details on preparation of this member.
    lsapi_StrayLightFactorData* pData;                          ///< Array of structures data. Must be pre-allocated by caller.
    size_t ElementCount;                                        ///< # of lsapi_StrayLightFactorData structs in 'pData'.
} lsapi_ApiCall_Device_Utilities_GetStrayLightFactors_Params;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scan functionality, part 2

/*! Defines the lsapi_ApiCall_Scan_ControlHdr.CtlFcn values for ::lsapi_ApiCall_Scan_Control */
typedef enum
{
    lsapi_scan_CtlFcn_Unknown                  =  0,  ///< This value is a placeholder and is not used.
    lsapi_scan_CtlFcn_Abort                    =  1,  ///< Abort an in-progress scan. \n Used with ::lsapi_ApiCall_Scan_ControlHdr
    lsapi_scan_CtlFcn_GetInfo                  =  2,  ///< Retrieve scan state and characteristics of the scan. \n Used with ::lsapi_ApiCall_Scan_GetInfo_Params
    lsapi_scan_CtlFcn_GetData                  =  3,  ///< Retrieve raw spectrum data from a DUT, Dark, or Processed Scan. Used with ::lsapi_ApiCall_Scan_GetData_Params
    lsapi_scan_CtlFcn_GetAutoExposureInfo      =  4,  ///< Retrieve results of an Autoexposure scan. \n Used with ::lsapi_ApiCall_Scan_GetAutoExposureInfo_Params
    lsapi_scan_CtlFcn_Process                  =  5,  ///< Apply selected corrections (dark, non-linearity, etc) to existing scans to create a new ProcessedScan. \n Used with ::lsapi_ApiCall_Scan_Process_Params
    lsapi_scan_CtlFcn_GetUncalibratedSpectrum  =  6,  ///< Retrieve an uncalibrated spectrum from a Processed Scan. Used with ::lsapi_ApiCall_Scan_GetUncalibratedSpectrum_Params
    lsapi_scan_CtlFcn_Reconstitute             =  7,  ///< Create a scan handle from raw data. Used with ::lsapi_ApiCall_Scan_Reconstitute_Params
    lsapi_scan_CtlFcn_Interpolate              =  8,  ///< Interpolate an existing Scan object to a new one with different wavelengths, e.g. at regular 1nm intervals. Used with ::lsapi_ApiCall_Scan_Interpolate_Params
    lsapi_scan_CtlFcn_ProcessEx                =  9,  ///< Variant of lsapi_scan_CtlFcn_Process to control details of corrections, for API developers. \n Used with ::lsapi_ApiCall_Scan_ProcessEx_Params
    lsapi_scan_CtlFcn_SaveToFile               = 10,  ///< Save scan to a text file in spdx format per TM-27-20. \n Used with ::lsapi_ApiCall_Scan_SaveToFile_Params
    lsapi_scan_CtlFcn_LoadFromFile             = 11,  ///< Load scan from a text file in spdx format per TM-27-20. \n Used with ::lsapi_ApiCall_Scan_LoadFromFile_Params
} lsapi_scan_CtlFcnT;

/*! Indicates the result of an Autoexposure. \n Retrieved via ::lsapi_scan_CtlFcn_GetAutoExposureInfo */
typedef enum
{
    lsapi_scan_AutoexposureResult_Unknown                   = 0,     ///< Unknown value for indicating uninitialized var
    lsapi_scan_AutoexposureResult_SignalTooLow              = 51,    ///< Signal is too weak to put saturation in the sweet spot at maximum integration time.
    lsapi_scan_AutoexposureResult_SignalTooHigh             = 52,    ///< Signal is too strong to put saturation in the sweet spot at minimum integration time.
    lsapi_scan_AutoexposureResult_Success_Strict            = 53,    ///< Saturation was successfully obtained within in the "strict" range
    lsapi_scan_AutoexposureResult_Success_Loose             = 54,    ///< Saturation was successfully obtained within in the "loose" range
    lsapi_scan_AutoexposureResult_Success_PrecisionLimited  = 55,    ///< The attained saturation is limited by the precision of the spectrometer's integration time.
} lsapi_scan_AutoexposureResultT;


// ScanData processing options
typedef unsigned long lsapi_scan_ProcessingOptionsT;        ///< A bitmask of processing options to be performed on the raw Scan data obtained from a Spectrometer
#define lsapi_scan_ProcessingOption_None                                ((lsapi_scan_ProcessingOptionsT)0)                 ///< Request "no processing" in a call to ::lsapi_scan_CtlFcn_Process
#define lsapi_scan_ProcessingOption_StrayLightCorrection                ((lsapi_scan_ProcessingOptionsT)0x00000001)        ///< Request that Stray Light Correction be performed in a call to ::lsapi_scan_CtlFcn_Process
#define lsapi_scan_ProcessingOption_NonLinearityCorrection              ((lsapi_scan_ProcessingOptionsT)0x00000002)        ///< Request that Non Linearity Correction be performed in a call to ::lsapi_scan_CtlFcn_Process
#define lsapi_scan_ProcessingOption_Interpolation                       ((lsapi_scan_ProcessingOptionsT)0x00000008)        ///< Request that Interpolation be performed in a call to ::lsapi_scan_CtlFcn_Process
#define lsapi_scan_ProcessingOption_ConstrainWavelengthsToDeviceLimits  ((lsapi_scan_ProcessingOptionsT)0x00000020)        ///< Request that Wavelengths be constrained to within published Device limits in a call to ::lsapi_scan_CtlFcn_Process

/*! Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Scan_Control. \n
Most ::lsapi_scan_CtlFcnT commands have their own custom structs based on this struct. \n
See the individual lsapi_ApiCall_Scan_Control_* structs for details. */
typedef struct lsapi_ApiCall_Scan_ControlHdr
{
    lsapi_ApiCallHdr ApiCallHdr;                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_scan_CtlFcnT CtlFcn;                  ///< In: Control function to execute
    lsapi_HandleT hScan;                        ///< In: Handle of scan to operate upon. May be either a ::lsapi_SpectrometerScanHandleT or an ::lsapi_ProcessedScanHandleT
} lsapi_ApiCall_Scan_ControlHdr;

typedef struct lsapi_ApiCall_Scan_GetInfo_Params    ///< Please see \link dox_w32_scan_fcns_GetScanInfo GetScanInfo\endlink for usage
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;                   ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    lsapi_scan_StateT ScanState;                            ///< Out: Current state of hScan
    bool ScanIsIncomplete;                                  ///< Out: <b>true</b> if the scan has not finished; <b>false</b> if ScanState is one of lsapi_scan_State_Succeeded | lsapi_scan_State_Aborted | lsapi_scan_State_Error
    size_t ElementCount;                                    ///< Out: # of elements
    double IntegrationMs;                                   ///< Out: Integration time used to obtain scandata
    lsapi_device_spectrometer_FilterT Filter;               ///< Out: Filter value
    lsapi_DeviceHandleT hSpectrometer;                      ///< Out: Handle of the spectrometer taking the scan
} lsapi_ApiCall_Scan_GetInfo_Params;

typedef struct lsapi_ApiCall_Scan_GetAutoExposureInfo_Params ///< Please see \link dox_w32_scan_fcns_GetAutoExposureInfo GetAutoExposureInfo\endlink for usage
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;                       ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    double Saturation;                                          ///< Out: 0.0 <= x <= 1.0 (1.0 being fully saturated)
    double IntegrationMs;                                       ///< Out: Integration time used to obtain above saturation
    lsapi_device_spectrometer_FilterT Filter;                   ///< Out: Filter value (meaning of value varies by spectrometer)
    lsapi_scan_AutoexposureResultT AexResult;                   ///< Out: The result of scan
} lsapi_ApiCall_Scan_GetAutoExposureInfo_Params;

typedef struct lsapi_ApiCall_Scan_Process_Params     ///< Please see \link dox_w32_scan_fcns_Process Process\endlink for usage
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    struct
    {
        lsapi_scan_ProcessingOptionsT Options;          ///< In: Mask of lsapi_scan_ProcessingOption_* flags
        lsapi_SpectrometerScanHandleT hDarkScan;        ///< In: The (optional) dark scan to use for dark correction
    } Requested;        ///< Requested processing options. You fill these in before calling the API.
    struct
    {
        lsapi_scan_ProcessingOptionsT Options;          ///< Out: The options that were actually applied. May be different from Requested.Options.
        double StrayLightFactor;                        ///< Out: The Stray Light Factor that was applied; 0 if API did not apply external Stray Light Correction.
    } Applied;          ///< Applied processing options. These will usually match Requested.Options, but if you have critical options that <i>must</i> be applied, confirm them in Applied.Options before proceeding.

    lsapi_ProcessedScanHandleT hProcessedScan;          ///< Out: The processed scan
    int ProcessedScanStartOffset;                       ///< Out: Offset of the output ProcessedScan elements w.r.t. the input SpecScan(s) elements; not applicable when lsapi_scan_ProcessingOption_Interpolation is applied.
} lsapi_ApiCall_Scan_Process_Params;

typedef struct lsapi_ApiCall_Scan_GetData_Params        ///< Please see \link dox_w32_scan_fcns_GetScanData GetScanData\endlink for usage
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    lsapi_ScanData ScanData;                            ///< Out: Data for the scan (though ScanData.pData and ScanData.ElementCount must be init'ed by the caller)
    double Saturation;                                  ///< Out: 0.0 <= x <= 1.0 (1.0 being fully saturated)
} lsapi_ApiCall_Scan_GetData_Params;

typedef struct lsapi_ApiCall_Scan_GetUncalibratedSpectrum_Params     ///< Please see \link dox_w32_scan_fcns_GetUncalibratedSpectrum GetUncalibratedSpectrum\endlink for usage
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    lsapi_SpectrumHandleT hUncalibratedSpectrum;        ///< Out: Handle to the uncalibrated spectrum
} lsapi_ApiCall_Scan_GetUncalibratedSpectrum_Params;

typedef struct lsapi_ApiCall_Scan_Reconstitute_Params   ///< Please see \link dox_w32_scan_fcns_Reconstitute Reconstitute\endlink for usage
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    lsapi_ScanData ScanData;                            ///< In: Data for the scan
    lsapi_ReconstitutedScanHandleT hReconstitutedScan;  ///< Out: The reconstituted scan handle.
} lsapi_ApiCall_Scan_Reconstitute_Params;

typedef struct lsapi_ApiCall_Scan_Interpolate_Params    ///< Please see \link dox_w32_scan_fcns_Interpolate Interpolate\endlink for usage
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    lsapi_HandleT hReferenceScan;                       ///< In:  Scan object with wavelength set for interpolated Scan, or 0 with non-zero RegularInterval_nm
    unsigned long RegularInterval_nm;                   ///< In:  Regular Interval for wavelength set of interpolated Scan, or 0 with a valid hReferenceScan
    lsapi_ProcessedScanHandleT hInterpolatedScan;       ///< Out: Handle to the new ProcessedScan object, with data interpolated to specified wavelength set
} lsapi_ApiCall_Scan_Interpolate_Params;

typedef struct lsapi_ApiCall_Scan_ProcessEx_Params      ///< Used by lsapi_scan_CtlFcn_Process to control details of corrections, for API developers.
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    struct
    {
        lsapi_scan_ProcessingOptionsT Options;          ///< In: Mask of lsapi_scan_ProcessExingOption_* flags
        double StrayLightFactor;                        ///< In: [NEW] The Stray Light Factor requested.
        lsapi_SpectrometerScanHandleT hDarkScan;        ///< In: The (optional) dark scan to use for dark correction
    } Requested;        ///< Requested processing options. You fill these in before calling the API.
    struct
    {
        lsapi_scan_ProcessingOptionsT Options;          ///< Out: The options that were actually applied. May be different from Requested.Options.
        double StrayLightFactor;                        ///< Out: The Stray Light Factor that was applied; 0 if API did not apply external Stray Light Correction.
    } Applied;          ///< Applied processing options. These will usually match Requested.Options, but if you have critical options that <i>must</i> be applied, confirm them in Applied.Options before proceeding.

    lsapi_ProcessedScanHandleT hProcessedScan;          ///< Out: The processed scan
    int ProcessedScanStartOffset;                       ///< Out: Offset of the output ProcessedScan elements w.r.t. the input SpecScan(s) elements; not applicable when lsapi_scan_ProcessingOption_Interpolation is applied.
} lsapi_ApiCall_Scan_ProcessEx_Params;

typedef struct lsapi_ApiCall_Scan_SaveToFile_Params      ///< Set CtlHdr.CtlFcn = ::lsapi_scan_CtlFcn_SaveToFile
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    wchar_t FilePath[lsapi_TEXTBUF_SIZE];               ///< In: Fully-qualified path for the file to save
    wchar_t Description[lsapi_TEXTBUF_SIZE];            ///< In: Client-provided Description for scan
    lsapi_DeviceHandleT hSpectrometer;                  ///< In: If non-zero, output for ProcessedScan will include Spectrometer info. (API internal SpecScan object references its Spectrometer, but ProcessedScan object does not.)
    bool Overwrite;                                     ///< In: Pass true to overwrite any existing file. If false and file exists, save will fail.
} lsapi_ApiCall_Scan_SaveToFile_Params;

typedef struct lsapi_ApiCall_Scan_LoadFromFile_Params   ///< Set CtlHdr.CtlFcn = ::lsapi_scan_CtlFcn_LoadFromFile
{
    lsapi_ApiCall_Scan_ControlHdr CtlHdr;               ///< In: extension of lsapi_ApiCallHdr that includes a scan function and handle
    wchar_t FilePath[lsapi_TEXTBUF_SIZE];               ///< In: Fully-qualified path to the file to load
    wchar_t Description[lsapi_TEXTBUF_SIZE];            ///< Out: Client-provided Description for scan
    lsapi_HandleT hScanLoaded;                          ///< Out: The handle of the newly loaded scan object, either ProcessedScan or ReconstitutedScan
} lsapi_ApiCall_Scan_LoadFromFile_Params;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScalarTransient [scan] functionality

/*! Defines the lsapi_ApiCall_ScalarTransient_ControlHdr.CtlFcn values for ::lsapi_ApiCall_ScalarTransient_Control */
typedef enum
{
    lsapi_ScalarTransient_CtlFcn_GetStatus                =  0,  ///< Get current status of ScalarTransient scan. \n Used with ::lsapi_ApiCall_ScalarTransient_GetStatus_Params
    lsapi_ScalarTransient_CtlFcn_Abort                    =  1,  ///< Abort an in-progress scan. \n Used with ::lsapi_ApiCall_ScalarTransient_ControlHdr
    lsapi_ScalarTransient_CtlFcn_GetData                  =  2,  ///< Retrieve scalar transient time-series and related data. Used with ::lsapi_ApiCall_ScalarTransient_GetData_Params
    lsapi_ScalarTransient_CtlFcn_Reconstitute             =  3,  ///< Create a ScalarTransient object from time-series and related data. Used with ::lsapi_ApiCall_ScalarTransient_Reconstitute_Params
    lsapi_ScalarTransient_CtlFcn_GetFourierSeries         =  4,  ///< Retrieve calculated Fourier series data. Used with ::lsapi_ApiCall_ScalarTransient_GetFourierSeries_Params
    lsapi_ScalarTransient_CtlFcn_GetFlickerMetrics        =  5,  ///< Retrieve calculated Flicker Metrics. Used with ::lsapi_ApiCall_ScalarTransient_GetFlickerMetrics_Params
    lsapi_ScalarTransient_CtlFcn_GetFlickerPstLM          =  6,  ///< Retrieve calculated PstLM flicker metric (NEMA 77-2017). Used with ::lsapi_ApiCall_ScalarTransient_GetFlickerPstLM_Params
} lsapi_ScalarTransient_CtlFcnT;

/*! Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_ScalarTransient_Control. \n
Most ::lsapi_ScalarTransient_CtlFcnT commands have their own custom structs based on this struct. \n
See the individual lsapi_ApiCall_ScalarTransient_* structs for details. */
typedef struct lsapi_ApiCall_ScalarTransient_ControlHdr
{
    lsapi_ApiCallHdr ApiCallHdr;                        ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_ScalarTransient_CtlFcnT CtlFcn;               ///< In: Control function to execute
    lsapi_ScalarTransientHandleT hScalarTransient;      ///< In/Out: Handle of ScalarTransient to operate upon. Output for lsapi_ScalarTransient_CtlFcn_Reconstitute, input for other commands.
} lsapi_ApiCall_ScalarTransient_ControlHdr;

typedef struct lsapi_ApiCall_ScalarTransient_GetStatus_Params   ///< lsapi_ApiCall_ScalarTransient_Control / lsapi_ScalarTransient_CtlFcn_GetStatus
{
    lsapi_ApiCall_ScalarTransient_ControlHdr CtlHdr;    ///< See ..lsapi_ApiCall_ScalarTransient_ControlHdr Preparation above for details on preparation of this member.
    lsapi_scan_StateT ScanState;                        ///< Out: Current state of hScan
    bool ScanIsIncomplete;                              ///< Out: <b>true</b> if the scan has not finished; <b>false</b> if ScanState is one of lsapi_scan_State_Succeeded | lsapi_scan_State_Aborted | lsapi_scan_State_Error
    unsigned int NumSamples;                            ///< Out: Requested number of samples if ScanIsIncomplete, otherwise actual number of samples.
    double SampleRateHz;                                ///< Out: Requested sample rate if ScanIsIncomplete, otherwise actual sample rate.
    bool HasSamplesOverRange;                           ///< Out: If not ScanIsIncomplete, indicates whether any scan samples were over range. NOTE: If HasSamplesOverRange, client may choose to skip lsapi_ScalarTransient_CtlFcn_GetData and simply delete scan.
    lsapi_DeviceHandleT hTransientDetector;             ///< Out: Handle of the owning TransientDetector if ScanIsIncomplete, otherwise 0.
    wchar_t ErrorDetail[lsapi_TEXTBUF_SIZE];            ///< Out: String with error detail if ScanState==lsapi_scan_State_Error, otherwise empty string.
} lsapi_ApiCall_ScalarTransient_GetStatus_Params;

typedef struct lsapi_ApiCall_ScalarTransient_GetData_Params             ///< Used with with ::lsapi_ScalarTransient_CtlFcn_GetData
{
    lsapi_ApiCall_ScalarTransient_ControlHdr CtlHdr;    ///< See ..lsapi_ApiCall_ScalarTransient_ControlHdr Preparation above for details on preparation of this member.
    double* TimeSeries;                                 ///< In: Array for time-series data, allocated by caller with NumSamples(In) elements; API copies time-series into array if NumSamples(In) <= NumSamples(Out), else returns InsufficientBuffer.
    unsigned int NumSamples;                            ///< In/Out: Number of doubles allocated by caller in TimeSeries array (In); actual NumSamples in TimeSeries (Out).
    double SampleRateHz;                                ///< Out: Actual Sample Rate (Hz).
    double FirmwareSampleRateHz;                        ///< Out: Firmware Sample Rate (Hz), before sample averaging to anti-alias.
    bool IsScanForPstLM;                                ///< Out: Scan is for flicker metric PstLM (NEMA 77-2017), which requires longer duration (60-180 sec) than other metrics.
    double MinValue;                                    ///< Out: Min value of the TimeSeries data
    double MaxValue;                                    ///< Out: Max value of the TimeSeries data
    double ScaleMaxValue;                               ///< Out: Max value for the scale at which scan was taken (useful for normalizing)
    unsigned int NumSamplesOverRange;                   ///< Out: Num samples that were over range. WARNING: If a scan has NumSamplesOverRange > 0, its Fourier and Flicker calculations will be unreliable.
    lsapi_device_DeviceInfo TransientDetectorInfo;      ///< Out: DeviceType and Serial Number of the TransientDetector that created this scan.
    double Timestamp;                                   ///< Out: Time the originating scan was started, expressed as a number of seconds since Jan 1, 1970 (time_t)
} lsapi_ApiCall_ScalarTransient_GetData_Params;

typedef struct lsapi_ApiCall_ScalarTransient_Reconstitute_Params        ///< Used with with ::lsapi_ScalarTransient_CtlFcn_Reconstitute
{
    lsapi_ApiCall_ScalarTransient_ControlHdr CtlHdr;    ///< In/Out: If successful, CtlHdr.hScalarTransient returns handle to newly-created ScalarTransient object.
    const double* TimeSeries;                           ///< In: Array for time-series data supplied by caller with NumSamples(In) elements
    unsigned int NumSamples;                            ///< In: Number of elements (type double) in TimeSeries.
    double SampleRateHz;                                ///< In: Actual Sample Rate (Hz).
    double FirmwareSampleRateHz;                        ///< In: Firmware Sample Rate (Hz), before sample averaging to anti-alias, if available.
    bool IsScanForPstLM;                                ///< In: Scan is for flicker metric PstLM (NEMA 77-2017), which requires longer duration (60-180 sec) than other metrics.
    double ScaleMaxValue;                               ///< In: Max value for the scale at which scan was taken, if available.
    unsigned int NumSamplesOverRange;                   ///< In: Num samples that were over range, if available.
    lsapi_device_DeviceInfo TransientDetectorInfo;      ///< In: DeviceType and Serial Number of the TransientDetector that created this scan, if available.
    double Timestamp;                                   ///< In: Time the originating scan was started (expressed as a number of seconds since Jan 1, 1970 i.e. time_t), if available.
} lsapi_ApiCall_ScalarTransient_Reconstitute_Params;

typedef struct lsapi_ApiCall_ScalarTransient_GetFourierSeries_Params    ///< Used with with ::lsapi_ScalarTransient_CtlFcn_GetFourierSeries
{
    lsapi_ApiCall_ScalarTransient_ControlHdr CtlHdr;    ///< See ..lsapi_ApiCall_ScalarTransient_ControlHdr Preparation above for details on preparation of this member.
    lsapi_Harmonic* FourierSeries;                      ///< In/Out: Array for Fourier-series data, allocated by caller with NumHarmonics elements; API copies the largest harmonics into FourierSeries array, in order of frequency.
    unsigned int NumHarmonics;                          ///< In/Out: Number of harmonics allocated by caller in FourierSeries array (In); lesser of requested or max NumHarmonics (Out).
} lsapi_ApiCall_ScalarTransient_GetFourierSeries_Params;

typedef struct lsapi_ApiCall_ScalarTransient_GetFlickerMetrics_Params   ///< Used with with ::lsapi_ScalarTransient_CtlFcn_GetFlickerMetrics
{
    lsapi_ApiCall_ScalarTransient_ControlHdr CtlHdr;    ///< See ..lsapi_ApiCall_ScalarTransient_ControlHdr Preparation above for details on preparation of this member.
    double FundamentalFrequencyHz;                      ///< Out: Frequency (Hz) of largest harmonic from Fourier analysis
    double PercentFlicker;                              ///< Out: PercentFlicker, a.k.a. Amplitude Modulation or Modulation Depth: (Max output - Min output) / (Max output + Min output). Range: 0 to 100%. Insensitive to periodic frequency.
    double PercentFlicker_40Hz;                         ///< Out: PercentFlicker filtered above 40Hz, per California Title 24 Part 6 JA 10.6 Calculations
    double PercentFlicker_90Hz;                         ///< Out: PercentFlicker filtered above 90Hz
    double PercentFlicker_200Hz;                        ///< Out: PercentFlicker filtered above 200Hz
    double PercentFlicker_400Hz;                        ///< Out: PercentFlicker filtered above 400Hz
    double PercentFlicker_1000Hz;                       ///< Out: PercentFlicker filtered above 1000Hz
    double FlickerIndex;                                ///< Out: FlickerIndex: Ratio of Area1 (time-integrated output that exceeds average output) to Area2 (total time-integrated output). Range: 0 to 1. Insensitive to periodic frequency.
    double SVM;                                         ///< Out: Stroboscopic Visibility Measure. References: NEMA 77-2017 (Temporal Light Artifacts); CIE TN 006:2016 (Visual aspects of time-modulated lighting systems)
    double ASSIST_Mp;                                   ///< Out: ASSIST Perceptual Modulation metric, from "Recommended metric for assessing the direct perception of light source" Vol 11, Issue 3, Jan 2015; A. Bierman, RPI Lighting Research Center
} lsapi_ApiCall_ScalarTransient_GetFlickerMetrics_Params;

typedef struct lsapi_ApiCall_ScalarTransient_GetFlickerPstLM_Params   ///< Used with with ::lsapi_ScalarTransient_CtlFcn_GetFlickerPstLM
{
    lsapi_ApiCall_ScalarTransient_ControlHdr CtlHdr;    ///< See ..lsapi_ApiCall_ScalarTransient_ControlHdr Preparation above for details on preparation of this member.
    double PstLM;                                       ///< Out: ASSIST Perceptual Modulation metric, from "Recommended metric for assessing the direct perception of light source" Vol 11, Issue 3, Jan 2015; A. Bierman, RPI Lighting Research Center
} lsapi_ApiCall_ScalarTransient_GetFlickerPstLM_Params;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Math-specific functionality

#define lsapi_DEFAULT_WAVELENGTH_INTERVAL    1              ///< The API uses 1nm as a default interval for interpolated spectra

#define lsapi_math_CIE_NUM_TCS_CRI          15              ///< The number of Test Color Samples for CRI structs (original 14 plus R15 "Japanese skin")
#define lsapi_math_CIE_NUM_TCS_CQS          15              ///< The number of Test Color Samples for CQS structs.
#define lsapi_math_NUM_IES_TM30_HueBins     16              ///< IES TM-30-18 number of bins in {a',b'} plane

typedef enum                                                  ///< Value indicating the type of data represented by a spectrum
{
    lsapi_spectrum_Quantity_Unknown                 =  0,     ///< Spectrum quantity not specified
    lsapi_spectrum_Quantity_Flux                    =  1,     ///< Spectrum represents Spectral Flux
    lsapi_spectrum_Quantity_LuminousFlux            =  2,     ///< Spectrum represents Luminous Spectral Flux
    lsapi_spectrum_Quantity_Intensity               =  3,     ///< Spectrum represents Spectral Intensity  
    lsapi_spectrum_Quantity_Irradiance              =  4,     ///< Spectrum represents Spectral Irradiance  
    lsapi_spectrum_Quantity_Radiance                =  5,     ///< Spectrum represents Spectral Radiance  
    lsapi_spectrum_Quantity_Exitance                =  6,     ///< Spectrum represents Spectral Exitance  
} lsapi_spectrum_QuantityT;

typedef struct lsapi_SpectrumData_wc                ///< Support struct for Spectrum object data, including WaveCount array at spectrometer "pixel space" wavelengths. Replacement for lsapi_SpectrumData_nm.
{
    lsapi_WaveCount* pWaveCounts;                   ///< Array of count-for-wavelength data. Must be pre-allocated by caller.
    size_t ElementCount;                            ///< # of WaveCount structs in 'pWaveCounts'.
    lsapi_spectrum_QuantityT Quantity;              ///< Physical Quantity measured by Spectrum object, e.g. lsapi_spectrum_Quantity_Flux
} lsapi_SpectrumData_wc;


typedef struct lsapi_SpectrumData_nm        ///< Data associated with a Spectrum, presented at integral wavelengths with fixed intervals. (Notes: Formerly 'lsapi_SpectrumData'; DEPRECTATED, prefer 'lsapi_SpectrumData_wc'.)
{
    double* Values;                     ///< Array of spectrum values. \n In: Prepopulated values. \n Out: pre-alloc'ed array must be passed in, data will be placed in array.
    size_t NumValues;                   ///< In: Number of doubles (not bytes) in 'Values' ( i.e. use _countof(), not _sizeof() ). \n When retrieving data, the API will set this to the number of array elements that were populated. \n When retrieving data, if this value is too small to hold all elements that the API wishes to return, Counts will *NOT* be populated, lsapi_Result_InsufficientBuffer will be returned, and the API will set this var to the required number of elements.
    size_t FirstWavelength;             ///< The wavelength that corresponds to Values[0], in nanometers.
    size_t WavelengthInterval;          ///< The interval between wavelengths, in nanometers. \n When this value is zero the API will assume an interval of lsapi_DEFAULT_WAVELENGTH_INTERVAL.
} lsapi_SpectrumData_nm;


typedef struct lsapi_math_CIE1931xyY        ///< "CIE 1931 xyY" color space coords {x,y,Y}.
{
    double x;        ///< x coordinate in CIE 1931 color space
    double y;        ///< y coordinate in CIE 1931 color space
    double Y;        ///< Y coordinate in CIE 1931 color space
} lsapi_math_CIE1931xyY;

typedef struct lsapi_math_CIE1964xy10deg    ///< "CIE 1964" 10-degree observer color space coords {x10,y10}
{
    double x10;        ///< x coordinate in CIE 1964 10-degree observer color space
    double y10;        ///< y coordinate in CIE 1964 10-degree observer color space
} lsapi_math_CIE1964xy10deg;

typedef struct lsapi_math_CIE1960UCS        ///< "CIE 1960 UCS" color space coords {u,v}.
{
    double u;        ///< u coordinate in CIE 1960 UCS color space
    double v;        ///< v coordinate in CIE 1960 UCS color space
} lsapi_math_CIE1960UCS;

typedef struct lsapi_math_CIE1976UCS        ///< "CIE 1976 UCS" color space coords {u',v'}.
{
    double uPrime;        ///< u' coordinate in CIE 1976 UCS color space
    double vPrime;        ///< v' coordinate in CIE 1976 UCS color space
} lsapi_math_CIE1976UCS;

typedef struct lsapi_math_CIE1960Blackbody    ///< Correlated Color Temperature (CCT) and Distance of Chromaticity (DC) compare an SPD to a Blackbody radiator in CIE 1960 {u,v} color space.
{
    double  CCT;        ///< Correlated Color Temperature (CCT) is the blackbody temperature [Kelvin] of the nearest point to the SPD on the Planckian Locus.
    double  DC;         ///< Distance of Chromaticity (DC) is the Euclidian distance between this SPD and the nearest point on the Planckian Locus.
} lsapi_math_CIE1960Blackbody;

typedef struct lsapi_math_CIE1931SpectralLocus    ///< The Spectral Locus represents coords (in CIE 1931 {x,y} space) of visible monochromatic sources on the boundary of the visible "gamut", and intersects the line containing the DuT and Whitepoint coords.
{
    double  DominantWave;               ///< The monochromatic wavelength on the Spectral Locus at its intersection with the DuT-Whitepoint line which is closer to DuT; a measure of DuT's "hue".
    bool    HasDominantWave;            ///< DuT has no DominantWave if the DuT-Whitepoint line intersects the "line of purples" gamut boundary (which can't be reproduced with a monochromatic source) closer to DuT.
    double  Purity;                     ///< Ratio of DuT-Whitepoint distance (in {x,y} space) to Dominant-Whitepoint distance, as a percentage; the higher its Purity, the more closely the DuT resembles its Dominant monochromatic source.
    double  ComplementaryWave;          ///< The monochromatic wavelength on the Spectral Locus at its intersection with the DuT-Whitepoint line which is closer to Whitepoint; mixing this with DuT produces Whitepoint.
    bool    HasComplementaryWave;       ///< DuT has no ComplementaryWave if the DuT-Whitepoint line intersects the "line of purples" gamut boundary (which can't be reproduced with a monochromatic source) closer to Whitepoint.
} lsapi_math_CIE1931SpectralLocus;

typedef struct lsapi_math_RadiometricPeak    ///< Radiometric peak (maximum flux) wavelength, center, width, and centroid (useful for LED sources).
{
    double  PeakWavelength;             ///< Wavelength with peak power in the spectral power distribution (SPD).
    double  CenterWavelength;           ///< Wavelength at center of peak in the spectral power distribution (SPD).
    double  FullWidth;                  ///< Width of peak at half-maximum power in the spectral power distribution (SPD).
    double  CentroidWavelength;         ///< Wavelength at centroid of peak in the spectral power distribution (SPD).
} lsapi_math_RadiometricPeak;

typedef struct lsapi_math_CRI           ///< Color Rendering Indices (CRI) compare reflections from standard Test Color Samples (TCS) of an SPD illuminant, against the TCS reflections from an ideal reference illuminant.
{
    double General;                                 ///< General CRI is defined as the average of the first 8 Special (i.e. individual) CRIs.
    double Special[lsapi_math_CIE_NUM_TCS_CRI];     ///< Special CRI is an array of individual indices for each of the 14 standard Test Color Samples.
    double DC;                                      ///< Distance of Chromaticity (DC) in CIE 1960 {u,v} color space between this illuminant and the CRI algorithm's ideal reference illuminant.
} lsapi_math_CRI;

typedef struct lsapi_math_CQS           ///< NIST CQS compares reflections from standard Test Color Samples (TCS) of an SPD illuminant, against the TCS reflections from an ideal reference illuminant.
{
    double Qa;                                      ///< Overall CQS score
    double Individual[lsapi_math_CIE_NUM_TCS_CQS];  ///< Individual CQS scores, one for each of the 15 standard Test Color Samples.
    double DC;                                      ///< Distance of Chromaticity (DC) in CIE 1960 {u,v} color space between this illuminant and the CQS algorithm's ideal reference illuminant.
} lsapi_math_CQS;

typedef struct lsapi_math_TM30_HueBin   ///< IES TM-30-18 Hue Bin comparison metrics and Color Vector Graphic values.
{
    size_t  BinNumber;                  ///< Hue Bin number (1..16), using first-element-1 semantics. Note: BinNumber = n+1, where n is the lsapi_math_TM30::HueBin[n] array index.
    size_t  NumCES;                     ///< Number of CES samples in the Hue Bin, under the Reference illuminant.
    double  aPrimeAvg_Ref;              ///< Average a' coordinate in CIECAM02 color system for the CES samples in the Hue Bin, under the Reference illuminant.
    double  bPrimeAvg_Ref;              ///< Average b' coordinate in CIECAM02 color system for the CES samples in the Hue Bin, under the Reference illuminant.
    double  aPrimeAvg_DuT;              ///< Average a' coordinate in CIECAM02 color system for the CES samples in the Hue Bin, under the DuT illuminant.
    double  bPrimeAvg_DuT;              ///< Average b' coordinate in CIECAM02 color system for the CES samples in the Hue Bin, under the DuT illuminant.
    double  ColorVector_xRef;           ///< Average x coordinate under Reference illuminant normalized to the unit circle, for Color Vector Graphic.
    double  ColorVector_yRef;           ///< Average y coordinate under Reference illuminant normalized to the unit circle, for Color Vector Graphic.
    double  ColorVector_xDuT;           ///< Average x coordinate under DuT illuminant w.r.t. the normalized Reference coordinates, for Color Vector Graphic.
    double  ColorVector_yDuT;           ///< Average y coordinate under DuT illuminant w.r.t. the normalized Reference coordinates, for Color Vector Graphic.
    double  Rf_hue;                     ///< Average Fidelity Index for the Hue Bin.
    double  Rcs_hue;                    ///< Average Chroma Shift for the Hue Bin, as percentage.
    double  Rhs_hue;                    ///< Average Hue Shift for the Hue Bin, as percentage.
    unsigned short ColorVectorR;        ///< sRBG color values, for display of Color Vector Graphic
    unsigned short ColorVectorG;        ///< sRBG color values, for display of Color Vector Graphic
    unsigned short ColorVectorB;        ///< sRBG color values, for display of Color Vector Graphic
} lsapi_math_TM30_HueBin;

typedef struct lsapi_math_TM30          ///< IES TM-30-18 compares the appearance of 99 Color Evaluation Samples (CES) under an SPD illuminant against their appearance under an ideal reference illuminant, then groups the 99 CES into 16 Hue Bins, and presents comparison metrics for each Hue Bin, plus summary values 'Rf' and 'Rg' for all Hue Bins.
{
    double Rf;                                                      ///< Fidelity Index, comparable to CRI "Ra"
    double Rg;                                                      ///< Gamut Index: if Rg > 100, saturation/chroma of samples generally increases under DuT compared to Ref, otherwise it decreases.
    double Rf_ces[99];                                              ///< Sample Color Fidelity, a fidelity value for each of the 99 CES calculated using the same scaling protocol as for Rf.
    lsapi_math_TM30_HueBin HueBin[lsapi_math_NUM_IES_TM30_HueBins]; ///< Comparison metrics and Color Vector Graphic values for each of the 16 Hue Bins.
} lsapi_math_TM30;


typedef struct lsapi_math_SSL2015_FixedBins     ///< SSL2015 reference ANSI_C78.377-2015 defines the set of Fixed Bins at NominalCCT {2200,2500,2700,3000,3500,4500,4000,5000,5700,6500}.\n See ANSI_C78.377-2015 "Specifications for the Chromaticity of Solid State Lighting (SSL) Products"
{                                       
    size_t NominalCCT_7StepQuad;                    ///< Out: NominalCCT of Fixed 7-Step Quadrangle in which DUT lies, or 0 if none. (A DUT will lie in one or zero of the 7-Step Quadrangles.)
    size_t NominalCCT_4StepQuad;                    ///< Out: NominalCCT of Fixed 4-Step Quadrangle in which DUT lies, or 0 if none. (A DUT will lie in one or zero of the 4-Step Quadrangles.)
    size_t NominalCCT_NearestCircle;                ///< Out: NominalCCT of nearest {u',v'} Circle to the DUT. (A DUT will always have a nearest Circle.)
    double NumStepsToNearestCircle;                 ///< Out: Number of Steps (a measure of chromaticity difference) from DUT to nearest {u',v'} Circle center.
    bool   LiesIn4StepCircle;                       ///< Out: True if DUT lies within the nearest 4-Step {u',v'} Circle, i.e. NumStepsToNearestCircle <= 4.
} lsapi_math_SSL2015_FixedBins;

typedef struct lsapi_math_SSL2015_QuadCoords    ///< SSL2015 reference ANSI_C78.377-2015 defines the coordinates of the 4 vertices of a CCT quadrangle in CIE 1976 UCS color space
{
    lsapi_math_CIE1976UCS VertexA;                  ///< Out: {u',v'} coordinates of the first vertex
    lsapi_math_CIE1976UCS VertexB;                  ///< Out: {u',v'} coordinates of the second vertex
    lsapi_math_CIE1976UCS VertexC;                  ///< Out: {u',v'} coordinates of the third vertex
    lsapi_math_CIE1976UCS VertexD;                  ///< Out: {u',v'} coordinates of the fourth vertex
} lsapi_math_SSL2015_QuadCoords;

typedef struct lsapi_math_SSL2015_FlexibleBins  ///< SSL2015 reference ANSI_C78.377-2015 also defines the set of overlapping Flexible Bins.\n Caller must provide the TargetCCT to specify bins.
{
    size_t TargetCCT;                               ///< In/Out: Caller specifies the TargetCCT. SPECIAL CASE: Specify TargetCCT=0 to use the DUT's actual CCT.
    size_t NominalCCT;                              ///< Out: The NominalCCT of the Flexible bin nearest to the caller's TargetCCT.
    bool   LiesIn7StepQuad;                         ///< Out: True if DUT lies within the Flexible 7-Step Quadrangle corresponding to NominalCCT.
    bool   LiesIn4StepQuad;                         ///< Out: True if DUT lies within the Flexible 4-Step Quadrangle corresponding to NominalCCT.
    bool   LiesIn4StepCircle;                       ///< Out: True if DUT lies within the Flexible 4-Step Circle corresponding to NominalCCT.
    double NumStepsToCircle;                        ///< Out: Number of Steps (a measure of chromaticity difference) from DUT to center of Flexible 4-Step Circle corresponding to NominalCCT.
    lsapi_math_SSL2015_QuadCoords Coords_7StepQuad; ///< Out: Coords of 7-step quadrangle vertices in {u',v'} space.
    lsapi_math_SSL2015_QuadCoords Coords_4StepQuad; ///< Out: Coords of 4-step quadrangle vertices in {u',v'} space.
    lsapi_math_CIE1976UCS         CircleCenter;     ///< Out: Coords of circle center in {u',v'} space.
} lsapi_math_SSL2015_FlexibleBins;

typedef struct lsapi_CalibrationLampInfo    ///< Information about a Calibration Lamp
{
    wchar_t LampType[lsapi_TEXTBUF_SIZE];               ///< The calibration lamp type, e.g. "SCL-050"
    wchar_t SerialNumber[lsapi_TEXTBUF_SIZE];           ///< The lamp's serial number, e.g. "A6"
    double RatedVolts;                                  ///< The lamp's rated voltage, e.g. 4.2
    double RatedAmps;                                   ///< The lamp's rated current, e.g. 1.05
    double MaxHours;                                    ///< Maximum number of usable hours before lamp must be replaced or recalibrated
    double BurnedHours;                                 ///< Number of hours this lamp has been burned.
} lsapi_CalibrationLampInfo;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// General API functionality

typedef enum    ///< Used to specify the data contained in an API handle
{
    lsapi_SpectralObjectType_Unknown=0,                 ///< Invalid value
    lsapi_SpectralObjectType_Spectrum,                  ///< A generic spectrum
    lsapi_SpectralObjectType_ProcessedScan,             ///< A Processed Scan (i.e. a Device scan that has been processed)
} lsapi_SpectralObjectTypeT;

typedef enum    ///< Values for lsapi_ApiCall_Api_UtilitiesHdr.UtilityType
{
    lsapi_api_UtilityType_Unknown=0,                    ///< Invalid value.
    lsapi_api_UtilityType_GetLastErrorDetail,           ///< Retrieve textual details for last-returned error. Used with ::lsapi_ApiCall_Api_Utility_GetLastErrorDetail_Params
    lsapi_api_UtilityType_ReleaseHandle,                ///< Release a handle that was created by the API. Used with ::lsapi_ApiCall_Api_Utility_ReleaseHandle_Params
    lsapi_api_UtilityType_SetDatabasePath,              ///< Specify a custom location for the API's database file. Used with ::lsapi_ApiCall_Api_Utilities_SetDatabasePath_Params
} lsapi_api_UtilityTypeT;




typedef enum    ///< Values for lsapi_ApiCall_LicenseHdr.LicenseFcn
{
    lsapi_api_LicenseFcn_Unknown=0,                     ///< Invalid value.
    lsapi_api_LicenseFcn_GetSystemKey,                  ///< Retrieve the system-specific key. Used with ::lsapi_ApiCall_Api_License_GetSystemKey_Params
    lsapi_api_LicenseFcn_List,                          ///< Retrieve a list of installed Licenses. Used with ::lsapi_ApiCall_Api_License_List_Params
    lsapi_api_LicenseFcn_Contents,                      ///< Retrieve the contents of a License. Used with ::lsapi_ApiCall_Api_License_Contents_Params
    lsapi_api_LicenseFcn_Install,                       ///< Install a license. Used with ::lsapi_ApiCall_Api_License_Install_Params
    lsapi_api_LicenseFcn_Permissions,                   ///< Retrieve Licensed permissions for this system. Used with ::lsapi_ApiCall_Api_License_Permissions_Params
} lsapi_api_LicenseFcnT;

typedef enum     ///< Values for lsapi_LicensePermission.Permission
{
    lsapi_LicensePermission_Unknown=0,                  ///< Invalid value.
    lsapi_LicensePermission_API,                        ///< Use of the API is permitted on the System.
    lsapi_LicensePermission_Users,                      ///< Number of users permitted on the System.
    lsapi_LicensePermission_AC,                         ///< The Illumia+ AC module is permitted on the System.
    lsapi_LicensePermission_LM82,                       ///< The Illumia+ LM-82 module is permitted on the System.
    lsapi_LicensePermission_Maintenance,                ///< A Maintenance window for the system.
} lsapi_LicensePermissionT;

typedef struct lsapi_ApiCall_Api_Open_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Api_Open 
{
    lsapi_ApiCallHdr ApiCallHdr;                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_Version Apiver;                       ///< Out: Version of the API; always populated even when API Key is not valid
    bool Licensed;                              ///< Out: True if the system has an API license. False if it does not. If not licensed, only Virtual devices can be connected.
} lsapi_ApiCall_Api_Open_Params;

// Note: there is no custom struct for lsapi_ApiCall_Api_Close

// *** Utilities

typedef struct lsapi_ApiCall_Api_UtilitiesHdr   ///< Used with lsapi_ApiCall_Api_Utilities. \n Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Api_Utilities
{
    lsapi_ApiCallHdr ApiCallHdr;                ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_api_UtilityTypeT UtilityType;         ///< The type of utility to be performed.
} lsapi_ApiCall_Api_UtilitiesHdr;

typedef struct lsapi_ApiCall_Api_Utility_GetLastErrorDetail_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Api_Utilities and UtilsHdr.UtilityType = ::lsapi_api_UtilityType_GetLastErrorDetail
{
    lsapi_ApiCall_Api_UtilitiesHdr UtilsHdr;        ///< See UtilsHdr Preparation above for details on preparation of this member.
    wchar_t ErrorDetail[lsapi_TEXTBUF_SIZE];     ///< Out: The error detail text, if any. Many errors do not set detail text, in which case the function will return Success with an empty ErrorDetail string.
} lsapi_ApiCall_Api_Utility_GetLastErrorDetail_Params;

typedef struct lsapi_ApiCall_Api_Utility_ReleaseHandle_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Api_Utilities and UtilsHdr.UtilityType = ::lsapi_api_UtilityType_ReleaseHandle
{
    lsapi_ApiCall_Api_UtilitiesHdr UtilsHdr;        ///< See UtilsHdr Preparation above for details on preparation of this member.
    lsapi_HandleT hToRelease;                       ///< In: The handle to be released.
} lsapi_ApiCall_Api_Utility_ReleaseHandle_Params;

typedef struct lsapi_ApiCall_Api_Utilities_SetDatabasePath_Params ///< Set UtilsHdr.UtilityType = ::lsapi_api_UtilityType_SetDatabasePath
{
    lsapi_ApiCall_Api_UtilitiesHdr UtilsHdr;            ///< See UtilsHdr Preparation above for details on preparation of this member.
    wchar_t DbFilePath[lsapi_TEXTBUF_SIZE];             ///< In: Fully-qualified path for the database file. Path must exist. If supplied string is a path without a filename, the default API DB filename will be used. If a DB file does not exist in this location, one will be created.
} lsapi_ApiCall_Api_Utilities_SetDatabasePath_Params;


// *** License

typedef struct lsapi_LicenseItem                    ///< Licensing is no longer part of lsapi
{
    wchar_t Key[lsapi_TEXTBUF_SIZE];                ///< The name of the license item.
    wchar_t Value[lsapi_TEXTBUF_SIZE];              ///< The value for the license item. Format depends upon 'Key' (i.e. may be a string, or a string representation of a number, etc).
} lsapi_LicenseItem;

typedef struct lsapi_LicensePermission              ///< Licensing is no longer part of lsapi
{
    lsapi_LicensePermissionT Permission;            ///< The permission.
    wchar_t Value[lsapi_TEXTBUF_SIZE];              ///< The value for the license item. Format depends upon 'Key' (i.e. may be a string, or a string representation of a number, etc).
    unsigned long Expires;                          ///< Date/time when this permission expires, expressed as a number of seconds since Jan 1, 1970 (time_t). A zero-value indicates that it never expires.
} lsapi_LicensePermission;

typedef struct lsapi_ApiCall_LicenseHdr             ///< Used with lsapi_ApiCall_License. \n Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_License
{
    lsapi_ApiCallHdr ApiCallHdr;                    ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_api_LicenseFcnT LicenseFcn;               ///< The License function to be performed.
} lsapi_ApiCall_LicenseHdr;

typedef struct lsapi_ApiCall_Api_License_GetSystemKey_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_License and LicenseHdr.LicenseFcn = ::lsapi_api_LicenseFcn_GetSystemKey
{
    lsapi_ApiCall_LicenseHdr LicenseHdr;            ///< See LicenseHdr Preparation above for details on preparation of this member.
    wchar_t* pswKey;                                ///< Out: The System Key. You can pass nullptr to determine the required buffer size.
    size_t keySize;                                 ///< In: The size of the passed-in psKey buffer. Out: The number of wchars required to hold the entire key (when psKey==nullptr, or keySize is too small; return code will be lsapi_Result_InsufficientBuffer)
} lsapi_ApiCall_Api_License_GetSystemKey_Params;

typedef struct lsapi_ApiCall_Api_License_List_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_License and LicenseHdr.LicenseFcn = ::lsapi_api_LicenseFcn_List
{
    lsapi_ApiCall_LicenseHdr LicenseHdr;            ///< See LicenseHdr Preparation above for details on preparation of this member.
    wchar_t* pswGuids;                              ///< In: Filled with one or more comma-separated guid strings. Must be pre-allocated by caller.
    size_t BufferSize;                              ///< In/Out: # of wchar_t's in 'pInfo'.
} lsapi_ApiCall_Api_License_List_Params;

typedef struct lsapi_ApiCall_Api_License_Contents_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_License and LicenseHdr.LicenseFcn = ::lsapi_api_LicenseFcn_Contents
{
    lsapi_ApiCall_LicenseHdr LicenseHdr;            ///< See LicenseHdr Preparation above for details on preparation of this member.
    wchar_t LicenseGuid[lsapi_TEXTBUF_SIZE];        ///< In: The GUID of the License.
    lsapi_LicenseItem* pItems;                      ///< Out: An array of structs to hold details. Must be pre-allocated by caller.
    size_t ItemCount;                               ///< In/Out: # of structs 'pItems'.
} lsapi_ApiCall_Api_License_Contents_Params;

typedef struct lsapi_ApiCall_Api_License_Install_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_License and LicenseHdr.LicenseFcn = ::lsapi_api_LicenseFcn_Install
{
    lsapi_ApiCall_LicenseHdr LicenseHdr;            ///< See LicenseHdr Preparation above for details on preparation of this member.
    const wchar_t* pswEncryptedLicense;             ///< In: The encryped license.
    wchar_t LicenseGuid[lsapi_TEXTBUF_SIZE];        ///< Out: The GUID for the passed-in EncryptedLicense, if it is valid license data. This is filled even if installation fails.
} lsapi_ApiCall_Api_License_Install_Params;

typedef struct lsapi_ApiCall_Api_License_Permissions_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_License and LicenseHdr.LicenseFcn = ::lsapi_api_LicenseFcn_Permissions
{
    lsapi_ApiCall_LicenseHdr LicenseHdr;            ///< See LicenseHdr Preparation above for details on preparation of this member.
    lsapi_LicensePermission* pPermissions;          ///< Out: An array of structs to hold details. Must be pre-allocated by caller.
    size_t ItemCount;                               ///< In/Out: # of structs 'pPermissions'.
} lsapi_ApiCall_Api_License_Permissions_Params;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spectrum-specific functionality

typedef enum    ///< Values for lsapi_ApiCall_Spectrum_FunctionHdr.SpectrumFcn
{
    lsapi_Spectrum_Function_Unknown=0,              ///< Invalid value.
    lsapi_Spectrum_Function_GetRadiometry,          ///< Get Radiometry of a Spectrum object. Used with ::lsapi_ApiCall_Spectrum_Function_GetRadiometry_Params
    lsapi_Spectrum_Function_GetColorimetry,         ///< Get Colorimetry of a Spectrum object. Used with ::lsapi_ApiCall_Spectrum_Function_GetColorimetry_Params
    lsapi_Spectrum_Function_GetColorRendering,      ///< Get ColorRendering of a Spectrum object. Used with ::lsapi_ApiCall_Spectrum_Function_GetColorRendering_Params
    lsapi_Spectrum_Function_GetData,                ///< Retrieve a Spectrum object's data, including WaveCounts array. Used with ::lsapi_ApiCall_Spectrum_Function_GetData_Params
    lsapi_Spectrum_Function_Reconstitute,           ///< Create a new Spectrum object from caller's data. Used with ::lsapi_ApiCall_Spectrum_Function_Reconstitute_Params
    lsapi_Spectrum_Function_Resample,               ///< Resample an existing Spectrum object to a new one with different wavelengths, e.g. at regular 1nm intervals; conserves flux. Used with ::lsapi_ApiCall_Spectrum_Function_Resample_Params
    lsapi_Spectrum_Function_GetData_nm,             ///< Retrieve a Spectrum object's data known to be at fixed wavelength intervals; formerly lsapi_api_UtilityType_GetSpectrumData. Used with ::lsapi_ApiCall_Spectrum_Function_GetData_nm_Params
    lsapi_Spectrum_Function_GetSpectralLuminousFlux_nm,///< Retrieve a Spectrum object's Luminous Flux data at fixed wavelength intervals. Used with ::lsapi_ApiCall_Spectrum_Function_GetSpectralLuminousFlux_nm_Params
    lsapi_Spectrum_Function_Reconstitute_nm,        ///< Create a new Spectrum object from caller's data at fixed wavelength intervals; formerly lsapi_api_UtilityType_CreateSpectrum. Used with ::lsapi_ApiCall_Spectrum_Function_Reconstitute_nm_Params
    lsapi_Spectrum_Function_SaveToFile,             ///< Save Spectrum object to text file. Used with ::lsapi_ApiCall_Spectrum_Function_SaveToFile_Params
    lsapi_Spectrum_Function_LoadFromFile,           ///< Load Spectrum object from text file. Used with ::lsapi_ApiCall_Spectrum_Function_LoadFromFile_Params
} lsapi_Spectrum_FunctionT;


typedef struct lsapi_ApiCall_Spectrum_FunctionHdr   ///< Used with lsapi_ApiCall_Spectrum. \n Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum
{
    lsapi_ApiCallHdr ApiCallHdr;                    ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_Spectrum_FunctionT SpectrumFcn;           ///< The spectrum function to be performed.
} lsapi_ApiCall_Spectrum_FunctionHdr;


typedef struct lsapi_ApiCall_Spectrum_Function_GetData_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_GetData
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumHandleT hSpectrum;                    ///< In:  Spectrum object handle
    lsapi_SpectrumData_wc SpectrumData;                 ///< Out: Spectrum data.
} lsapi_ApiCall_Spectrum_Function_GetData_Params;


typedef struct lsapi_ApiCall_Spectrum_Function_Reconstitute_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_Reconstitute
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumData_wc SpectrumData;                 ///< In:  Spectrum data, including pWaveCounts array with ElementCount elements.
    lsapi_SpectrumHandleT hCreatedSpectrum;             ///< Out: Handle to the created Spectrum object.
} lsapi_ApiCall_Spectrum_Function_Reconstitute_Params;


typedef struct lsapi_ApiCall_Spectrum_Function_Resample_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_Resample
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumHandleT hSpectrum;                    ///< In:  Spectrum object handle
    lsapi_SpectrumHandleT hReferenceSpectrum;           ///< In:  Spectrum object with wavelength set to use for resampled Spectrum, or 0 with non-zero RegularInterval_nm
    unsigned long RegularInterval_nm;                   ///< In:  Regular Interval for wavelength set of resampled Spectrum, or 0 with a valid hReferenceSpectrum
    lsapi_SpectrumHandleT hResampledSpectrum;           ///< Out: Handle to a new Spectrum object, with data resampled to specified wavelength set
} lsapi_ApiCall_Spectrum_Function_Resample_Params;


typedef struct lsapi_ApiCall_Spectrum_Function_GetData_nm_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_GetData_nm
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumHandleT hSpectrum;                    ///< In: Handle containing the API's copy of the spectrum data
    lsapi_SpectrumData_nm Data;                         ///< Out: Spectrum data. Note: fails if Spectrum does not represent data at fixed-interval wavelengths.
} lsapi_ApiCall_Spectrum_Function_GetData_nm_Params;


typedef struct lsapi_ApiCall_Spectrum_Function_GetSpectralLuminousFlux_nm_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_GetSpectralLuminousFlux_nm
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumHandleT hSpectrum;                    ///< In: Handle containing the API's copy of the spectrum data
    lsapi_SpectrumData_nm SpectralLuminousFlux;         ///< Out: Spectral Luminous Flux data, represented at fixed-interval wavelengths.
} lsapi_ApiCall_Spectrum_Function_GetSpectralLuminousFlux_nm_Params;


typedef struct lsapi_ApiCall_Spectrum_Function_Reconstitute_nm_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_Reconstitute_nm
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumData_nm Data;                         ///< In: Spectrum data array, whose number of elements must satisfy: numElements = 1 + (nWavelengthHigh_nm - nWavelengthLow_nm) div nWavelengthInterval.
    lsapi_SpectrumHandleT hCreatedSpectrum;             ///< Out: The handle to the created spectrum.
} lsapi_ApiCall_Spectrum_Function_Reconstitute_nm_Params;


typedef struct lsapi_ApiCall_Spectrum_Function_SaveToFile_Params      ///< Use SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_SaveToFile
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    wchar_t FilePath[lsapi_TEXTBUF_SIZE];               ///< In: Fully-qualified path for the file to save
    wchar_t Description[lsapi_TEXTBUF_SIZE];            ///< In: Client-provided Description for Spectrum object
    lsapi_SpectrumHandleT hSpectrumToSave;              ///< In: Handle of Spectrum object to save
    bool Overwrite;                                     ///< In: Pass true to overwrite any existing file. If false and file exists, save will fail.
} lsapi_ApiCall_Spectrum_Function_SaveToFile_Params;

typedef struct lsapi_ApiCall_Spectrum_Function_LoadFromFile_Params   ///< Use SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_LoadFromFile
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;     ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    wchar_t FilePath[lsapi_TEXTBUF_SIZE];               ///< In: Fully-qualified path to the file to load
    wchar_t Description[lsapi_TEXTBUF_SIZE];            ///< Out: Client-provided Description for Spectrum object
    lsapi_SpectrumHandleT hSpectrumLoaded;              ///< Out: The handle of the newly-loaded Spectrum object
} lsapi_ApiCall_Spectrum_Function_LoadFromFile_Params;


typedef struct lsapi_spectrum_RadiometryInRange             ///< struct for calculated Radiometry values returned by \ref dox_w32_analysis_fcns_GetRadiometry
{
	double FullRangeWavelengthLower;						///< Lower wavelength (nm) of the Spectrum's full range for integrated Flux. Note: value is slightly less than the Spectrum's first wavelength, because of first interval's implicit width.
	double FullRangeWavelengthUpper;						///< Upper wavelength (nm) of the Spectrum's full range for integrated Flux. Note: value is slightly greater than the Spectrum's last wavelength, because of last interval's implicit width.
	double RangeWavelengthLower;							///< Lower wavelength (nm) of the specified range: higher value of TargetRangeLowWavelength or FullRangeWavelengthLower
	double RangeWavelengthUpper;							///< Upper wavelength (nm) of the specified range: lower value of TargetRangeHighWavelength or FullRangeWavelengthUpper
	double RadiantFlux;										///< Radiant Flux (Watts) over the specified range.
	double PhotonFlux;										///< Photon Flux (micromoles per sec) over the specified range.
	double LuminousFlux;									///< Luminous Flux (lumens) over the specified range.
	lsapi_math_RadiometricPeak PeakSpectralFlux;			///< Wavelength, center, width, and centroid of the peak spectral flux (W/nm) in the specified range.
} lsapi_spectrum_RadiometryInRange;

typedef struct lsapi_ApiCall_Spectrum_Function_GetRadiometry_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_GetRadiometry
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;         ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumHandleT hSpectrum;                        ///< In: Spectrum object handle
    double  TargetRangeWavelengthLower;                     ///< In: Lower wavelength of user-specified range. SPECIAL CASE: Use special value 0 to specify the lower wavelength of the Spectrum's full range i.e. FullRangeWavelengthLower.
    double  TargetRangeWavelengthUpper;                     ///< In: Upper wavelength of the spectral range.  SPECIAL CASE: Use special value 0 to specify the upper wavelength of the Spectrum's full range i.e. FullRangeWavelengthUpper.
    lsapi_spectrum_RadiometryInRange RadiometryInRange;     ///< Out: Radiant flux and peak data
} lsapi_ApiCall_Spectrum_Function_GetRadiometry_Params;


typedef struct lsapi_spectrum_Colorimetry                   ///< struct for calculated Colorimetry values returned by \ref dox_w32_analysis_fcns_GetColorimetry
{
    double TotalLuminousFlux;                               ///< Total Luminous Flux (lumens) over the DUT's spectral range.
    double ScotopicFlux;                                    ///< Scotopic Flux (scotopic lumens), based on the CIE scotopic visibility function (for low light conditions)
    lsapi_math_CIE1931xyY CIE1931xyY;                       ///< DUT coords {x,y,Y} in CIE 1931 color space.
    lsapi_math_CIE1964xy10deg CIE1964xy10deg;               ///< DUT coords {x, y} in CIE 1964 10 degree observer space.
    lsapi_math_CIE1960UCS CIE1960UCS;                       ///< DUT coords {u,v} in CIE 1960 UCS color space.
    lsapi_math_CIE1976UCS CIE1976UCS;                       ///< DUT coords {u',v'} in CIE 1976 UCS color space.
    lsapi_math_CIE1960Blackbody CIE1960Blackbody;           ///< DUT's CCT (Correlated Color Temperature) and DC compare it to the nearest Blackbody radiator in CIE 1960 {u,v} color space.
    lsapi_math_CIE1931SpectralLocus SpectralLocus;          ///< DUT's Dominant Wavelength and other data relating to the Spectral Locus in CIE 1931 {x,y} color space.
    lsapi_math_SSL2015_FixedBins SSL2015_FixedBins;         ///< DUT proximity to Fixed NominalCCT Bins, from SSL2015 reference ANSI_C78.377-2015.
    lsapi_math_SSL2015_FlexibleBins SSL2015_FlexibleBins;   ///< DUT proximity to the Flexible NominalCCT Bins corresponding to user's TargetCCT, from SSL2015 reference ANSI_C78.377-2015.
} lsapi_spectrum_Colorimetry;

typedef struct lsapi_ApiCall_Spectrum_Function_GetColorimetry_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_GetColorimetry
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;         ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumHandleT hSpectrum;                        ///< In: Spectrum object handle
    size_t BinningTargetCCT;                                ///< In: Caller specifies the TargetCCT for SSL2015 FlexibleBins. SPECIAL CASE: Specify TargetCCT=0 to use the DUT's actual CCT.
    lsapi_spectrum_Colorimetry Colorimetry;                 ///< Out: Colorimetry results
} lsapi_ApiCall_Spectrum_Function_GetColorimetry_Params;


typedef struct lsapi_ApiCall_Spectrum_Function_GetColorRendering_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Spectrum and SpectrumHdr.SpectrumFcn = ::lsapi_Spectrum_Function_GetColorRendering
{
    lsapi_ApiCall_Spectrum_FunctionHdr SpectrumHdr;         ///< See ..Spectrum_FunctionHdr Preparation above for details on preparation of this member.
    lsapi_SpectrumHandleT hSpectrum;                        ///< In: Spectrum object handle
    lsapi_math_CRI CRI;                                     ///< Out: CRI (Color Rendering Indices) quantify the apparent colors of standard Test Color Samples (TCS) when illuminated by the DUT.
    lsapi_math_CQS CQS;                                     ///< Out: NIST CQS is an alternative to CRI to quantify the apparent colors of standard TCS when illuminated by the DUT.
    lsapi_math_TM30 TM30;                                   ///< Out: IES TM-30-18 is an alternative to CRI to quantify the apparent colors of standard TCS when illuminated by the DUT.
} lsapi_ApiCall_Spectrum_Function_GetColorRendering_Params;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Calibration-specific functionality


typedef enum    ///< Values for lsapi_ApiCall_Calibration_FunctionHdr.CalFcn
{
    lsapi_calibration_Function_Unknown=0,               ///< Invalid value.
    lsapi_calibration_Function_List,                    ///< Retrieve a list of all known calibrations. Used with ::lsapi_ApiCall_Calibration_Function_List_Params
    lsapi_calibration_Function_Create,                  ///< Create a new calibration. Used with ::lsapi_ApiCall_Calibration_Function_Create_Params
    lsapi_calibration_Function_CreateEx,                ///< Create a new calibration. Used with ::lsapi_ApiCall_Calibration_Function_CreateEx_Params
    lsapi_calibration_Function_Save,                    ///< Save a calibration. Used with ::lsapi_ApiCall_Calibration_Function_Save_Params
    lsapi_calibration_Function_Load,                    ///< Load an existing calibration. Used with ::lsapi_ApiCall_Calibration_Function_Load_Params
    lsapi_calibration_Function_Delete,                  ///< Delete an existing calibration. Used with ::lsapi_ApiCall_Calibration_Function_Delete_Params
    lsapi_calibration_Function_UpdateAuxDut,            ///< Update the AuxWithDut scan for an existing calibration. Used with ::lsapi_ApiCall_Calibration_Function_UpdateAuxDut_Params
    lsapi_calibration_Function_Apply,                   ///< Apply the given calibration to the supplied DUT scan. Used with ::lsapi_ApiCall_Calibration_Function_Apply_Params
    lsapi_calibration_Function_InstallCalLamp,          ///< Add a Calibration Lamp to the API's list of Cal Lamps, from data in a text file. Used with ::lsapi_ApiCall_Calibration_Function_InstallCalLamp_Params
    lsapi_calibration_Function_InstallCalLampData,      ///< Add a Calibration Lamp to the API's list of Cal Lamps, from data supplied directly. Used with ::lsapi_ApiCall_Calibration_Function_InstallCalLampData_Params
    lsapi_calibration_Function_ListCalLamps,            ///< Retrieve a list of all known Calibration lamps. Used with ::lsapi_ApiCall_Calibration_Function_ListCalLamps_Params
    lsapi_calibration_Function_AddCalLampBurnTime,      ///< Add to a Calibration lamp's burn time. Used with ::lsapi_ApiCall_Calibration_Function_AddCalLampBurnTime_Params
    lsapi_calibration_Function_GetCalLampData,          ///< Retrieve the wavelength data for this Cal lamp. Used with ::lsapi_ApiCall_Calibration_Function_GetCalLampData_Params
    lsapi_calibration_Function_GetData,                 ///< Retrieve data, including scan handles, for an existing calibration. Used with ::lsapi_ApiCall_Calibration_Function_GetData_Params
} lsapi_calibration_FunctionT;

typedef struct lsapi_CalibrationInfo        ///< Used with lsapi_ApiCall_Calibration. \n Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration
{
    wchar_t CalName[lsapi_TEXTBUF_SIZE];                ///< The calibration's name.
    unsigned long Timestamp;                            ///< Date/time the calibration was created, expressed as a number of seconds since Jan 1, 1970 (time_t)
    lsapi_device_DeviceInfo SpectrometerInfo;           ///< Type and serial number of Spectrometer used for calibration.
    lsapi_CalibrationLampInfo LampInfo;                 ///< The Lamp Type and Serial Number of the Calibration lamp (supplied by Labsphere with the Calibration lamp). \n Must have been previously installed into the API via ::lsapi_calibration_Function_InstallCalLamp
    lsapi_scan_ProcessingOptionsT AppliedOptions;       ///< Options used to process the calibration's scans
    double AppliedStrayLightFactor;                     ///< StrayLight Factor used to process the calibration's scans
} lsapi_CalibrationInfo;
                                                                                                                

typedef struct lsapi_ApiCall_Calibration_FunctionHdr ///< Used with lsapi_ApiCall_Calibration. \n Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration
{
    lsapi_ApiCallHdr ApiCallHdr;                     ///< Init ApiCallHdr.ApiCall and ApiCallHdr.hdr.StructSize before calling ::lsapi_ApiCall()
    lsapi_calibration_FunctionT CalFcn;              ///< The calibration function to be performed.
} lsapi_ApiCall_Calibration_FunctionHdr;

typedef struct lsapi_ApiCall_Calibration_Function_List_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_List
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;    ///< See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationInfo* pInfo;                       ///< In: Array of structures. A pre-alloc'ed array must be passed in, data will be placed in array.
    size_t NumInfoCount;                                ///< In: Number of structures (not bytes) in 'pInfo'. \n On function return the API will set this to the number of array elements that were populated. \n If the passed-in value is too small to hold all elements that the API wishes to return, pInfo will *NOT* be populated, lsapi_Result_InsufficientBuffer will be returned, and the API will set this var to the required number of elements.
} lsapi_ApiCall_Calibration_Function_List_Params;

typedef struct lsapi_Calibration_Scan_Set    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_Create
{
    lsapi_device_spectrometer_FilterT Filter;                   ///< The filter used for the constituent scans.
    lsapi_ProcessedScanHandleT hProcessedCalLampScan;           ///< The processed Calibration Lamp scan.
    lsapi_ProcessedScanHandleT hProcessedAuxWithCalScan;        ///< The processed Aux Lamp scan with the Cal Lamp installed.
    lsapi_ProcessedScanHandleT hProcessedAuxWithDutScan;        ///< The processed Aux Lamp scan with the DUT installed.
} lsapi_Calibration_Scan_Set;

typedef struct lsapi_ApiCall_Calibration_Function_Create_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_Create
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_DeviceHandleT hDvc;                                   ///< In: The device used to take the CalLamp, AuxWithCal, and AuxWithDut scans.
    lsapi_CalibrationLampInfo LampInfo;                         ///< In: The Lamp Type and Serial Number of the Calibration lamp. \n Must have been previously installed into the API via ::lsapi_calibration_Function_InstallCalLamp
    lsapi_Calibration_Scan_Set ScanSets[LSAPI_MAX_FILTERS];     ///< In: The array of scan sets, one set for each of the spectrometer's filters. Zero out unused elements.
    bool ReleaseHandles;                                        ///< In: Pass 'true' to have the API release all handles in 'ScanSets' (typically, these scans were taken solely for creating the calibration). \n If you pass 'false' the caller is responsible for releasing all handles in 'ScanSets'.
    lsapi_CalibrationHandleT hCreatedCal;                       ///< Out: The created calibration.
} lsapi_ApiCall_Calibration_Function_Create_Params;

typedef struct lsapi_ApiCall_Calibration_Function_CreateEx_Params ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_CreateEx
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_device_DeviceInfo DeviceInfo;                         ///< In: The DeviceInfo for the spectrometer that was used to take the CalLamp, AuxWithCal, and AuxWithDut scans.
    lsapi_CalibrationLampInfo LampInfo;                         ///< In: The Lamp Type and Serial Number of the Calibration lamp. \n Must have been previously installed into the API via ::lsapi_calibration_Function_InstallCalLamp
    lsapi_Calibration_Scan_Set ScanSets[LSAPI_MAX_FILTERS];     ///< In: The array of scan sets, one set for each of the spectrometer's filters. Zero out unused elements.
    bool ReleaseHandles;                                        ///< In: Pass 'true' to have the API release all handles in 'ScanSets' (typically, these scans were taken solely for creating the calibration). \n If you pass 'false' the caller is responsible for releasing all handles in 'ScanSets'.
    lsapi_CalibrationHandleT hCreatedCal;                       ///< Out: The created calibration.
} lsapi_ApiCall_Calibration_Function_CreateEx_Params;

typedef struct lsapi_ApiCall_Calibration_Function_Save_Params   ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_Save
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    wchar_t CalName[lsapi_TEXTBUF_SIZE];                        ///< In: The calibration's name. Must be unique.
    lsapi_CalibrationHandleT hCalToSave;                        ///< In: The calibration to be saved. Handle created via lsapi_calibration_Function_Create.
} lsapi_ApiCall_Calibration_Function_Save_Params;

typedef struct lsapi_ApiCall_Calibration_Function_Load_Params   ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_Load
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    wchar_t CalName[lsapi_TEXTBUF_SIZE];                        ///< In: The name of the calibration to load. Must have been previously saved via lsapi_calibration_Function_Save.
    lsapi_CalibrationHandleT hLoadedCal;                        ///< Out: The loaded calibration.
} lsapi_ApiCall_Calibration_Function_Load_Params;

typedef struct lsapi_ApiCall_Calibration_Function_Delete_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_Delete
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    wchar_t CalName[lsapi_TEXTBUF_SIZE];                        ///< In: The name of the existing calibration to be deleted.
} lsapi_ApiCall_Calibration_Function_Delete_Params;

typedef struct lsapi_ApiCall_Calibration_Function_UpdateAuxDut_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_UpdateAuxDut
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationHandleT hExistingCal;                      ///< In: The existing calibration to be updated.
    lsapi_ProcessedScanHandleT NewProcessedAuxWithDutScans[LSAPI_MAX_FILTERS];  ///< In: The updated, pre-processed Aux-with-DUT-scans, one per filter. Zero out unused elements.
    bool ReleaseHandles;                                        ///< In: Pass 'true' to have the API release all handles in 'NewProcessedAuxWithDutScans' (typically, these scans were taken solely for creating the calibration). \n If you pass 'false' the caller is responsible for releasing all handles in 'NewProcessedAuxWithDutScans'.
    lsapi_CalibrationHandleT hUpdatedCal;                       ///< Out: The newly created calibration which combines hExistingCal with the NewAuxWithDut values.
} lsapi_ApiCall_Calibration_Function_UpdateAuxDut_Params;

typedef struct lsapi_ApiCall_Calibration_Function_Apply_Params  ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_Apply
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationHandleT hCal;                              ///< In: The calibration to be applied.
    lsapi_ProcessedScanHandleT hProcessedScan;                  ///< In: The pre-processed DUT scan to be calibrated
    bool PerformAbsorptionCorrection;                           ///< In: Pass 'true' if absorption correction should be applied
    lsapi_SpectrumHandleT hCalibratedSpectrum;                  ///< Out: The handle to the created Calibrated Spectrum.
} lsapi_ApiCall_Calibration_Function_Apply_Params;

typedef struct lsapi_ApiCall_Calibration_Function_InstallCalLamp_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_InstallCalLamp
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    wchar_t FilePath[lsapi_TEXTBUF_SIZE];                       ///< In: The path to the CalLamp calibration file to be used in the calibration.
    lsapi_CalibrationLampInfo LampInfo;                         ///< In: The Lamp Type and Serial Number associated with 'CalLampFilePath'.
} lsapi_ApiCall_Calibration_Function_InstallCalLamp_Params;

typedef struct lsapi_ApiCall_Calibration_Function_InstallCalLampData_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_InstallCalLampData
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationLampInfo LampInfo;                         ///< In: The Lamp Type and Serial Number associated with 'CalLampFilePath'.
    lsapi_SpectrumData_nm SpectrumData;                         ///< In: See lsapi_SpectrumData_nm docs for preparation
} lsapi_ApiCall_Calibration_Function_InstallCalLampData_Params;

typedef struct lsapi_ApiCall_Calibration_Function_ListCalLamps_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_ListCalLamps
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationLampInfo* pInfo;                           ///< In: A pre-alloc'ed array to hold the information.
    size_t NumInfoCount;                                        ///< In: Number of structures (not bytes) in 'pInfo'. \n On function return the API will set this to the number of array elements that were populated. \n If the passed-in value is too small to hold all elements that the API wishes to return, pInfo will *NOT* be populated, lsapi_Result_InsufficientBuffer will be returned, and the API will set this var to the required number of elements.
} lsapi_ApiCall_Calibration_Function_ListCalLamps_Params;

typedef struct lsapi_ApiCall_Calibration_Function_AddCalLampBurnTime_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_AddCalLampBurnTime
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationLampInfo LampInfo;                         ///< In:  Used to specify the Lamp Type and Serial Number. Other fields are ignored.
    double AdditionalBurnHours;                                 ///< In:  Number of hours to add to this lamp's burn time.
    double UpdatedBurnHours;                                    ///< Out: New total burn hours for this lamp, after AdditionalBurnHours was added
} lsapi_ApiCall_Calibration_Function_AddCalLampBurnTime_Params;

typedef struct lsapi_ApiCall_Calibration_Function_GetCalLampData_Params    ///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_GetCalLampData
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationLampInfo LampInfo;                         ///< In: The Lamp Type and Serial Number associated with 'CalLampFilePath'.
    lsapi_SpectrumData_nm SpectrumData;                         ///< In/Out: See lsapi_SpectrumData_nm docs for preparation
} lsapi_ApiCall_Calibration_Function_GetCalLampData_Params;

typedef struct lsapi_ApiCall_Calibration_Function_GetData_Params///< Pass to ::lsapi_ApiCall() with ApiCallHdr.ApiCall = ::lsapi_ApiCall_Calibration and CalFcnHdr.CalFcn = ::lsapi_calibration_Function_GetData
{
    lsapi_ApiCall_Calibration_FunctionHdr CalFcnHdr;            ///< In: See CalFcnHdr Preparation above for details on preparation of this member.
    lsapi_CalibrationHandleT hCal;                              ///< In: The calibration whose data is sought.
    lsapi_CalibrationInfo CalibrationInfo;                      ///< Out: The Calibration Info (see ::lsapi_CalibrationInfo)
    lsapi_Calibration_Scan_Set ScanSets[LSAPI_MAX_FILTERS];     ///< Out: The array of scan sets, one set for each of the spectrometer's filters; unused elements are zero.
    lsapi_SpectrumHandleT hCalLampFlux_wc;                      ///< Out: The CalLampFlux interpolated to align with Scan "pixel space" wavelengths.
    int CalLampFluxScanStartOffset;                             ///< Out: Offset of CalLampFlux_wc elements relative to Scan elements.
} lsapi_ApiCall_Calibration_Function_GetData_Params;


#pragma pack(pop)

#endif //- LSAPI_H_74948D10C9EE4B3090CE3B2851C7F0B6

#ifdef __cplusplus      //-
}                       //-
#endif                  //-
