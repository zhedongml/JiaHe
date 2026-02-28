using System;
using System.Runtime.InteropServices;

namespace Project1
{
    internal static class LKIF2
    {
        #region Structure

        // Measurement value
        [StructLayout(LayoutKind.Sequential)]
        public struct LKIF_FLOATVALUE_OUT
        {
            public int outNo; // OUT No(0-11)
            public LKIF_FLOATRESULT FloatResult; // valid or invalid data
            public float value; // Measurement value
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct LKIF_FLOATVALUE
        {
            public LKIF_FLOATRESULT FloatResult; // valid or invalid data
            public float value; // Measurement value
        };

        // IP address
        [StructLayout(LayoutKind.Sequential)]
        public struct IN_ADDR
        {
            public int S_addr;
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct LKIF_OPENPARAM_ETHERNET
        {
            public IN_ADDR IPAddress;
        };

        #endregion Structure

        #region enum

        // Return Code List
        internal enum RC
        {
            RC_OK = 0x0, // The operation is completed successfully.
            //''''''''''''''''''''''
            // Communication error from controller notification
            //
            RC_NAK_COMMAND = 0x1001, // Command error
            RC_NAK_COMMAND_LENGTH = 0x1002, // Command length error
            RC_NAK_TIMEOUT = 0x1003, // Timeout
            RC_NAK_CHECKSUM = 0x1004, // Check sum error
            RC_NAK_INVALID_STATE = 0x1005, // Status error
            RC_NAK_OTHER = 0x1006, // Other error
            RC_NAK_PARAMETER = 0x1007, // Parameter error
            RC_NAK_OUT_STAGE = 0x1008, // OUT calculation count limitation error
            RC_NAK_OUT_HEAD_NUM = 0x1009, // No. of used head/OUT over error
            RC_NAK_OUT_INVALID_CALC = 0x100A, // OUT which cannot be used for calculation was specified for calculation.
            RC_NAK_OUT_VOID = 0x100B, // OUT which specified for calculation is not found.
            RC_NAK_INVALID_CYCLE = 0x100C, // Unavailable sampling cycle
            RC_NAK_CTRL_ERROR = 0x100D, // Main unit error
            RC_NAK_SRAM_ERROR = 0x100E, // Setting value error
            //''''''''''''''''''''''
            // Communication DLL error notification
            //
            RC_ERR_OPEN_DEVICE = 0x2000, // Opening the device failed.
            RC_ERR_NO_DEVICE = 0x2001, // The device is not open.
            RC_ERR_SEND = 0x2002, // Command sending error
            RC_ERR_RECEIVE = 0x2003, // Response receiving error
            RC_ERR_TIMEOUT = 0x2004, // Timeout
            RC_ERR_NODATA = 0x2005, // No data
            RC_ERR_NOMEMORY = 0x2006, // No free memory
            RC_ERR_DISCONNECT = 0x2007, // Cable disconnection suspected
            RC_ERR_UNKNOWN = 0x2008 // Undefined error
        }

        // Measurement value structures
        internal enum LKIF_FLOATRESULT
        {
            LKIF_FLOATRESULT_VALID, // valid data
            LKIF_FLOATRESULT_RANGEOVER_P, // over range at positive (+) side
            LKIF_FLOATRESULT_RANGEOVER_N, // over range at negative (-) side
            LKIF_FLOATRESULT_WAITING, // Wait for comparator result
            LKIF_FLOATRESULT_ALARM, // alarm
            LKIF_FLOATRESULT_INVALID // Invalid (error, etc.)
        }

        // Set ABLE
        internal enum LKIF_ABLEMODE
        {
            LKIF_ABLEMODE_AUTO, // AUTO
            LKIF_ABLEMODE_MANUAL // manual
        }

        // Set Measurement Mode
        internal enum LKIF_MEASUREMODE
        {
            LKIF_MEASUREMODE_NORMAL, // normal
            LKIF_MEASUREMODE_HALF_T, // translucent object
            LKIF_MEASUREMDOE_TRAN_1, // transparent object
            LKIF_MEASUREMODE_TRAN_2, // transparent object 2
            LKIF_MEASUREMODE_MRS, // multireflective object
            LKIF_MEASUREMODE_OPAQUE = (int)LKIF_MEASUREMODE.LKIF_MEASUREMODE_MRS // Semi opaque object
        }

