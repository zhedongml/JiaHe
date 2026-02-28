Attribute VB_Name = "LKIF2"
Option Explicit

' Return Code List
Public Enum RC
    RC_OK = &H0                      ' The operation is completed successfully.
    '''''''''''''''''''''''
    ' Communication error from controller notification
    '
    RC_NAK_COMMAND = &H1001          ' Command error
    RC_NAK_COMMAND_LENGTH = &H1002   ' Command length error
    RC_NAK_TIMEOUT = &H1003          ' Timeout
    RC_NAK_CHECKSUM = &H1004         ' Check sum error
    RC_NAK_INVALID_STATE = &H1005    ' Status error
    RC_NAK_OTHER = &H1006            ' Other error
    RC_NAK_PARAMETER = &H1007        ' Parameter error
    RC_NAK_OUT_STAGE = &H1008        ' OUT calculation count limitation error
    RC_NAK_OUT_HEAD_NUM = &H1009     ' No. of used head/OUT over error
    RC_NAK_OUT_INVALID_CALC = &H100A ' OUT which cannot be used for calculation was specified for calculation.
    RC_NAK_OUT_VOID = &H100B         ' OUT which specified for calculation is not found.
    RC_NAK_INVALID_CYCLE = &H100C    ' Unavailable sampling cycle
    RC_NAK_CTRL_ERROR = &H100D       ' Main unit error
    RC_NAK_SRAM_ERROR = &H100E       ' Setting value error
    '''''''''''''''''''''''
    ' Communication DLL error notification
    '
    RC_ERR_OPEN_DEVICE = &H2000     ' Opening the device failed.
    RC_ERR_NO_DEVICE = &H2001       ' The device is not open.
    RC_ERR_SEND = &H2002            ' Command sending error
    RC_ERR_RECEIVE = &H2003         ' Response receiving error
    RC_ERR_TIMEOUT = &H2004         ' Timeout
    RC_ERR_NODATA = &H2005          ' No data
    RC_ERR_NOMEMORY = &H2006        ' No free memory
    
    RC_ERR_DISCONNECT = &H2007      ' Cable disconnection suspected
    RC_ERR_UNKNOWN = &H2008         ' Undefined error
End Enum

' Measurement value structures
Public Enum LKIF_FLOATRESULT
    LKIF_FLOATRESULT_VALID          ' valid data
    LKIF_FLOATRESULT_RANGEOVER_P    ' over range at positive (+) side
    LKIF_FLOATRESULT_RANGEOVER_N    ' over range at negative (-) side
    LKIF_FLOATRESULT_WAITING        ' Wait for comparator result
    LKIF_FLOATRESULT_ALARM          ' alarm
    LKIF_FLOATRESULT_INVALID        ' Invalid (error, etc.)
End Enum

' Set ABLE
Public Enum LKIF_ABLEMODE
    LKIF_ABLEMODE_AUTO              ' AUTO
    LKIF_ABLEMODE_MANUAL            ' manual
End Enum

' Set Measurement Mode
Public Enum LKIF_MEASUREMODE
    LKIF_MEASUREMODE_NORMAL                         ' normal
    LKIF_MEASUREMODE_HALF_T                         ' translucent object
    LKIF_MEASUREMDOE_TRAN_1                         ' transparent object
    LKIF_MEASUREMODE_TRAN_2                         ' transparent object 2
    LKIF_MEASUREMODE_MRS                            ' multireflective object
    LKIF_MEASUREMODE_OPAQUE = LKIF_MEASUREMODE_MRS  ' Semi opaque object
End Enum

' Set Basic Point
Public Enum LKIF_BASICPOINT
    LKIF_BASICPOINT_NEAR            ' NEAR
    LKIF_BASICPOINT_FAR             ' FAR
End Enum

' Set Mounting Mode
Public Enum LKIF_REFLECTIONMODE
    LKIF_REFLECTIONMODE_DIFFUSION   ' diffuse reflection
    LKIF_REFLECTIONMODE_MIRROR      ' specular reflection
End Enum

' Set Median
Public Enum LKIF_MEDIAN
    LKIF_MEDIAN_OFF             ' OFF
    LKIF_MEDIAN_7               ' 7 point
    LKIF_MEDIAN_15              ' 15 point
    LKIF_MEDIAN_31              ' 31 point
End Enum

' Set LASER CTRL group.
Public Enum LKIF_LASER_CTRL_GROUP
    LKIF_LASER_CTRL_GROUP_1         ' LASER CTRL 1
    LKIF_LASER_CTRL_GROUP_2         ' LASER CTRL 2
End Enum

' Set a range.
Public Enum LKIF_RANGE
    LKIF_RANGE_CENTER       ' CENTER
    LKIF_RANGE_FAR          ' FAR
End Enum

' Set Mutual Interference Prevention Group
Public Enum LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP
    LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP_A ' Group A
    LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP_B ' Group B
    LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP_C ' Group C
End Enum