        // Set Basic Point
        internal enum LKIF_BASICPOINT
        {
            LKIF_BASICPOINT_NEAR, // NEAR
            LKIF_BASICPOINT_FAR // FAR
        }

        // Set Mounting Mode
        internal enum LKIF_REFLECTIONMODE
        {
            LKIF_REFLECTIONMODE_DIFFUSION, // diffuse reflection
            LKIF_REFLECTIONMODE_MIRROR // specular reflection
        }

        // Set Median
        internal enum LKIF_MEDIAN
        {
            LKIF_MEDIAN_OFF, // OFF
            LKIF_MEDIAN_7, // 7 point
            LKIF_MEDIAN_15, // 15 point
            LKIF_MEDIAN_31 // 31 point
        }

        // Set LASER CTRL group.
        internal enum LKIF_LASER_CTRL_GROUP
        {
            LKIF_LASER_CTRL_GROUP_1, // LASER CTRL 1
            LKIF_LASER_CTRL_GROUP_2 // LASER CTRL 2
        }

        // Set a range.
        internal enum LKIF_RANGE
        {
            LKIF_RANGE_CENTER, // CENTER
            LKIF_RANGE_FAR // FAR
        }

        // Set Mutual Interference Prevention Group
        internal enum LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP
        {
            LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP_A, // Group A
            LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP_B, // Group B
            LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP_C // Group C
        }

        // Set calculation method.
        internal enum LKIF_CALCMETHOD
        {
            LKIF_CALCMETHOD_HEADA, // head A
            LKIF_CALCMETHOD_HEADB, // head B
            LKIF_CALCMETHOD_HEAD_HEADA_PLUS_HEADB, // head A+head B
            LKIF_CALCMETHOD_HEAD_HEADA_MINUS_HEADB, // head A-head B
            LKIF_CALCMETHOD_HEAD_HEADA_TRANSPARENT, // head A transparent object
            LKIF_CALCMETHOD_HEAD_HEADB_TRANSPARENT, // head B transparent object
            LKIF_CALCMETHOD_HEAD = 0, // head
            LKIF_CALCMETHOD_OUT, // OUT
            LKIF_CALCMETHOD_ADD, // ADD
            LKIF_CALCMETHOD_SUB, // SUB
            LKIF_CALCMETHOD_AVE, // AVE
            LKIF_CALCMETHOD_PP, // P-P
            LKIF_CALCMETHOD_MAX, // MAX
            LKIF_CALCMETHOD_MIN // MIN
        }

        // Set Target Surface
        internal enum LKIF_CALCTARGET
        {
            LKIF_CALCTARGET_PEAK_1, // peak 1
            LKIF_CALCTARGET_PEAK_2, // peak 2
            LKIF_CALCTARGET_PEAK_3, // peak 3
            LKIF_CALCTARGET_PEAK_4, // peak 4
            LKIF_CALCTARGET_PEAK_1_2, // peak 1-peak 2
            LKIF_CALCTARGET_PEAK_1_3, // peak 1-peak 3
            LKIF_CALCTARGET_PEAK_1_4, // peak 1-peak 4
            LKIF_CALCTARGET_PEAK_2_3, // peak 2-peak 3
            LKIF_CALCTARGET_PEAK_2_4, // peak 2-peak 4
            LKIF_CALCTARGET_PEAK_3_4 // peak 3-peak 4
        }

        // Set Filter Mode
        internal enum LKIF_FILTERMODE
        {
            LKIF_FILTERMODE_MOVING_AVERAGE, // moving average
            LKIF_FILTERMODE_LOWPASS, // low pass filter
            LKIF_FILTERMODE_HIGHPASS // high pass filter
        }

        // Specify the filter parameter. (The number of averaging for moving average, or the cutoff frequency for LPF/HPF.)
        internal enum LKIF_FILTERPARA
        {
            LKIF_FILTERPARA_AVE_1 = 0, // 1 time
            LKIF_FILTERPARA_AVE_4, // 4 times
            LKIF_FILTERPARA_AVE_16, // 16 times
            LKIF_FILTERPARA_AVE_64, // 64 times
            LKIF_FILTERPARA_AVE_256, // 256 times
            LKIF_FILTERPARA_AVE_1024, // 1024 times
            LKIF_FILTERPARA_AVE_4096, // 4096 times
            LKIF_FILTERPARA_AVE_16384, // 16384 times
            LKIF_FILTERPARA_AVE_65536, // 65536 times
            LKIF_FILTERPARA_AVE_262144, // 262144 times
            LKIF_FILTERPARA_COFF_3000 = 0, // 3000Hz
            LKIF_FILTERPARA_COFF_1000, // 1000Hz
            LKIF_FILTERPARA_COFF_300, // 300Hz
            LKIF_FILTERPARA_COFF_100, // 100Hz
            LKIF_FILTERPARA_COFF_30, // 30Hz
            LKIF_FILTERPARA_COFF_10, // 10Hz
            LKIF_FILTERPARA_COFF_3, // 3Hz
            LKIF_FILTERPARA_COFF_1, // 1Hz
            LKIF_FILTERPARA_COFF_0_3, // 0.3Hz
            LKIF_FILTERPARA_COFF_0_1 // 0.1Hz
        }

        // Set Number of Times for Averaging
        internal enum LKIF_AVERAGE
        {
            LKIF_AVERAGE_1, // 1 time
            LKIF_AVERAGE_4, // 4 times
            LKIF_AVERAGE_16, // 16 times
            LKIF_AVERAGE_64, // 64 times
            LKIF_AVERAGE_256, // 256 times
            LKIF_AVERAGE_1024, // 1024 times
            LKIF_AVERAGE_4096, // 4096 times
            LKIF_AVERAGE_16384, // 16384 times
            LKIF_AVERAGE_65536, // 65536 times
            LKIF_AVERAGE_262144 // 262144 times
        }

        // Set Cutoff Frequency
        internal enum LKIF_CUTOFFFREQUENCY
        {
            LKIF_CUTOFFFREQUENCY_3000, // 3000Hz
            LKIF_CUTOFFFREQUENCY_1000, // 1000Hz
            LKIF_CUTOFFFREQUENCY_300, // 300Hz
            LKIF_CUTOFFFREQUENCY_100, // 100Hz
            LKIF_CUTOFFFREQUENCY_30, // 30Hz
            LKIF_CUTOFFFREQUENCY_10, // 10Hz
            LKIF_CUTOFFFREQUENCY_3, // 3Hz
            LKIF_CUTOFFFREQUENCY_1, // 1Hz
            LKIF_CUTOFFFREQUENCY_0_3, // 0.3Hz
            LKIF_CUTOFFFREQUENCY_0_1 // 0.1Hz
        }

        // Set Trigger Mode
        internal enum LKIF_TRIGGERMODE
        {
            LKIF_TRIGGERMODE_EXT1, // external trigger 1
            LKIF_TRIGGERMODE_EXT2 // external trigger 2
        }

        // Set the Measurement Mode.
        internal enum LKIF_CALCMODE
        {
            LKIF_CALCMODE_NORMAL, // normal
            LKIF_CALCMODE_PEAKHOLD, // peak hold
            LKIF_CALCMODE_BOTTOMHOLD, // bottom hold
            LKIF_CALCMODE_PEAKTOPEAKHOLD, // peak-to-peak hold
            LKIF_CALCMODE_SAMPLEHOLD, // sample hold
            LKIF_CALCMODE_AVERAGEHOLD // average hold
        }

        // Set Minimum Display Unit
        internal enum LKIF_DISPLAYUNIT
        {
            LKIF_DISPLAYUNIT_0000_01MM = 0, // 0.01mm
            LKIF_DISPLAYUNIT_000_001MM, // 0.001mm
            LKIF_DISPLAYUNIT_00_0001MM, // 0.0001mm
            LKIF_DISPLAYUNIT_0_00001MM, // 0.00001mm
            LKIF_DISPLAYUNIT_00000_1UM, // 0.1um
            LKIF_DISPLAYUNIT_0000_01UM, // 0.01um
            LKIF_DISPLAYUNIT_000_001UM, // 0.001um
            LKIF_DISPLAYUNIT_00000_1M_S = 0, // 0.1m/s
            LKIF_DISPLAYUNIT_0000_01M_S, // 0.01m/s
            LKIF_DISPLAYUNIT_000_001M_S, // 0.001m/s
            LKIF_DISPLAYUNIT_00000_1MM_S, // 0.1mm/s
            LKIF_DISPLAYUNIT_0000_01MM_S, // 0.01mm/s
            LKIF_DISPLAYUNIT_000_001MM_S, // 0.001mm/s
            LKIF_DISPLAYUNIT_00_0001MM_S, // 0.0001mm/s
            LKIF_DISPLAYUNIT_00000_1KM_S2 = 0, // 0.1km/s2
            LKIF_DISPLAYUNIT_0000_01KM_S2, // 0.01km/s2
            LKIF_DISPLAYUNIT_000_001KM_S2, // 0.001km/s2
            LKIF_DISPLAYUNIT_00000_1M_S2, // 0.1m/s2
            LKIF_DISPLAYUNIT_0000_01M_S2, // 0.01m/s2
            LKIF_DISPLAYUNIT_000_001M_S2, // 0.001m/s2
            LKIF_DISPLAYUNIT_00_0001M_S2 // 0.0001m/s2
        }