' Set calculation method.
Public Enum LKIF_CALCMETHOD
    LKIF_CALCMETHOD_HEADA                   ' head A
    LKIF_CALCMETHOD_HEADB                   ' head B
    LKIF_CALCMETHOD_HEAD_HEADA_PLUS_HEADB   ' head A+head B
    LKIF_CALCMETHOD_HEAD_HEADA_MINUS_HEADB  ' head A-head B
    LKIF_CALCMETHOD_HEAD_HEADA_TRANSPARENT  ' head A transparent object
    LKIF_CALCMETHOD_HEAD_HEADB_TRANSPARENT  ' head B transparent object

    LKIF_CALCMETHOD_HEAD = 0                ' head
    LKIF_CALCMETHOD_OUT                     ' OUT
    LKIF_CALCMETHOD_ADD                     ' ADD
    LKIF_CALCMETHOD_SUB                     ' SUB
    LKIF_CALCMETHOD_AVE                     ' AVE
    LKIF_CALCMETHOD_PP                      ' P-P
    LKIF_CALCMETHOD_MAX                     ' MAX
    LKIF_CALCMETHOD_MIN                     ' MIN
End Enum

' Set Target Surface
Public Enum LKIF_CALCTARGET
    LKIF_CALCTARGET_PEAK_1                  ' peak 1
    LKIF_CALCTARGET_PEAK_2                  ' peak 2
    LKIF_CALCTARGET_PEAK_3                  ' peak 3
    LKIF_CALCTARGET_PEAK_4                  ' peak 4
    LKIF_CALCTARGET_PEAK_1_2                ' peak 1-peak 2
    LKIF_CALCTARGET_PEAK_1_3                ' peak 1-peak 3
    LKIF_CALCTARGET_PEAK_1_4                ' peak 1-peak 4
    LKIF_CALCTARGET_PEAK_2_3                ' peak 2-peak 3
    LKIF_CALCTARGET_PEAK_2_4                ' peak 2-peak 4
    LKIF_CALCTARGET_PEAK_3_4                ' peak 3-peak 4
End Enum

' Set Filter Mode
Public Enum LKIF_FILTERMODE
    LKIF_FILTERMODE_MOVING_AVERAGE          ' moving average
    LKIF_FILTERMODE_LOWPASS                 ' low pass filter
    LKIF_FILTERMODE_HIGHPASS                ' high pass filter
End Enum

' Specify the filter parameter. (The number of averaging for moving average, or the cutoff frequency for LPF/HPF.)
Public Enum LKIF_FILTERPARA
    LKIF_FILTERPARA_AVE_1 = 0   ' 1 time
    LKIF_FILTERPARA_AVE_4       ' 4 times
    LKIF_FILTERPARA_AVE_16      ' 16 times
    LKIF_FILTERPARA_AVE_64      ' 64 times
    LKIF_FILTERPARA_AVE_256     ' 256 times
    LKIF_FILTERPARA_AVE_1024    ' 1024 times
    LKIF_FILTERPARA_AVE_4096    ' 4096 times
    LKIF_FILTERPARA_AVE_16384   ' 16384 times
    LKIF_FILTERPARA_AVE_65536   ' 65536 times
    LKIF_FILTERPARA_AVE_262144  ' 262144 times
    
    LKIF_FILTERPARA_COFF_3000 = 0   ' 3000Hz
    LKIF_FILTERPARA_COFF_1000       ' 1000Hz
    LKIF_FILTERPARA_COFF_300        ' 300Hz
    LKIF_FILTERPARA_COFF_100        ' 100Hz
    LKIF_FILTERPARA_COFF_30         ' 30Hz
    LKIF_FILTERPARA_COFF_10         ' 10Hz
    LKIF_FILTERPARA_COFF_3          ' 3Hz
    LKIF_FILTERPARA_COFF_1          ' 1Hz
    LKIF_FILTERPARA_COFF_0_3        ' 0.3Hz
    LKIF_FILTERPARA_COFF_0_1        ' 0.1Hz
End Enum

' Set Number of Times for Averaging
Public Enum LKIF_AVERAGE
    LKIF_AVERAGE_1              ' 1 time
    LKIF_AVERAGE_4              ' 4 times
    LKIF_AVERAGE_16             ' 16 times
    LKIF_AVERAGE_64             ' 64 times
    LKIF_AVERAGE_256            ' 256 times
    LKIF_AVERAGE_1024           ' 1024 times
    LKIF_AVERAGE_4096           ' 4096 times
    LKIF_AVERAGE_16384          ' 16384 times
    LKIF_AVERAGE_65536          ' 65536 times
    LKIF_AVERAGE_262144         ' 262144 times
End Enum

' Set Cutoff Frequency
Public Enum LKIF_CUTOFFFREQUENCY
    LKIF_CUTOFFFREQUENCY_3000   ' 3000Hz
    LKIF_CUTOFFFREQUENCY_1000   ' 1000Hz
    LKIF_CUTOFFFREQUENCY_300    ' 300Hz
    LKIF_CUTOFFFREQUENCY_100    ' 100Hz
    LKIF_CUTOFFFREQUENCY_30     ' 30Hz
    LKIF_CUTOFFFREQUENCY_10     ' 10Hz
    LKIF_CUTOFFFREQUENCY_3      ' 3Hz
    LKIF_CUTOFFFREQUENCY_1      ' 1Hz
    LKIF_CUTOFFFREQUENCY_0_3    ' 0.3Hz
    LKIF_CUTOFFFREQUENCY_0_1    ' 0.1Hz