        // Set measurement type.
        internal enum LKIF_MEASURETYPE
        {
            LKIF_MEASURETYPE_DISPLACEMENT, // Displacement
            LKIF_MEASURETYPE_SPEED, // Speed
            LKIF_MEASURETYPE_ACCELERATION // Acceleration
        }

        // Specify OUT
        [Flags]
        internal enum LKIF_OUTNO
        {
            LKIF_OUTNO_01 = 0x1, // OUT01
            LKIF_OUTNO_02 = 0x2, // OUT02
            LKIF_OUTNO_03 = 0x4, // OUT03
            LKIF_OUTNO_04 = 0x8, // OUT04
            LKIF_OUTNO_05 = 0x10, // OUT05
            LKIF_OUTNO_06 = 0x20, // OUT06
            LKIF_OUTNO_07 = 0x40, // OUT07
            LKIF_OUTNO_08 = 0x80, // OUT08
            LKIF_OUTNO_09 = 0x100, // OUT09
            LKIF_OUTNO_10 = 0x200, // OUT10
            LKIF_OUTNO_11 = 0x400, // OUT11
            LKIF_OUTNO_12 = 0x800, // OUT12
            LKIF_OUTNO_ALL = 0xFFF // All OUTs
        }

        // Set Synchronization  *Specify TRUE/FALSE for each OUT to set ON/OFF.
        internal enum LKIF_SYNCHRONIZATION
        {
            LKIF_SYNCHRONIZATION_ASYNCHRONOUS, // asynchronous
            LKIF_SYNCHRONIZATION_SYNCHRONIZED // synchronous
        }

        // Set Storage (Target)  *Specify TRUE/FALSE for each OUT to set ON/OFF.
        internal enum LKIF_TARGETOUT
        {
            LKIF_TARGETOUT_NONE, // no target OUT
            LKIF_TARGETOUT_OUT1, // OUT1
            LKIF_TARGETOUT_OUT2, // OUT2
            LKIF_TARGETOUT_BOTH // OUT 1/2
        }

        // Set Storage (Cycle)
        internal enum LKIF_STORAGECYCLE
        {
            LKIF_STORAGECYCLE_1, // sampling rate x 1
            LKIF_STORAGECYCLE_2, // sampling rate x 2
            LKIF_STORAGECYCLE_5, // sampling rate x 5
            LKIF_STORAGECYCLE_10, // sampling rate x 10
            LKIF_STORAGECYCLE_20, // sampling rate x 20
            LKIF_STORAGECYCLE_50, // sampling rate x 50
            LKIF_STORAGECYCLE_100, // sampling rate x 100
            LKIF_STORAGECYCLE_200, // sampling rate x 200
            LKIF_STORAGECYCLE_500, // sampling rate x 500
            LKIF_STORAGECYCLE_1000, // sampling rate x 1000
            LKIF_STORAGECYCLE_TIMING // Timing sync
        }

        // Set Sampling Rate
        internal enum LKIF_SAMPLINGCYCLE
        {
            LKIF_SAMPLINGCYCLE_2_55USEC, // 2.55us
            LKIF_SAMPLINGCYCLE_5USEC, // 5us
            LKIF_SAMPLINGCYCLE_10USEC, // 10us
            LKIF_SAMPLINGCYCLE_20USEC, // 20us
            LKIF_SAMPLINGCYCLE_50USEC, // 50us
            LKIF_SAMPLINGCYCLE_100USEC, // 100us
            LKIF_SAMPLINGCYCLE_200USEC, // 200us
            LKIF_SAMPLINGCYCLE_500USEC, // 500us
            LKIF_SAMPLINGCYCLE_1MSEC // 1ms
        }

        // Set Mutual interference prevention
        internal enum LKIF_MUTUAL_INTERFERENCE_PREVENTION
        {
            LKIF_MUTUAL_INTERFERENCE_PREVENTION_OFF, // OFF
            LKIF_MUTUAL_INTERFERENCE_PREVENTION_AB_ON, // AB-ON
            LKIF_MUTUAL_INTERFERENCE_PREVENTION_ABC_ON // ABC-ON
        }

        // Set Comparator Output Format
        internal enum LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT
        {
            LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT_NORMAL, // normal
            LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT_HOLD, // hold
            LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT_OFF_DELAY // off-delay
        }

        // Set Strobe Time
        internal enum LKIF_STOROBETIME
        {
            LKIF_STOROBETIME_2MS, // 2ms
            LKIF_STOROBETIME_5MS, // 5ms
            LKIF_STOROBETIME_10MS, // 10ms
            LKIF_STOROBETIME_20MS // 20ms
        }

        // Set alarm output form.
        internal enum LKIF_ALARM_OUTPUT_FORM
        {
            LKIF_ALARM_OUTPUT_FORM_SYSTEM, // System alarm
            LKIF_ALARM_OUTPUT_FORM_MEASURE, // Measurement alarm
            LKIF_ALARM_OUTPUT_FORM_BOTH // System alarm and measurement alarm
        }

        // Mode Switch
        internal enum LKIF_MODE
        {
            LKIF_MODE_NORMAL, // normal mode
            LKIF_MODE_COMMUNICATION // setting mode
        }

        #endregion enum

        #region DLL Methods

        //''''''''''''''''''''''''''''''''''''''''''''''
        // Measurement control commands
        //
        //Measured value output (single)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetCalcDataSingle(int outNo, ref LKIF_FLOATVALUE_OUT calcData);

        //Measured value output (multiple)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetCalcDataMulti(LKIF_OUTNO outNo, ref LKIF_FLOATVALUE_OUT calcData);

        //Measured value output (ALL)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetCalcDataALL(ref int outNo, ref LKIF_FLOATVALUE_OUT calcData);

        //Timing ON/OFF (single)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetTimingSingle(int outNo, int onOff);

        //Timing ON/OFF (multiple)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetTimingMulti(LKIF_OUTNO outNo, int onOff);

        //Timing ON/OFF (synchronous)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetTimingSync(int onOff);

        //Auto-zero ON/OFF (single)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetZeroSingle(int outNo, int onOff);

        //Auto-zero ON/OFF (multiple)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetZeroMulti(LKIF_OUTNO outNo, int onOff);

        //Auto-zero ON/OFF (synchronous)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetZeroSync(int onOff);

        //Measured value reset (single)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetResetSingle(int outNo);

        //Measured value reset (multiple)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetResetMulti(LKIF_OUTNO outNo);

        //Measured value reset (synchronous)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetResetSync();

        //Panel lock
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetPanelLock(int onOff);

        //Program Change
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetProgramNo(int programNo);

        //Program Check
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetProgramNo(ref int programNo);

        //Starting the Data Storage
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_DataStorageStart();

        //Stopping the Data Storage
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_DataStorageStop();

        //Outputting the Data Storage
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_DataStorageGetData(int outNo, int numOutBuffer, ref LKIF_FLOATVALUE OutBuffer, ref int numReceived);

        //Initializing the Data Storage
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_DataStorageInit();

        //Data Storage Accumulation Status Output
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_DataStorageGetStatus(ref int isStorage, ref int numStorageData);

        //Receive Light Waveform
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetLight(int headNo, int peekNo, ref int measurePosition, ref byte waveData);

        //''''''''''''''''''''''''''''''''''''''''''''''
        //Setting change commands
        //
        //Display Panel Switch
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetPanel(int upperDisp, int lowerDisp);

        //Set Tolerance
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetTolerance(int outNo, int upperLimit, int lowerLimit, int hysteresis);