End Enum

' Set Trigger Mode
Public Enum LKIF_TRIGGERMODE
    LKIF_TRIGGERMODE_EXT1       ' external trigger 1
    LKIF_TRIGGERMODE_EXT2       ' external trigger 2
End Enum

' Set the Measurement Mode.
Public Enum LKIF_CALCMODE
    LKIF_CALCMODE_NORMAL            ' normal
    LKIF_CALCMODE_PEAKHOLD          ' peak hold
    LKIF_CALCMODE_BOTTOMHOLD        ' bottom hold
    LKIF_CALCMODE_PEAKTOPEAKHOLD    ' peak-to-peak hold
    LKIF_CALCMODE_SAMPLEHOLD        ' sample hold
    LKIF_CALCMODE_AVERAGEHOLD       ' average hold
End Enum

' Set Minimum Display Unit
Public Enum LKIF_DISPLAYUNIT
    LKIF_DISPLAYUNIT_0000_01MM = 0  ' 0.01mm
    LKIF_DISPLAYUNIT_000_001MM      ' 0.001mm
    LKIF_DISPLAYUNIT_00_0001MM      ' 0.0001mm
    LKIF_DISPLAYUNIT_0_00001MM      ' 0.00001mm
    LKIF_DISPLAYUNIT_00000_1UM      ' 0.1um
    LKIF_DISPLAYUNIT_0000_01UM      ' 0.01um
    LKIF_DISPLAYUNIT_000_001UM      ' 0.001um

    LKIF_DISPLAYUNIT_00000_1M_S = 0 ' 0.1m/s
    LKIF_DISPLAYUNIT_0000_01M_S     ' 0.01m/s
    LKIF_DISPLAYUNIT_000_001M_S     ' 0.001m/s
    LKIF_DISPLAYUNIT_00000_1MM_S    ' 0.1mm/s
    LKIF_DISPLAYUNIT_0000_01MM_S    ' 0.01mm/s
    LKIF_DISPLAYUNIT_000_001MM_S    ' 0.001mm/s
    LKIF_DISPLAYUNIT_00_0001MM_S    ' 0.0001mm/s

    LKIF_DISPLAYUNIT_00000_1KM_S2 = 0   ' 0.1km/s2
    LKIF_DISPLAYUNIT_0000_01KM_S2       ' 0.01km/s2
    LKIF_DISPLAYUNIT_000_001KM_S2       ' 0.001km/s2
    LKIF_DISPLAYUNIT_00000_1M_S2        ' 0.1m/s2
    LKIF_DISPLAYUNIT_0000_01M_S2        ' 0.01m/s2
    LKIF_DISPLAYUNIT_000_001M_S2        ' 0.001m/s2
    LKIF_DISPLAYUNIT_00_0001M_S2        ' 0.0001m/s2
End Enum

' Set measurement type.
Public Enum LKIF_MEASURETYPE
    LKIF_MEASURETYPE_DISPLACEMENT   ' Displacement
    LKIF_MEASURETYPE_SPEED          ' Speed
    LKIF_MEASURETYPE_ACCELERATION   ' Acceleration
End Enum

' Specify OUT
Public Enum LKIF_OUTNO
    LKIF_OUTNO_01 = &H1             ' OUT01
    LKIF_OUTNO_02 = &H2             ' OUT02
    LKIF_OUTNO_03 = &H4             ' OUT03
    LKIF_OUTNO_04 = &H8             ' OUT04
    LKIF_OUTNO_05 = &H10            ' OUT05
    LKIF_OUTNO_06 = &H20            ' OUT06
    LKIF_OUTNO_07 = &H40            ' OUT07
    LKIF_OUTNO_08 = &H80            ' OUT08
    LKIF_OUTNO_09 = &H100           ' OUT09
    LKIF_OUTNO_10 = &H200           ' OUT10
    LKIF_OUTNO_11 = &H400           ' OUT11
    LKIF_OUTNO_12 = &H800           ' OUT12
    LKIF_OUTNO_ALL = &HFFF          ' All OUTs
End Enum

' Set Synchronization  *Specify TRUE/FALSE for each OUT to set ON/OFF.
Public Enum LKIF_SYNCHRONIZATION
    LKIF_SYNCHRONIZATION_ASYNCHRONOUS   ' asynchronous
    LKIF_SYNCHRONIZATION_SYNCHRONIZED   ' synchronous
End Enum

' Set Storage (Target)  *Specify TRUE/FALSE for each OUT to set ON/OFF.
Public Enum LKIF_TARGETOUT
    LKIF_TARGETOUT_NONE         ' no target OUT
    LKIF_TARGETOUT_OUT1         ' OUT1
    LKIF_TARGETOUT_OUT2         ' OUT2
    LKIF_TARGETOUT_BOTH         ' OUT 1/2
End Enum