        //Set ABLE
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetAbleMode(int headNo, LKIF_ABLEMODE ableMode);
        //Set ABLE Control Range
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetAbleMinMax(int headNo, int min, int max);
        //Set Detection mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetMeasureMode(int headNo, LKIF_MEASUREMODE measureMode);
        //Set Base point
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetBasicPoint(int headNo, LKIF_BASICPOINT basicPoint);
        //Set Number of Times of Alarm Processing
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetNumAlarm(int headNo, int numAlarm);
        //Set Number of Times of Alarm Recovery
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetNumRecovery(int headNo, int recoveryNum);
        //Set Alarm Level
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetAlarmLevel(int headNo, int alaramLevel);
        //Starting the ABLE Calibration
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_AbleStart(int headNo);
        //Finishing the ABLE Calibration
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_AbleStop();
        //Stopping the ABLE Calibration
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_AbleCancel();
        //Set Mounting Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetReflectionMode(int headNo, LKIF_REFLECTIONMODE reflectionMode);
        //Set Mask
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetMask(int headNo, int onOff, int pos1, int pos2);
        //Set Median
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetMedian(int headNo, LKIF_MEDIAN median);
        //Set LASER CTRL group
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetLaserCtrlGroup(int headNo, LKIF_LASER_CTRL_GROUP laserCtrlGroup);
        //Set Range
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetRange(int headNo, LKIF_RANGE range);
        //Set Mutual interference prevention group
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetMutualInterferencePreventionGroup(int headNo, LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP group);

        //Set Calculation method
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetCalcMethod(int outNo, LKIF_CALCMETHOD calcMethod, int head_Out_No);
        //Set Surface to be measured
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetCalcTarget(int outNo, LKIF_CALCTARGET calcTarget);
        //Set OUT to be calculated (ADD, SUB)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetOutAddSub(int outNo, int addSub1, int addSub2);
        //Set OUT to be calculated (AVE, MAX, MIN, P-P)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetOutOperation(int outNo, LKIF_OUTNO targetOut);
        //Set Scaling
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetScaling(int outNo, int inputValue1, int outputValue1, int inputValue2, int outputValue2);
        //Set Filter Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetFilter(int outNo, LKIF_FILTERMODE filterMode, LKIF_FILTERPARA filterPara);
        //Set Trigger Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetTriggerMode(int outNo, LKIF_TRIGGERMODE triggerMode);
        //Set Offset
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetOffset(int outNo, int offset);
        //Set Calculation Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetCalcMode(int outNo, LKIF_CALCMODE CalcMode);
        //Set Analog Output Scaling
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetAnalogScaling(int outNo, int inputValue1, int outputVoltage1, int inputValue2, int outputVoltage2);
        //Set Minimum Display Unit
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetDisplayUnit(int outNo, LKIF_DISPLAYUNIT displayUnit);
        //Set Measurement type
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetMeasureType(int outNo, LKIF_MEASURETYPE measureType);
        //Set Synchronization
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetSynchronization(int outNo, int onOff);
        //Set Storage (OUT No. specification)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetStorageTarget(int outNo, int onOff);

        //Set Sampling Rate
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetSamplingCycle(LKIF_SAMPLINGCYCLE samplingCycle);
        //Set Mutual Interference Prevention
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetMutualInterferencePrevention(LKIF_MUTUAL_INTERFERENCE_PREVENTION onOff);
        //Set Comparator Output Format
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetToleranceComparatorOutputFormat(LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT toleranceComparatorOutputFormat);
        //Set Strobe Time
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetStrobeTime(LKIF_STOROBETIME storobeTime);

        //Set Data Storage
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetDataStorage(int storageNum, LKIF_STORAGECYCLE storageCycle);
        //Set Analog output channel
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetAnalogChannel(int chNo, int outNo);
        //Set Alarm output type
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetAlarmOutputForm(LKIF_ALARM_OUTPUT_FORM alarmOutputForm);
        //Set Active head count
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetNumOfUsedHeads(int numOfUsedHeads);
        //Set Active OUT count
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetNumOfUsedOut(int numOfUsedOut);
        //Set Active analog output channel count
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_SetNumOfUsedAnalogCh(int numOfUsedAnalogCh);

        //''''''''''''''''''''''''''''''''''''''''''''''
        // Setting confirmation commands
        //
        //Display Panel Check
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetPanel(ref int upperDisp, ref int lowerDisp);

        //Get Tolerance
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetTolerance(int outNo, ref int upperLimit, ref int lowerLimit, ref int hysteresis);

        //Get ABLE
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetAbleMode(int headNo, ref LKIF_ABLEMODE ableMode);
        //Get ABLE Control Range
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetAbleMinMax(int headNo, ref int min, ref int max);
        //Get Detection mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetMeasureMode(int headNo, ref LKIF_MEASUREMODE measureMode);
        //Get Base point
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetBasicPoint(int headNo, ref LKIF_BASICPOINT basicPoint);
        //Get Number of Times of Alarm Processing
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetNumAlarm(int headNo, ref int numAlarm);
        //Get Number of Times of Alarm Recovery
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetNumRecovery(int headNo, ref int recoveryNum);
        //Get Alarm Level
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetAlarmLevel(int headNo, ref int alaramLevel);
        //Get Mounting Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetReflectionMode(int headNo, ref LKIF_REFLECTIONMODE reflectionMode);
        //Get Mask
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetMask(int headNo, ref int onOff, ref int pos1, ref int pos2);
        //Get Median
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetMedian(int headNo, ref LKIF_MEDIAN median);
        //Get LASER CTRL group
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetLaserCtrlGroup(int headNo, ref LKIF_LASER_CTRL_GROUP laserCtrlGroup);
        //Get Range
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetRange(int headNo, ref LKIF_RANGE range);
        //Get Mutual interference prevention group
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetMutualInterferencePreventionGroup(int headNo, ref LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP group);

        //Get Calculation method
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetCalcMethod(int outNo, ref LKIF_CALCMETHOD calcMethod, ref int head_Out_No);
        //Get Surface to be measured
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetCalcTarget(int outNo, ref LKIF_CALCTARGET calcTarget);
        //Get OUT to be calculated (ADD, SUB)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetOutAddSub(int outNo, ref int addSub1, ref int addSub2);
        //Get OUT to be calculated (AVE, MAX, MIN, P-P)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetOutOperation(int outNo, ref LKIF_OUTNO targetOut);
        //Get Scaling
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetScaling(int outNo, ref int inputValue1, ref int outputValue1, ref int inputValue2, ref int outputValue2);
        //Get Filter Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetFilter(int outNo, ref LKIF_FILTERMODE filterMode, ref LKIF_FILTERPARA filterPara);
        //Get Trigger Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetTriggerMode(int outNo, ref LKIF_TRIGGERMODE triggerMode);
        //Get Offset
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetOffset(int outNo, ref int offset);
        //Get Calculation Mode
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetCalcMode(int outNo, ref LKIF_CALCMODE CalcMode);
        //Get Analog Output Scaling
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetAnalogScaling(int outNo, ref int inputValue1, ref int outputVoltage1, ref int inputValue1_Renamed, ref int outputVoltage2);
        //Get Minimum Display Unit
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetDisplayUnit(int outNo, ref LKIF_DISPLAYUNIT displayUnit);
        //Get Measurement type
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetMeasureType(int outNo, ref LKIF_MEASURETYPE measureType);
        //Get Synchronization
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetSynchronization(int outNo, ref int onOff);
        //Get Storage (OUT No. specification)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetStorageTarget(int outNo, ref int onOff);

        //Get Sampling Rate
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetSamplingCycle(ref LKIF_SAMPLINGCYCLE samplingCycle);
        //Get Mutual Interference Prevention
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetMutualInterferencePrevention(ref LKIF_MUTUAL_INTERFERENCE_PREVENTION onOff);
        //Get Comparator Output Format
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetToleranceComparatorOutputFormat(ref LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT toleranceComparatorOutputFormat);
        //Get Strobe Time
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetStrobeTime(ref LKIF_STOROBETIME storobeTime);
        //Get Data Storage
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetDataStorage(ref int storageNum, ref LKIF_STORAGECYCLE storageCycle);
        //Get Analog output channel
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetAnalogChannel(int chNo, ref int outNo);
        //Get Alarm output type
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetAlarmOutputForm(ref LKIF_ALARM_OUTPUT_FORM alarmOutputForm);
        //Get Active head count
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetNumOfUsedHeads(ref int numOfUsedHeads);
        //Get Active OUT count
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetNumOfUsedOut(ref int numOfUsedOut);
        //Get Active analog output channel count
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_GetNumOfUsedAnalogCh(ref int numOfUsedAnalogCh);

        //''''''''''''''''''''''''''''''''''''''''''''''
        // Mode change commands
        //
        //Start Measurement (Mode switch)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_StartMeasure();
        //Stop Measurement (Mode switch)
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_StopMeasure();

        //''''''''''''''''''''''''''''''''''''''''''''''
        // Other commands
        //
        //Opens the USB device.
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_OpenDeviceUsb();
        //Opens the Ethernet device.
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_OpenDeviceETHER(ref LKIF_OPENPARAM_ETHERNET openParam);
        //Closes the currently open device.
        [DllImport("LKIF2.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        extern public static RC LKIF2_CloseDevice();

        #endregion DLL Methods
    }
}