' Set Storage (Cycle)
Public Enum LKIF_STORAGECYCLE
    LKIF_STORAGECYCLE_1             ' sampling rate x 1
    LKIF_STORAGECYCLE_2             ' sampling rate x 2
    LKIF_STORAGECYCLE_5             ' sampling rate x 5
    LKIF_STORAGECYCLE_10            ' sampling rate x 10
    LKIF_STORAGECYCLE_20            ' sampling rate x 20
    LKIF_STORAGECYCLE_50            ' sampling rate x 50
    LKIF_STORAGECYCLE_100           ' sampling rate x 100
    LKIF_STORAGECYCLE_200           ' sampling rate x 200
    LKIF_STORAGECYCLE_500           ' sampling rate x 500
    LKIF_STORAGECYCLE_1000          ' sampling rate x 1000
    LKIF_STORAGECYCLE_TIMING        ' Timing sync
End Enum

' Set Sampling Rate
Public Enum LKIF_SAMPLINGCYCLE
    LKIF_SAMPLINGCYCLE_2_55USEC     ' 2.55us
    LKIF_SAMPLINGCYCLE_5USEC        ' 5us
    LKIF_SAMPLINGCYCLE_10USEC       ' 10us
    LKIF_SAMPLINGCYCLE_20USEC       ' 20us
    LKIF_SAMPLINGCYCLE_50USEC       ' 50us
    LKIF_SAMPLINGCYCLE_100USEC      ' 100us
    LKIF_SAMPLINGCYCLE_200USEC      ' 200us
    LKIF_SAMPLINGCYCLE_500USEC      ' 500us
    LKIF_SAMPLINGCYCLE_1MSEC        ' 1ms
End Enum

' Set Mutual interference prevention
Public Enum LKIF_MUTUAL_INTERFERENCE_PREVENTION
    LKIF_MUTUAL_INTERFERENCE_PREVENTION_OFF     ' OFF
    LKIF_MUTUAL_INTERFERENCE_PREVENTION_AB_ON   ' AB-ON
    LKIF_MUTUAL_INTERFERENCE_PREVENTION_ABC_ON  ' ABC-ON
End Enum

' Set Comparator Output Format
Public Enum LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT
    LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT_NORMAL      ' normal
    LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT_HOLD        ' hold
    LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT_OFF_DELAY   ' off-delay
End Enum

' Set Strobe Time
Public Enum LKIF_STOROBETIME
    LKIF_STOROBETIME_2MS        ' 2ms
    LKIF_STOROBETIME_5MS        ' 5ms
    LKIF_STOROBETIME_10MS       ' 10ms
    LKIF_STOROBETIME_20MS       ' 20ms
End Enum

' Set alarm output form.
Public Enum LKIF_ALARM_OUTPUT_FORM
    LKIF_ALARM_OUTPUT_FORM_SYSTEM   ' System alarm
    LKIF_ALARM_OUTPUT_FORM_MEASURE  ' Measurement alarm
    LKIF_ALARM_OUTPUT_FORM_BOTH     ' System alarm and measurement alarm
End Enum

' Mode Switch
Public Enum LKIF_MODE
    LKIF_MODE_NORMAL        ' normal mode
    LKIF_MODE_COMMUNICATION ' setting mode
End Enum

' Measurement value
Public Type LKIF_FLOATVALUE_OUT
    outNo As Long                   ' OUT No(0-11)
    FloatResult As LKIF_FLOATRESULT ' valid or invalid data
    value As Single                 ' Measurement value
End Type
Public Type LKIF_FLOATVALUE
    FloatResult As LKIF_FLOATRESULT ' valid or invalid data
    value As Single                 ' Measurement value
End Type

' IP address
Type IN_ADDR
    S_addr As Long
End Type
Type LKIF_OPENPARAM_ETHERNET
    IPAddress  As IN_ADDR
End Type




'''''''''''''''''''''''''''''''''''''''''''''''
' Measurement control commands
'
'Measured value output (single)
Public Declare Function LKIF2_GetCalcDataSingle Lib "LKIF2.dll" (ByVal outNo As Long, ByRef calcData As LKIF_FLOATVALUE_OUT) As RC

'Measured value output (multiple)
Public Declare Function LKIF2_GetCalcDataMulti Lib "LKIF2.dll" (ByVal outNo As LKIF_OUTNO, ByRef calcData As LKIF_FLOATVALUE_OUT) As RC

'Measured value output (ALL)
Public Declare Function LKIF2_GetCalcDataALL Lib "LKIF2.dll" (ByRef outNo As Long, ByRef calcData As LKIF_FLOATVALUE_OUT) As RC

'Timing ON/OFF (single)
Public Declare Function LKIF2_SetTimingSingle Lib "LKIF2.dll" (ByVal outNo As Long, ByVal onOff As Long) As RC

'Timing ON/OFF (multiple)
Public Declare Function LKIF2_SetTimingMulti Lib "LKIF2.dll" (ByVal outNo As LKIF_OUTNO, ByVal onOff As Long) As RC

'Timing ON/OFF (synchronous)
Public Declare Function LKIF2_SetTimingSync Lib "LKIF2.dll" (ByVal onOff As Long) As RC

'Auto-zero ON/OFF (single)
Public Declare Function LKIF2_SetZeroSingle Lib "LKIF2.dll" (ByVal outNo As Long, ByVal onOff As Long) As RC

'Auto-zero ON/OFF (multiple)
Public Declare Function LKIF2_SetZeroMulti Lib "LKIF2.dll" (ByVal outNo As LKIF_OUTNO, ByVal onOff As Long) As RC

'Auto-zero ON/OFF (synchronous)
Public Declare Function LKIF2_SetZeroSync Lib "LKIF2.dll" (ByVal onOff As Long) As RC

'Measured value reset (single)
Public Declare Function LKIF2_SetResetSingle Lib "LKIF2.dll" (ByVal outNo As Long) As RC

'Measured value reset (multiple)
Public Declare Function LKIF2_SetResetMulti Lib "LKIF2.dll" (ByVal outNo As LKIF_OUTNO) As RC

'Measured value reset (synchronous)
Public Declare Function LKIF2_SetResetSync Lib "LKIF2.dll" () As RC

'Panel lock
Public Declare Function LKIF2_SetPanelLock Lib "LKIF2.dll" (ByVal onOff As Long) As RC

'Program Change
Public Declare Function LKIF2_SetProgramNo Lib "LKIF2.dll" (ByVal programNo As Long) As RC

'Program Check
Public Declare Function LKIF2_GetProgramNo Lib "LKIF2.dll" (ByRef programNo As Long) As RC

'Starting the Data Storage
Public Declare Function LKIF2_DataStorageStart Lib "LKIF2.dll" () As RC

'Stopping the Data Storage
Public Declare Function LKIF2_DataStorageStop Lib "LKIF2.dll" () As RC

'Outputting the Data Storage
Public Declare Function LKIF2_DataStorageGetData Lib "LKIF2.dll" (ByVal outNo As Long, ByVal numOutBuffer As Long, ByRef OutBuffer As LKIF_FLOATVALUE, ByRef numReceived As Long) As RC

'Initializing the Data Storage
Public Declare Function LKIF2_DataStorageInit Lib "LKIF2.dll" () As RC

'Data Storage Accumulation Status Output
Public Declare Function LKIF2_DataStorageGetStatus Lib "LKIF2.dll" (ByRef isStorage As Long, ByRef numStorageData As Long) As RC

'Receive Light Waveform
Public Declare Function LKIF2_GetLight Lib "LKIF2.dll" (ByVal headNo As Long, ByVal peekNo As Long, ByRef measurePosition As Long, ByRef waveData As Byte) As RC

'''''''''''''''''''''''''''''''''''''''''''''''
'Setting change commands
'
'Display Panel Switch
Public Declare Function LKIF2_SetPanel Lib "LKIF2.dll" (ByVal upperDisp As Long, ByVal lowerDisp As Long) As RC

'Set Tolerance
Public Declare Function LKIF2_SetTolerance Lib "LKIF2.dll" (ByVal outNo As Long, ByVal upperLimit As Long, ByVal lowerLimit As Long, ByVal hysteresis As Long) As RC

'Set ABLE
Public Declare Function LKIF2_SetAbleMode Lib "LKIF2.dll" (ByVal headNo As Long, ByVal ableMode As LKIF_ABLEMODE) As RC
'Set ABLE Control Range
Public Declare Function LKIF2_SetAbleMinMax Lib "LKIF2.dll" (ByVal headNo As Long, ByVal min As Long, ByVal max As Long) As RC
'Set Detection mode
Public Declare Function LKIF2_SetMeasureMode Lib "LKIF2.dll" (ByVal headNo As Long, ByVal measureMode As LKIF_MEASUREMODE) As RC
'Set Base point
Public Declare Function LKIF2_SetBasicPoint Lib "LKIF2.dll" (ByVal headNo As Long, ByVal basicPoint As LKIF_BASICPOINT) As RC
'Set Number of Times of Alarm Processing
Public Declare Function LKIF2_SetNumAlarm Lib "LKIF2.dll" (ByVal headNo As Long, ByVal numAlarm As Long) As RC
'Set Number of Times of Alarm Recovery
Public Declare Function LKIF2_SetNumRecovery Lib "LKIF2.dll" (ByVal headNo As Long, ByVal recoveryNum As Long) As RC
'Set Alarm Level
Public Declare Function LKIF2_SetAlarmLevel Lib "LKIF2.dll" (ByVal headNo As Long, ByVal alaramLevel As Long) As RC
'Starting the ABLE Calibration
Public Declare Function LKIF2_AbleStart Lib "LKIF2.dll" (ByVal headNo As Long) As RC
'Finishing the ABLE Calibration
Public Declare Function LKIF2_AbleStop Lib "LKIF2.dll" () As RC
'Stopping the ABLE Calibration
Public Declare Function LKIF2_AbleCancel Lib "LKIF2.dll" () As RC
'Set Mounting Mode
Public Declare Function LKIF2_SetReflectionMode Lib "LKIF2.dll" (ByVal headNo As Long, ByVal reflectionMode As LKIF_REFLECTIONMODE) As RC
'Set Mask
Public Declare Function LKIF2_SetMask Lib "LKIF2.dll" (ByVal headNo As Long, ByVal onOff As Long, ByVal pos1 As Long, ByVal pos2 As Long) As RC
'Set Median
Public Declare Function LKIF2_SetMedian Lib "LKIF2.dll" (ByVal headNo As Long, ByVal median As LKIF_MEDIAN) As RC
'Set LASER CTRL group
Public Declare Function LKIF2_SetLaserCtrlGroup Lib "LKIF2.dll" (ByVal headNo As Long, ByVal laserCtrlGroup As LKIF_LASER_CTRL_GROUP) As RC
'Set Range
Public Declare Function LKIF2_SetRange Lib "LKIF2.dll" (ByVal headNo As Long, ByVal range As LKIF_RANGE) As RC
'Set Mutual interference prevention group
Public Declare Function LKIF2_SetMutualInterferencePreventionGroup Lib "LKIF2.dll" (ByVal headNo As Long, ByVal group As LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP) As RC

'Set Calculation method
Public Declare Function LKIF2_SetCalcMethod Lib "LKIF2.dll" (ByVal outNo As Long, ByVal calcMethod As LKIF_CALCMETHOD, ByVal head_Out_No As Long) As RC
'Set Surface to be measured
Public Declare Function LKIF2_SetCalcTarget Lib "LKIF2.dll" (ByVal outNo As Long, ByVal calcTarget As LKIF_CALCTARGET) As RC
'Set OUT to be calculated (ADD, SUB)
Public Declare Function LKIF2_SetOutAddSub Lib "LKIF2.dll" (ByVal outNo As Long, ByVal addSub1 As Long, ByVal addSub2 As Long) As RC
'Set OUT to be calculated (AVE, MAX, MIN, P-P)
Public Declare Function LKIF2_SetOutOperation Lib "LKIF2.dll" (ByVal outNo As Long, ByVal targetOut As LKIF_OUTNO) As RC
'Set Scaling
Public Declare Function LKIF2_SetScaling Lib "LKIF2.dll" (ByVal outNo As Long, ByVal inputValue1 As Long, ByVal outputValue1 As Long, ByVal inputValue2 As Long, ByVal outputValue2 As Long) As RC
'Set Filter Mode
Public Declare Function LKIF2_SetFilter Lib "LKIF2.dll" (ByVal outNo As Long, ByVal filterMode As LKIF_FILTERMODE, ByVal filterPara As LKIF_FILTERPARA) As RC
'Set Trigger Mode
Public Declare Function LKIF2_SetTriggerMode Lib "LKIF2.dll" (ByVal outNo As Long, ByVal triggerMode As LKIF_TRIGGERMODE) As RC
'Set Offset
Public Declare Function LKIF2_SetOffset Lib "LKIF2.dll" (ByVal outNo As Long, ByVal offset As Long) As RC
'Set Calculation Mode
Public Declare Function LKIF2_SetCalcMode Lib "LKIF2.dll" (ByVal outNo As Long, ByVal CalcMode As LKIF_CALCMODE) As RC
'Set Analog Output Scaling
Public Declare Function LKIF2_SetAnalogScaling Lib "LKIF2.dll" (ByVal outNo As Long, ByVal inputValue1 As Long, ByVal outputVoltage1 As Long, ByVal inputValue2 As Long, ByVal outputVoltage2 As Long) As RC
'Set Minimum Display Unit
Public Declare Function LKIF2_SetDisplayUnit Lib "LKIF2.dll" (ByVal outNo As Long, ByVal displayUnit As LKIF_DISPLAYUNIT) As RC
'Set Measurement type
Public Declare Function LKIF2_SetMeasureType Lib "LKIF2.dll" (ByVal outNo As Long, ByVal measureType As LKIF_MEASURETYPE) As RC
'Set Synchronization
Public Declare Function LKIF2_SetSynchronization Lib "LKIF2.dll" (ByVal outNo As Long, ByVal onOff As Long) As RC
'Set Storage (OUT No. specification)
Public Declare Function LKIF2_SetStorageTarget Lib "LKIF2.dll" (ByVal outNo As Long, ByVal onOff As Long) As RC

'Set Sampling Rate
Public Declare Function LKIF2_SetSamplingCycle Lib "LKIF2.dll" (ByVal samplingCycle As LKIF_SAMPLINGCYCLE) As RC
'Set Mutual Interference Prevention
Public Declare Function LKIF2_SetMutualInterferencePrevention Lib "LKIF2.dll" (ByVal onOff As LKIF_MUTUAL_INTERFERENCE_PREVENTION) As RC
'Set Comparator Output Format
Public Declare Function LKIF2_SetToleranceComparatorOutputFormat Lib "LKIF2.dll" (ByVal toleranceComparatorOutputFormat As LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT) As RC
'Set Strobe Time
Public Declare Function LKIF2_SetStrobeTime Lib "LKIF2.dll" (ByVal storobeTime As LKIF_STOROBETIME) As RC

'Set Data Storage
Public Declare Function LKIF2_SetDataStorage Lib "LKIF2.dll" (ByVal storageNum As Long, ByVal storageCycle As LKIF_STORAGECYCLE) As RC
'Set Analog output channel
Public Declare Function LKIF2_SetAnalogChannel Lib "LKIF2.dll" (ByVal chNo As Long, ByVal outNo As Long) As RC
'Set Alarm output type
Public Declare Function LKIF2_SetAlarmOutputForm Lib "LKIF2.dll" (ByVal alarmOutputForm As LKIF_ALARM_OUTPUT_FORM) As RC
'Set Active head count
Public Declare Function LKIF2_SetNumOfUsedHeads Lib "LKIF2.dll" (ByVal numOfUsedHeads As Long) As RC
'Set Active OUT count
Public Declare Function LKIF2_SetNumOfUsedOut Lib "LKIF2.dll" (ByVal numOfUsedOut As Long) As RC
'Set Active analog output channel count
Public Declare Function LKIF2_SetNumOfUsedAnalogCh Lib "LKIF2.dll" (ByVal numOfUsedAnalogCh As Long) As RC

'''''''''''''''''''''''''''''''''''''''''''''''
' Setting confirmation commands
'
'Display Panel Check
Public Declare Function LKIF2_GetPanel Lib "LKIF2.dll" (ByRef upperDisp As Long, ByRef lowerDisp As Long) As RC

'Get Tolerance
Public Declare Function LKIF2_GetTolerance Lib "LKIF2.dll" (ByVal outNo As Long, ByRef upperLimit As Long, ByRef lowerLimit As Long, ByRef hysteresis As Long) As RC

'Get ABLE
Public Declare Function LKIF2_GetAbleMode Lib "LKIF2.dll" (ByVal headNo As Long, ByRef ableMode As LKIF_ABLEMODE) As RC
'Get ABLE Control Range
Public Declare Function LKIF2_GetAbleMinMax Lib "LKIF2.dll" (ByVal headNo As Long, ByRef min As Long, ByRef max As Long) As RC
'Get Detection mode
Public Declare Function LKIF2_GetMeasureMode Lib "LKIF2.dll" (ByVal headNo As Long, ByRef measureMode As LKIF_MEASUREMODE) As RC
'Get Base point
Public Declare Function LKIF2_GetBasicPoint Lib "LKIF2.dll" (ByVal headNo As Long, ByRef basicPoint As LKIF_BASICPOINT) As RC
'Get Number of Times of Alarm Processing
Public Declare Function LKIF2_GetNumAlarm Lib "LKIF2.dll" (ByVal headNo As Long, ByRef numAlarm As Long) As RC
'Get Number of Times of Alarm Recovery
Public Declare Function LKIF2_GetNumRecovery Lib "LKIF2.dll" (ByVal headNo As Long, ByRef recoveryNum As Long) As RC
'Get Alarm Level
Public Declare Function LKIF2_GetAlarmLevel Lib "LKIF2.dll" (ByVal headNo As Long, ByRef alaramLevel As Long) As RC
'Get Mounting Mode
Public Declare Function LKIF2_GetReflectionMode Lib "LKIF2.dll" (ByVal headNo As Long, ByRef reflectionMode As LKIF_REFLECTIONMODE) As RC
'Get Mask
Public Declare Function LKIF2_GetMask Lib "LKIF2.dll" (ByVal headNo As Long, ByRef onOff As Long, ByRef pos1 As Long, ByRef pos2 As Long) As RC
'Get Median
Public Declare Function LKIF2_GetMedian Lib "LKIF2.dll" (ByVal headNo As Long, ByRef median As LKIF_MEDIAN) As RC
'Get LASER CTRL group
Public Declare Function LKIF2_GetLaserCtrlGroup Lib "LKIF2.dll" (ByVal headNo As Long, ByRef laserCtrlGroup As LKIF_LASER_CTRL_GROUP) As RC
'Get Range
Public Declare Function LKIF2_GetRange Lib "LKIF2.dll" (ByVal headNo As Long, ByRef range As LKIF_RANGE) As RC
'Get Mutual interference prevention group
Public Declare Function LKIF2_GetMutualInterferencePreventionGroup Lib "LKIF2.dll" (ByVal headNo As Long, ByRef group As LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP) As RC

'Get Calculation method
Public Declare Function LKIF2_GetCalcMethod Lib "LKIF2.dll" (ByVal outNo As Long, ByRef calcMethod As LKIF_CALCMETHOD, ByRef head_Out_No As Long) As RC
'Get Surface to be measured
Public Declare Function LKIF2_GetCalcTarget Lib "LKIF2.dll" (ByVal outNo As Long, ByRef calcTarget As LKIF_CALCTARGET) As RC
'Get OUT to be calculated (ADD, SUB)
Public Declare Function LKIF2_GetOutAddSub Lib "LKIF2.dll" (ByVal outNo As Long, ByRef addSub1 As Long, ByRef addSub2 As Long) As RC
'Get OUT to be calculated (AVE, MAX, MIN, P-P)
Public Declare Function LKIF2_GetOutOperation Lib "LKIF2.dll" (ByVal outNo As Long, ByRef targetOut As LKIF_OUTNO) As RC
'Get Scaling
Public Declare Function LKIF2_GetScaling Lib "LKIF2.dll" (ByVal outNo As Long, ByRef inputValue1 As Long, ByRef outputValue1 As Long, ByRef inputValue2 As Long, ByRef outputValue2 As Long) As RC
'Get Filter Mode
Public Declare Function LKIF2_GetFilter Lib "LKIF2.dll" (ByVal outNo As Long, ByRef filterMode As LKIF_FILTERMODE, ByRef filterPara As LKIF_FILTERPARA) As RC
'Get Trigger Mode
Public Declare Function LKIF2_GetTriggerMode Lib "LKIF2.dll" (ByVal outNo As Long, ByRef triggerMode As LKIF_TRIGGERMODE) As RC
'Get Offset
Public Declare Function LKIF2_GetOffset Lib "LKIF2.dll" (ByVal outNo As Long, ByRef offset As Long) As RC
'Get Calculation Mode
Public Declare Function LKIF2_GetCalcMode Lib "LKIF2.dll" (ByVal outNo As Long, ByRef CalcMode As LKIF_CALCMODE) As RC
'Get Analog Output Scaling
Public Declare Function LKIF2_GetAnalogScaling Lib "LKIF2.dll" (ByVal outNo As Long, ByRef inputValue1 As Long, ByRef outputVoltage1 As Long, ByRef inputValue1 As Long, ByRef outputVoltage2 As Long) As RC
'Get Minimum Display Unit
Public Declare Function LKIF2_GetDisplayUnit Lib "LKIF2.dll" (ByVal outNo As Long, ByRef displayUnit As LKIF_DISPLAYUNIT) As RC
'Get Measurement type
Public Declare Function LKIF2_GetMeasureType Lib "LKIF2.dll" (ByVal outNo As Long, ByRef measureType As LKIF_MEASURETYPE) As RC
'Get Synchronization
Public Declare Function LKIF2_GetSynchronization Lib "LKIF2.dll" (ByVal outNo As Long, ByRef onOff As Long) As RC
'Get Storage (OUT No. specification)
Public Declare Function LKIF2_GetStorageTarget Lib "LKIF2.dll" (ByVal outNo As Long, ByRef onOff As Long) As RC

'Get Sampling Rate
Public Declare Function LKIF2_GetSamplingCycle Lib "LKIF2.dll" (ByRef samplingCycle As LKIF_SAMPLINGCYCLE) As RC
'Get Mutual Interference Prevention
Public Declare Function LKIF2_GetMutualInterferencePrevention Lib "LKIF2.dll" (ByRef onOff As LKIF_MUTUAL_INTERFERENCE_PREVENTION) As RC
'Get Comparator Output Format
Public Declare Function LKIF2_GetToleranceComparatorOutputFormat Lib "LKIF2.dll" (ByRef toleranceComparatorOutputFormat As LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT) As RC
'Get Strobe Time
Public Declare Function LKIF2_GetStrobeTime Lib "LKIF2.dll" (ByRef storobeTime As LKIF_STOROBETIME) As RC
'Get Data Storage
Public Declare Function LKIF2_GetDataStorage Lib "LKIF2.dll" (ByRef storageNum As Long, ByRef storageCycle As LKIF_STORAGECYCLE) As RC
'Get Analog output channel
Public Declare Function LKIF2_GetAnalogChannel Lib "LKIF2.dll" (ByVal chNo As Long, ByRef outNo As Long) As RC
'Get Alarm output type
Public Declare Function LKIF2_GetAlarmOutputForm Lib "LKIF2.dll" (ByRef alarmOutputForm As LKIF_ALARM_OUTPUT_FORM) As RC
'Get Active head count
Public Declare Function LKIF2_GetNumOfUsedHeads Lib "LKIF2.dll" (ByRef numOfUsedHeads As Long) As RC
'Get Active OUT count
Public Declare Function LKIF2_GetNumOfUsedOut Lib "LKIF2.dll" (ByRef numOfUsedOut As Long) As RC
'Get Active analog output channel count
Public Declare Function LKIF2_GetNumOfUsedAnalogCh Lib "LKIF2.dll" (ByRef numOfUsedAnalogCh As Long) As RC

'''''''''''''''''''''''''''''''''''''''''''''''
' Mode change commands
'
'Start Measurement (Mode switch)
Public Declare Function LKIF2_StartMeasure Lib "LKIF2.dll" () As RC
'Stop Measurement (Mode switch)
Public Declare Function LKIF2_StopMeasure Lib "LKIF2.dll" () As RC

'''''''''''''''''''''''''''''''''''''''''''''''
' Other commands
'
'Opens the USB device.
Public Declare Function LKIF2_OpenDeviceUsb Lib "LKIF2.dll" () As RC
'Opens the Ethernet device.
Public Declare Function LKIF2_OpenDeviceETHER Lib "LKIF2.dll" (ByRef openParam As LKIF_OPENPARAM_ETHERNET) As RC
'Closes the currently open device.
Public Declare Function LKIF2_CloseDevice Lib "LKIF2.dll" () As RC

