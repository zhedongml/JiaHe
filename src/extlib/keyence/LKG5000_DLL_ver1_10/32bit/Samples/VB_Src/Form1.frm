VERSION 5.00
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "TABCTL32.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form Form1 
   BorderStyle     =   1  '固定(実線)
   Caption         =   "Form1"
   ClientHeight    =   12675
   ClientLeft      =   150
   ClientTop       =   540
   ClientWidth     =   14025
   ClipControls    =   0   'False
   HasDC           =   0   'False
   MaxButton       =   0   'False
   MinButton       =   0   'False
   NegotiateMenus  =   0   'False
   ScaleHeight     =   12675
   ScaleWidth      =   14025
   Begin VB.CommandButton CloseButton 
      Caption         =   "Close"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   12360
      TabIndex        =   384
      Top             =   12000
      Width           =   1575
   End
   Begin VB.CheckBox IsDisplayMessageBox 
      Caption         =   "When the error occurs, the message is displayed. "
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   240
      TabIndex        =   383
      Top             =   12240
      Value           =   1  'ﾁｪｯｸ
      Width           =   4935
   End
   Begin TabDlg.SSTab TabControl 
      CausesValidation=   0   'False
      Height          =   11640
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   13815
      _ExtentX        =   24368
      _ExtentY        =   20532
      _Version        =   393216
      Tabs            =   7
      Tab             =   1
      TabsPerRow      =   4
      TabHeight       =   520
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Tahoma"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      TabCaption(0)   =   "Measurement control commands"
      TabPicture(0)   =   "Form1.frx":0000
      Tab(0).ControlEnabled=   0   'False
      Tab(0).Control(0)=   "GetCalcData(1)"
      Tab(0).Control(0).Enabled=   0   'False
      Tab(0).Control(1)=   "GetLightParam2"
      Tab(0).Control(1).Enabled=   0   'False
      Tab(0).Control(2)=   "DataStorageGetDataParam2"
      Tab(0).Control(2).Enabled=   0   'False
      Tab(0).Control(3)=   "CommonDialogForDataStorage"
      Tab(0).Control(3).Enabled=   0   'False
      Tab(0).Control(4)=   "SyncParam"
      Tab(0).Control(4).Enabled=   0   'False
      Tab(0).Control(5)=   "MultiParam2"
      Tab(0).Control(5).Enabled=   0   'False
      Tab(0).Control(6)=   "SingleParam(1)"
      Tab(0).Control(6).Enabled=   0   'False
      Tab(0).Control(7)=   "SetProgramNoParam1"
      Tab(0).Control(7).Enabled=   0   'False
      Tab(0).Control(8)=   "SetPanelLockParam1"
      Tab(0).Control(8).Enabled=   0   'False
      Tab(0).Control(9)=   "checkSaveGetLight"
      Tab(0).Control(9).Enabled=   0   'False
      Tab(0).Control(10)=   "checkSaveDataStorageGetData"
      Tab(0).Control(10).Enabled=   0   'False
      Tab(0).Control(11)=   "GetLightParam1"
      Tab(0).Control(11).Enabled=   0   'False
      Tab(0).Control(12)=   "SingleParam(0)"
      Tab(0).Control(12).Enabled=   0   'False
      Tab(0).Control(13)=   "SetReset(0)"
      Tab(0).Control(13).Enabled=   0   'False
      Tab(0).Control(14)=   "SetZero(0)"
      Tab(0).Control(14).Enabled=   0   'False
      Tab(0).Control(15)=   "SetTiming(0)"
      Tab(0).Control(15).Enabled=   0   'False
      Tab(0).Control(16)=   "SetReset(2)"
      Tab(0).Control(16).Enabled=   0   'False
      Tab(0).Control(17)=   "SetReset(1)"
      Tab(0).Control(17).Enabled=   0   'False
      Tab(0).Control(18)=   "SetZero(2)"
      Tab(0).Control(18).Enabled=   0   'False
      Tab(0).Control(19)=   "SetZero(1)"
      Tab(0).Control(19).Enabled=   0   'False
      Tab(0).Control(20)=   "ParamFrame(2)"
      Tab(0).Control(20).Enabled=   0   'False
      Tab(0).Control(21)=   "SetTiming(2)"
      Tab(0).Control(21).Enabled=   0   'False
      Tab(0).Control(22)=   "SetTiming(1)"
      Tab(0).Control(22).Enabled=   0   'False
      Tab(0).Control(23)=   "ParamFrame(1)"
      Tab(0).Control(23).Enabled=   0   'False
      Tab(0).Control(24)=   "GetCalcData(2)"
      Tab(0).Control(24).Enabled=   0   'False
      Tab(0).Control(25)=   "ParamFrame(0)"
      Tab(0).Control(25).Enabled=   0   'False
      Tab(0).Control(26)=   "GetCalcDataSingleParam1"
      Tab(0).Control(26).Enabled=   0   'False
      Tab(0).Control(27)=   "GetLight"
      Tab(0).Control(27).Enabled=   0   'False
      Tab(0).Control(28)=   "DataStorageGetDataParam1"
      Tab(0).Control(28).Enabled=   0   'False
      Tab(0).Control(29)=   "DataStorageGetStatus"
      Tab(0).Control(29).Enabled=   0   'False
      Tab(0).Control(30)=   "DataStorageGetData"
      Tab(0).Control(30).Enabled=   0   'False
      Tab(0).Control(31)=   "DataStorageInit"
      Tab(0).Control(31).Enabled=   0   'False
      Tab(0).Control(32)=   "DataStorageStop"
      Tab(0).Control(32).Enabled=   0   'False
      Tab(0).Control(33)=   "DataStorageStart"
      Tab(0).Control(33).Enabled=   0   'False
      Tab(0).Control(34)=   "GetCalcData(0)"
      Tab(0).Control(34).Enabled=   0   'False
      Tab(0).Control(35)=   "SetPanelLock"
      Tab(0).Control(35).Enabled=   0   'False
      Tab(0).Control(36)=   "GetProgramNo"
      Tab(0).Control(36).Enabled=   0   'False
      Tab(0).Control(37)=   "SetProgramNo"
      Tab(0).Control(37).Enabled=   0   'False
      Tab(0).Control(38)=   "CommonDialogForLight"
      Tab(0).Control(38).Enabled=   0   'False
      Tab(0).Control(39)=   "DataStorageGetStatusParam2"
      Tab(0).Control(39).Enabled=   0   'False
      Tab(0).Control(40)=   "DataStorageGetStatusParam1"
      Tab(0).Control(40).Enabled=   0   'False
      Tab(0).Control(41)=   "DataStorageGetDataParam3"
      Tab(0).Control(41).Enabled=   0   'False
      Tab(0).Control(42)=   "GetCalcDataSingleParam2"
      Tab(0).Control(42).Enabled=   0   'False
      Tab(0).Control(43)=   "GetProgramNoParam1"
      Tab(0).Control(43).Enabled=   0   'False
      Tab(0).ControlCount=   44
      TabCaption(1)   =   "Setting change commands 1"
      TabPicture(1)   =   "Form1.frx":001C
      Tab(1).ControlEnabled=   -1  'True
      Tab(1).Control(0)=   "SetMaskParam1(0)"
      Tab(1).Control(0).Enabled=   0   'False
      Tab(1).Control(1)=   "AbleStartParam1"
      Tab(1).Control(1).Enabled=   0   'False
      Tab(1).Control(2)=   "SetAbleMinMaxParam2(1)"
      Tab(1).Control(2).Enabled=   0   'False
      Tab(1).Control(3)=   "SetAbleMinMaxParam2(0)"
      Tab(1).Control(3).Enabled=   0   'False
      Tab(1).Control(4)=   "SetAbleModeParam(1)"
      Tab(1).Control(4).Enabled=   0   'False
      Tab(1).Control(5)=   "SetAbleMinMaxParam1"
      Tab(1).Control(5).Enabled=   0   'False
      Tab(1).Control(6)=   "SetAbleModeParam(0)"
      Tab(1).Control(6).Enabled=   0   'False
      Tab(1).Control(7)=   "SetCalcMethod"
      Tab(1).Control(7).Enabled=   0   'False
      Tab(1).Control(8)=   "AbleCancel"
      Tab(1).Control(8).Enabled=   0   'False
      Tab(1).Control(9)=   "AbleStop"
      Tab(1).Control(9).Enabled=   0   'False
      Tab(1).Control(10)=   "AbleStart"
      Tab(1).Control(10).Enabled=   0   'False
      Tab(1).Control(11)=   "SetAbleMinMax"
      Tab(1).Control(11).Enabled=   0   'False
      Tab(1).Control(12)=   "SetAbleMode"
      Tab(1).Control(12).Enabled=   0   'False
      Tab(1).Control(13)=   "SetPanel"
      Tab(1).Control(13).Enabled=   0   'False
      Tab(1).Control(14)=   "SetReflectionMode"
      Tab(1).Control(14).Enabled=   0   'False
      Tab(1).Control(15)=   "SetMeasureMode"
      Tab(1).Control(15).Enabled=   0   'False
      Tab(1).Control(16)=   "SetAlarmLevel"
      Tab(1).Control(16).Enabled=   0   'False
      Tab(1).Control(17)=   "SetReflectionModeParam(1)"
      Tab(1).Control(17).Enabled=   0   'False
      Tab(1).Control(18)=   "SetMeasureModeParam(1)"
      Tab(1).Control(18).Enabled=   0   'False
      Tab(1).Control(19)=   "SetAlarmLevelParam2"
      Tab(1).Control(19).Enabled=   0   'False
      Tab(1).Control(20)=   "SetAlarmLevelParam1"
      Tab(1).Control(20).Enabled=   0   'False
      Tab(1).Control(21)=   "SetMeasureModeParam(0)"
      Tab(1).Control(21).Enabled=   0   'False
      Tab(1).Control(22)=   "SetReflectionModeParam(0)"
      Tab(1).Control(22).Enabled=   0   'False
      Tab(1).Control(23)=   "SetScaling"
      Tab(1).Control(23).Enabled=   0   'False
      Tab(1).Control(24)=   "SetNumAlarm"
      Tab(1).Control(24).Enabled=   0   'False
      Tab(1).Control(25)=   "SetTolerance"
      Tab(1).Control(25).Enabled=   0   'False
      Tab(1).Control(26)=   "SetNumAlarmParam1"
      Tab(1).Control(26).Enabled=   0   'False
      Tab(1).Control(27)=   "SetNumAlarmParam2"
      Tab(1).Control(27).Enabled=   0   'False
      Tab(1).Control(28)=   "SetToleranceParam2(0)"
      Tab(1).Control(28).Enabled=   0   'False
      Tab(1).Control(29)=   "SetToleranceParam1"
      Tab(1).Control(29).Enabled=   0   'False
      Tab(1).Control(30)=   "SetToleranceParam2(1)"
      Tab(1).Control(30).Enabled=   0   'False
      Tab(1).Control(31)=   "SetToleranceParam2(2)"
      Tab(1).Control(31).Enabled=   0   'False
      Tab(1).Control(32)=   "SetBasicPoint"
      Tab(1).Control(32).Enabled=   0   'False
      Tab(1).Control(33)=   "SetNumRecovery"
      Tab(1).Control(33).Enabled=   0   'False
      Tab(1).Control(34)=   "SetMask"
      Tab(1).Control(34).Enabled=   0   'False
      Tab(1).Control(35)=   "SetMedian"
      Tab(1).Control(35).Enabled=   0   'False
      Tab(1).Control(36)=   "SetCalcTarget"
      Tab(1).Control(36).Enabled=   0   'False
      Tab(1).Control(37)=   "SetOutAddSub"
      Tab(1).Control(37).Enabled=   0   'False
      Tab(1).Control(38)=   "SetOutOperation"
      Tab(1).Control(38).Enabled=   0   'False
      Tab(1).Control(39)=   "SetLaserCtrlGroup"
      Tab(1).Control(39).Enabled=   0   'False
      Tab(1).Control(40)=   "SetRange"
      Tab(1).Control(40).Enabled=   0   'False
      Tab(1).Control(41)=   "SetMutualInterferencePreventionGroup"
      Tab(1).Control(41).Enabled=   0   'False
      Tab(1).Control(42)=   "SetPanelParam(0)"
      Tab(1).Control(42).Enabled=   0   'False
      Tab(1).Control(43)=   "SetPanelParam(1)"
      Tab(1).Control(43).Enabled=   0   'False
      Tab(1).Control(44)=   "SetBasicPointParam(0)"
      Tab(1).Control(44).Enabled=   0   'False
      Tab(1).Control(45)=   "SetBasicPointParam(1)"
      Tab(1).Control(45).Enabled=   0   'False
      Tab(1).Control(46)=   "SetNumRecoveryParam1"
      Tab(1).Control(46).Enabled=   0   'False
      Tab(1).Control(47)=   "SetNumRecoveryParam2"
      Tab(1).Control(47).Enabled=   0   'False
      Tab(1).Control(48)=   "SetMedianParam(0)"
      Tab(1).Control(48).Enabled=   0   'False
      Tab(1).Control(49)=   "SetMaskParam2(0)"
      Tab(1).Control(49).Enabled=   0   'False
      Tab(1).Control(50)=   "SetMaskParam2(1)"
      Tab(1).Control(50).Enabled=   0   'False
      Tab(1).Control(51)=   "SetMaskParam1(1)"
      Tab(1).Control(51).Enabled=   0   'False
      Tab(1).Control(52)=   "SetMedianParam(1)"
      Tab(1).Control(52).Enabled=   0   'False
      Tab(1).Control(53)=   "SetLaserCtrlGroupParam(0)"
      Tab(1).Control(53).Enabled=   0   'False
      Tab(1).Control(54)=   "SetLaserCtrlGroupParam(1)"
      Tab(1).Control(54).Enabled=   0   'False
      Tab(1).Control(55)=   "SetRangeParam(0)"
      Tab(1).Control(55).Enabled=   0   'False
      Tab(1).Control(56)=   "SetRangeParam(1)"
      Tab(1).Control(56).Enabled=   0   'False
      Tab(1).Control(57)=   "SetMutualInterferencePreventionGroupParam(0)"
      Tab(1).Control(57).Enabled=   0   'False
      Tab(1).Control(58)=   "SetMutualInterferencePreventionGroupParam(1)"
      Tab(1).Control(58).Enabled=   0   'False
      Tab(1).Control(59)=   "SetCalcMethodParam1(0)"
      Tab(1).Control(59).Enabled=   0   'False
      Tab(1).Control(60)=   "SetCalcMethodParam1(1)"
      Tab(1).Control(60).Enabled=   0   'False
      Tab(1).Control(61)=   "SetOutAddSubParam(0)"
      Tab(1).Control(61).Enabled=   0   'False
      Tab(1).Control(62)=   "SetOutAddSubParam(1)"
      Tab(1).Control(62).Enabled=   0   'False
      Tab(1).Control(63)=   "SetOutAddSubParam(2)"
      Tab(1).Control(63).Enabled=   0   'False
      Tab(1).Control(64)=   "SetOutOperationParam1"
      Tab(1).Control(64).Enabled=   0   'False
      Tab(1).Control(65)=   "SetScalingParam1"
      Tab(1).Control(65).Enabled=   0   'False
      Tab(1).Control(66)=   "SetScalingParam2(2)"
      Tab(1).Control(66).Enabled=   0   'False
      Tab(1).Control(67)=   "SetScalingParam2(1)"
      Tab(1).Control(67).Enabled=   0   'False
      Tab(1).Control(68)=   "SetScalingParam2(0)"
      Tab(1).Control(68).Enabled=   0   'False
      Tab(1).Control(69)=   "SetScalingParam2(3)"
      Tab(1).Control(69).Enabled=   0   'False
      Tab(1).Control(70)=   "ParamFrame(3)"
      Tab(1).Control(70).Enabled=   0   'False
      Tab(1).Control(71)=   "SetCalcTargetParam(0)"
      Tab(1).Control(71).Enabled=   0   'False
      Tab(1).Control(72)=   "SetCalcTargetParam(1)"
      Tab(1).Control(72).Enabled=   0   'False
      Tab(1).Control(73)=   "SetCalcMethodParam2"
      Tab(1).Control(73).Enabled=   0   'False
      Tab(1).ControlCount=   74
      TabCaption(2)   =   "Setting change commands 2"
      TabPicture(2)   =   "Form1.frx":0038
      Tab(2).ControlEnabled=   0   'False
      Tab(2).Control(0)=   "SetTriggerMode"
      Tab(2).Control(0).Enabled=   0   'False
      Tab(2).Control(1)=   "SetOffset"
      Tab(2).Control(1).Enabled=   0   'False
      Tab(2).Control(2)=   "SetFilter"
      Tab(2).Control(2).Enabled=   0   'False
      Tab(2).Control(3)=   "SetAnalogScaling"
      Tab(2).Control(3).Enabled=   0   'False
      Tab(2).Control(4)=   "SetDataStorage"
      Tab(2).Control(4).Enabled=   0   'False
      Tab(2).Control(5)=   "SetCalcMode"
      Tab(2).Control(5).Enabled=   0   'False
      Tab(2).Control(6)=   "SetSamplingCycle"
      Tab(2).Control(6).Enabled=   0   'False
      Tab(2).Control(7)=   "SetDisplayUnit"
      Tab(2).Control(7).Enabled=   0   'False
      Tab(2).Control(8)=   "SetSynchronization"
      Tab(2).Control(8).Enabled=   0   'False
      Tab(2).Control(9)=   "SetMutualInterferencePrevention"
      Tab(2).Control(9).Enabled=   0   'False
      Tab(2).Control(10)=   "SetToleranceComparatorOutputFormat"
      Tab(2).Control(10).Enabled=   0   'False
      Tab(2).Control(11)=   "SetStorobeTime"
      Tab(2).Control(11).Enabled=   0   'False
      Tab(2).Control(12)=   "SetMeasureType"
      Tab(2).Control(12).Enabled=   0   'False
      Tab(2).Control(13)=   "SetStorageTarget"
      Tab(2).Control(13).Enabled=   0   'False
      Tab(2).Control(14)=   "SetAnalogChannel"
      Tab(2).Control(14).Enabled=   0   'False
      Tab(2).Control(15)=   "SetAlarmOutputForm"
      Tab(2).Control(15).Enabled=   0   'False
      Tab(2).Control(16)=   "SetNumOfUsedHeads"
      Tab(2).Control(16).Enabled=   0   'False
      Tab(2).Control(17)=   "SetNumOfUsedOut"
      Tab(2).Control(17).Enabled=   0   'False
      Tab(2).Control(18)=   "SetNumOfUsedAnalogCh"
      Tab(2).Control(18).Enabled=   0   'False
      Tab(2).Control(19)=   "SetFilterParam(0)"
      Tab(2).Control(19).Enabled=   0   'False
      Tab(2).Control(20)=   "SetFilterParam(1)"
      Tab(2).Control(20).Enabled=   0   'False
      Tab(2).Control(21)=   "SetFilterParam(2)"
      Tab(2).Control(21).Enabled=   0   'False
      Tab(2).Control(22)=   "SetTriggerModeParam(0)"
      Tab(2).Control(22).Enabled=   0   'False
      Tab(2).Control(23)=   "SetOffsetParam2"
      Tab(2).Control(23).Enabled=   0   'False
      Tab(2).Control(24)=   "SetTriggerModeParam(1)"
      Tab(2).Control(24).Enabled=   0   'False
      Tab(2).Control(25)=   "SetOffsetParam1"
      Tab(2).Control(25).Enabled=   0   'False
      Tab(2).Control(26)=   "SetCalcModeParam(0)"
      Tab(2).Control(26).Enabled=   0   'False
      Tab(2).Control(27)=   "SetCalcModeParam(1)"
      Tab(2).Control(27).Enabled=   0   'False
      Tab(2).Control(28)=   "SetAnalogScalingParam2(0)"
      Tab(2).Control(28).Enabled=   0   'False
      Tab(2).Control(29)=   "SetAnalogScalingParam2(1)"
      Tab(2).Control(29).Enabled=   0   'False
      Tab(2).Control(30)=   "SetAnalogScalingParam2(2)"
      Tab(2).Control(30).Enabled=   0   'False
      Tab(2).Control(31)=   "SetAnalogScalingParam2(3)"
      Tab(2).Control(31).Enabled=   0   'False
      Tab(2).Control(32)=   "SetAnalogScalingParam1"
      Tab(2).Control(32).Enabled=   0   'False
      Tab(2).Control(33)=   "SetDisplayUnitParam(0)"
      Tab(2).Control(33).Enabled=   0   'False
      Tab(2).Control(34)=   "SetDisplayUnitParam(1)"
      Tab(2).Control(34).Enabled=   0   'False
      Tab(2).Control(35)=   "SetMeasureTypeParam(0)"
      Tab(2).Control(35).Enabled=   0   'False
      Tab(2).Control(36)=   "SetMeasureTypeParam(1)"
      Tab(2).Control(36).Enabled=   0   'False
      Tab(2).Control(37)=   "SetSynchronizationParam(0)"
      Tab(2).Control(37).Enabled=   0   'False
      Tab(2).Control(38)=   "SetSynchronizationParam(1)"
      Tab(2).Control(38).Enabled=   0   'False
      Tab(2).Control(39)=   "SetStorageTargetParam(0)"
      Tab(2).Control(39).Enabled=   0   'False
      Tab(2).Control(40)=   "SetStorageTargetParam(1)"
      Tab(2).Control(40).Enabled=   0   'False
      Tab(2).Control(41)=   "SetSamplingCycleParam"
      Tab(2).Control(41).Enabled=   0   'False
      Tab(2).Control(42)=   "SetMutualInterferencePreventionParam"
      Tab(2).Control(42).Enabled=   0   'False
      Tab(2).Control(43)=   "SetStorobeTimeParam"
      Tab(2).Control(43).Enabled=   0   'False
      Tab(2).Control(44)=   "SetDataStorageParam1"
      Tab(2).Control(44).Enabled=   0   'False
      Tab(2).Control(45)=   "SetDataStorageParam2"
      Tab(2).Control(45).Enabled=   0   'False
      Tab(2).Control(46)=   "SetAnalogChannelParam(0)"
      Tab(2).Control(46).Enabled=   0   'False
      Tab(2).Control(47)=   "SetAnalogChannelParam(1)"
      Tab(2).Control(47).Enabled=   0   'False
      Tab(2).Control(48)=   "SetAlarmOutputFormParam"
      Tab(2).Control(48).Enabled=   0   'False
      Tab(2).Control(49)=   "SetToleranceComparatorOutputFormatParam"
      Tab(2).Control(49).Enabled=   0   'False
      Tab(2).Control(50)=   "SetNumOfUsedHeadsParam"
      Tab(2).Control(50).Enabled=   0   'False
      Tab(2).Control(51)=   "SetNumOfUsedOutParam"
      Tab(2).Control(51).Enabled=   0   'False
      Tab(2).Control(52)=   "SetNumOfUsedAnalogChParam"
      Tab(2).Control(52).Enabled=   0   'False
      Tab(2).ControlCount=   53
      TabCaption(3)   =   "Setting confirmation commands 1"
      TabPicture(3)   =   "Form1.frx":0054
      Tab(3).ControlEnabled=   0   'False
      Tab(3).Control(0)=   "GetToleranceParam2(0)"
      Tab(3).Control(0).Enabled=   0   'False
      Tab(3).Control(1)=   "GetToleranceParam2(1)"
      Tab(3).Control(1).Enabled=   0   'False
      Tab(3).Control(2)=   "GetToleranceParam2(2)"
      Tab(3).Control(2).Enabled=   0   'False
      Tab(3).Control(3)=   "GetAbleModeParam2"
      Tab(3).Control(3).Enabled=   0   'False
      Tab(3).Control(4)=   "GetAbleMinMaxParam2"
      Tab(3).Control(4).Enabled=   0   'False
      Tab(3).Control(5)=   "GetAbleMinMaxParam3"
      Tab(3).Control(5).Enabled=   0   'False
      Tab(3).Control(6)=   "GetMeasureModeParam2"
      Tab(3).Control(6).Enabled=   0   'False
      Tab(3).Control(7)=   "GetNumAlarmParam2"
      Tab(3).Control(7).Enabled=   0   'False
      Tab(3).Control(8)=   "GetPanelParam(0)"
      Tab(3).Control(8).Enabled=   0   'False
      Tab(3).Control(9)=   "GetPanelParam(1)"
      Tab(3).Control(9).Enabled=   0   'False
      Tab(3).Control(10)=   "GetBasicPointParam2"
      Tab(3).Control(10).Enabled=   0   'False
      Tab(3).Control(11)=   "GetNumRecoveryParam2"
      Tab(3).Control(11).Enabled=   0   'False
      Tab(3).Control(12)=   "GetAlarmLevelParam2"
      Tab(3).Control(12).Enabled=   0   'False
      Tab(3).Control(13)=   "GetReflectionModeParam2"
      Tab(3).Control(13).Enabled=   0   'False
      Tab(3).Control(14)=   "GetMaskParam2(0)"
      Tab(3).Control(14).Enabled=   0   'False
      Tab(3).Control(15)=   "GetMaskParam2(1)"
      Tab(3).Control(15).Enabled=   0   'False
      Tab(3).Control(16)=   "GetMaskParam2(2)"
      Tab(3).Control(16).Enabled=   0   'False
      Tab(3).Control(17)=   "GetMedianParam2"
      Tab(3).Control(17).Enabled=   0   'False
      Tab(3).Control(18)=   "GetLaserCtrlGroupParam2"
      Tab(3).Control(18).Enabled=   0   'False
      Tab(3).Control(19)=   "GetRangeParam2"
      Tab(3).Control(19).Enabled=   0   'False
      Tab(3).Control(20)=   "GetMutualInterferencePreventionGroupParam2"
      Tab(3).Control(20).Enabled=   0   'False
      Tab(3).Control(21)=   "GetCalcMethodParam2(0)"
      Tab(3).Control(21).Enabled=   0   'False
      Tab(3).Control(22)=   "GetCalcMethodParam2(1)"
      Tab(3).Control(22).Enabled=   0   'False
      Tab(3).Control(23)=   "GetCalcTargetParam2"
      Tab(3).Control(23).Enabled=   0   'False
      Tab(3).Control(24)=   "GetOutAddSubParam2(0)"
      Tab(3).Control(24).Enabled=   0   'False
      Tab(3).Control(25)=   "GetOutAddSubParam2(1)"
      Tab(3).Control(25).Enabled=   0   'False
      Tab(3).Control(26)=   "GetScalingParam2(0)"
      Tab(3).Control(26).Enabled=   0   'False
      Tab(3).Control(27)=   "GetScalingParam2(1)"
      Tab(3).Control(27).Enabled=   0   'False
      Tab(3).Control(28)=   "GetScalingParam2(2)"
      Tab(3).Control(28).Enabled=   0   'False
      Tab(3).Control(29)=   "GetScalingParam2(3)"
      Tab(3).Control(29).Enabled=   0   'False
      Tab(3).Control(30)=   "GetMeasureMode"
      Tab(3).Control(30).Enabled=   0   'False
      Tab(3).Control(31)=   "GetReflectionMode"
      Tab(3).Control(31).Enabled=   0   'False
      Tab(3).Control(32)=   "GetNumAlarm"
      Tab(3).Control(32).Enabled=   0   'False
      Tab(3).Control(33)=   "GetTolerance"
      Tab(3).Control(33).Enabled=   0   'False
      Tab(3).Control(34)=   "GetPanel"
      Tab(3).Control(34).Enabled=   0   'False
      Tab(3).Control(35)=   "GetAbleMode"
      Tab(3).Control(35).Enabled=   0   'False
      Tab(3).Control(36)=   "GetAbleMinMax"
      Tab(3).Control(36).Enabled=   0   'False
      Tab(3).Control(37)=   "GetMask"
      Tab(3).Control(37).Enabled=   0   'False
      Tab(3).Control(38)=   "GetMedian"
      Tab(3).Control(38).Enabled=   0   'False
      Tab(3).Control(39)=   "GetToleranceParam1"
      Tab(3).Control(39).Enabled=   0   'False
      Tab(3).Control(40)=   "GetAbleModeParam1"
      Tab(3).Control(40).Enabled=   0   'False
      Tab(3).Control(41)=   "GetAbleMinMaxParam1"
      Tab(3).Control(41).Enabled=   0   'False
      Tab(3).Control(42)=   "GetMeasureModeParam1"
      Tab(3).Control(42).Enabled=   0   'False
      Tab(3).Control(43)=   "GetNumAlarmParam1"
      Tab(3).Control(43).Enabled=   0   'False
      Tab(3).Control(44)=   "GetBasicPoint"
      Tab(3).Control(44).Enabled=   0   'False
      Tab(3).Control(45)=   "GetNumRecovery"
      Tab(3).Control(45).Enabled=   0   'False
      Tab(3).Control(46)=   "GetAlarmLevel"
      Tab(3).Control(46).Enabled=   0   'False
      Tab(3).Control(47)=   "GetLaserCtrlGroup"
      Tab(3).Control(47).Enabled=   0   'False
      Tab(3).Control(48)=   "GetRange"
      Tab(3).Control(48).Enabled=   0   'False
      Tab(3).Control(49)=   "GetMutualInterferencePreventionGroup"
      Tab(3).Control(49).Enabled=   0   'False
      Tab(3).Control(50)=   "GetCalcMethod"
      Tab(3).Control(50).Enabled=   0   'False
      Tab(3).Control(51)=   "GetCalcTarget"
      Tab(3).Control(51).Enabled=   0   'False
      Tab(3).Control(52)=   "GetOutAddSub"
      Tab(3).Control(52).Enabled=   0   'False
      Tab(3).Control(53)=   "GetOutOperation"
      Tab(3).Control(53).Enabled=   0   'False
      Tab(3).Control(54)=   "GetScaling"
      Tab(3).Control(54).Enabled=   0   'False
      Tab(3).Control(55)=   "GetBasicPointParam1"
      Tab(3).Control(55).Enabled=   0   'False
      Tab(3).Control(56)=   "GetNumRecoveryParam1"
      Tab(3).Control(56).Enabled=   0   'False
      Tab(3).Control(57)=   "GetAlarmLevelParam1"
      Tab(3).Control(57).Enabled=   0   'False
      Tab(3).Control(58)=   "GetReflectionModeParam1"
      Tab(3).Control(58).Enabled=   0   'False
      Tab(3).Control(59)=   "GetMaskParam1"
      Tab(3).Control(59).Enabled=   0   'False
      Tab(3).Control(60)=   "GetMedianParam1"
      Tab(3).Control(60).Enabled=   0   'False
      Tab(3).Control(61)=   "GetLaserCtrlGroupParam1"
      Tab(3).Control(61).Enabled=   0   'False
      Tab(3).Control(62)=   "GetRangeParam1"
      Tab(3).Control(62).Enabled=   0   'False
      Tab(3).Control(63)=   "GetMutualInterferencePreventionGroupParam1"
      Tab(3).Control(63).Enabled=   0   'False
      Tab(3).Control(64)=   "GetCalcMethodParam1"
      Tab(3).Control(64).Enabled=   0   'False
      Tab(3).Control(65)=   "GetCalcTargetParam1"
      Tab(3).Control(65).Enabled=   0   'False
      Tab(3).Control(66)=   "GetOutAddSubParam1"
      Tab(3).Control(66).Enabled=   0   'False
      Tab(3).Control(67)=   "GetOutOperationParam1"
      Tab(3).Control(67).Enabled=   0   'False
      Tab(3).Control(68)=   "GetScalingParam1"
      Tab(3).Control(68).Enabled=   0   'False
      Tab(3).Control(69)=   "ParamFrame(4)"
      Tab(3).Control(69).Enabled=   0   'False
      Tab(3).ControlCount=   70
      TabCaption(4)   =   "Setting confirmation commands 2"
      TabPicture(4)   =   "Form1.frx":0070
      Tab(4).ControlEnabled=   0   'False
      Tab(4).Control(0)=   "GetAnalogChannelParam1"
      Tab(4).Control(0).Enabled=   0   'False
      Tab(4).Control(1)=   "GetStorageTargetParam1"
      Tab(4).Control(1).Enabled=   0   'False
      Tab(4).Control(2)=   "GetSynchronizationParam1"
      Tab(4).Control(2).Enabled=   0   'False
      Tab(4).Control(3)=   "GetMeasureTypeParam1"
      Tab(4).Control(3).Enabled=   0   'False
      Tab(4).Control(4)=   "GetDisplayUnitParam1"
      Tab(4).Control(4).Enabled=   0   'False
      Tab(4).Control(5)=   "GetAnalogScalingParam1"
      Tab(4).Control(5).Enabled=   0   'False
      Tab(4).Control(6)=   "GetCalcModeParam1"
      Tab(4).Control(6).Enabled=   0   'False
      Tab(4).Control(7)=   "GetOffsetParam1"
      Tab(4).Control(7).Enabled=   0   'False
      Tab(4).Control(8)=   "GetTriggerModeParam1"
      Tab(4).Control(8).Enabled=   0   'False
      Tab(4).Control(9)=   "GetFilterParam1"
      Tab(4).Control(9).Enabled=   0   'False
      Tab(4).Control(10)=   "GetNumOfUsedAnalogCh"
      Tab(4).Control(10).Enabled=   0   'False
      Tab(4).Control(11)=   "GetNumOfUsedOut"
      Tab(4).Control(11).Enabled=   0   'False
      Tab(4).Control(12)=   "GetNumOfUsedHeads"
      Tab(4).Control(12).Enabled=   0   'False
      Tab(4).Control(13)=   "GetAlarmOutputForm"
      Tab(4).Control(13).Enabled=   0   'False
      Tab(4).Control(14)=   "GetAnalogChannel"
      Tab(4).Control(14).Enabled=   0   'False
      Tab(4).Control(15)=   "GetStorageTarget"
      Tab(4).Control(15).Enabled=   0   'False
      Tab(4).Control(16)=   "GetStorobeTime"
      Tab(4).Control(16).Enabled=   0   'False
      Tab(4).Control(17)=   "GetSynchronization"
      Tab(4).Control(17).Enabled=   0   'False
      Tab(4).Control(18)=   "GetMutualInterferencePrevention"
      Tab(4).Control(18).Enabled=   0   'False
      Tab(4).Control(19)=   "GetSamplingCycle"
      Tab(4).Control(19).Enabled=   0   'False
      Tab(4).Control(20)=   "GetCalcMode"
      Tab(4).Control(20).Enabled=   0   'False
      Tab(4).Control(21)=   "GetAnalogScaling"
      Tab(4).Control(21).Enabled=   0   'False
      Tab(4).Control(22)=   "GetTriggerMode"
      Tab(4).Control(22).Enabled=   0   'False
      Tab(4).Control(23)=   "GetOffset"
      Tab(4).Control(23).Enabled=   0   'False
      Tab(4).Control(24)=   "GetMeasureType"
      Tab(4).Control(24).Enabled=   0   'False
      Tab(4).Control(25)=   "GetDataStorage"
      Tab(4).Control(25).Enabled=   0   'False
      Tab(4).Control(26)=   "GetDisplayUnit"
      Tab(4).Control(26).Enabled=   0   'False
      Tab(4).Control(27)=   "GetToleranceComparatorOutputFormat"
      Tab(4).Control(27).Enabled=   0   'False
      Tab(4).Control(28)=   "GetFilter"
      Tab(4).Control(28).Enabled=   0   'False
      Tab(4).Control(29)=   "GetNumOfUsedAnalogChParam"
      Tab(4).Control(29).Enabled=   0   'False
      Tab(4).Control(30)=   "GetNumOfUsedOutParam"
      Tab(4).Control(30).Enabled=   0   'False
      Tab(4).Control(31)=   "GetNumOfUsedHeadsParam"
      Tab(4).Control(31).Enabled=   0   'False
      Tab(4).Control(32)=   "GetAlarmOutputFormParam"
      Tab(4).Control(32).Enabled=   0   'False
      Tab(4).Control(33)=   "GetAnalogChannelParam2"
      Tab(4).Control(33).Enabled=   0   'False
      Tab(4).Control(34)=   "GetDataStorageParam(1)"
      Tab(4).Control(34).Enabled=   0   'False
      Tab(4).Control(35)=   "GetDataStorageParam(0)"
      Tab(4).Control(35).Enabled=   0   'False
      Tab(4).Control(36)=   "GetStorobeTimeParam"
      Tab(4).Control(36).Enabled=   0   'False
      Tab(4).Control(37)=   "GetToleranceComparatorOutputFormatParam"
      Tab(4).Control(37).Enabled=   0   'False
      Tab(4).Control(38)=   "GetMutualInterferencePreventionParam"
      Tab(4).Control(38).Enabled=   0   'False
      Tab(4).Control(39)=   "GetSamplingCycleParam"
      Tab(4).Control(39).Enabled=   0   'False
      Tab(4).Control(40)=   "GetStorageTargetParam2"
      Tab(4).Control(40).Enabled=   0   'False
      Tab(4).Control(41)=   "GetSynchronizationParam2"
      Tab(4).Control(41).Enabled=   0   'False
      Tab(4).Control(42)=   "GetMeasureTypeParam2"
      Tab(4).Control(42).Enabled=   0   'False
      Tab(4).Control(43)=   "GetDisplayUnitParam2"
      Tab(4).Control(43).Enabled=   0   'False
      Tab(4).Control(44)=   "GetAnalogScalingParam2(3)"
      Tab(4).Control(44).Enabled=   0   'False
      Tab(4).Control(45)=   "GetAnalogScalingParam2(2)"
      Tab(4).Control(45).Enabled=   0   'False
      Tab(4).Control(46)=   "GetAnalogScalingParam2(1)"
      Tab(4).Control(46).Enabled=   0   'False
      Tab(4).Control(47)=   "GetAnalogScalingParam2(0)"
      Tab(4).Control(47).Enabled=   0   'False
      Tab(4).Control(48)=   "GetCalcModeParam2"
      Tab(4).Control(48).Enabled=   0   'False
      Tab(4).Control(49)=   "GetOffsetParam2"
      Tab(4).Control(49).Enabled=   0   'False
      Tab(4).Control(50)=   "GetTriggerModeParam2"
      Tab(4).Control(50).Enabled=   0   'False
      Tab(4).Control(51)=   "GetFilterParam2(1)"
      Tab(4).Control(51).Enabled=   0   'False
      Tab(4).Control(52)=   "GetFilterParam2(0)"
      Tab(4).Control(52).Enabled=   0   'False
      Tab(4).ControlCount=   53
      TabCaption(5)   =   "Mode change commands"
      TabPicture(5)   =   "Form1.frx":008C
      Tab(5).ControlEnabled=   0   'False
      Tab(5).Control(0)=   "StopMeasure"
      Tab(5).Control(0).Enabled=   0   'False
      Tab(5).Control(1)=   "StartMeasure"
      Tab(5).Control(1).Enabled=   0   'False
      Tab(5).ControlCount=   2
      TabCaption(6)   =   "Other commands"
      TabPicture(6)   =   "Form1.frx":00A8
      Tab(6).ControlEnabled=   0   'False
      Tab(6).Control(0)=   "OpenDeviceETHERParamSub(3)"
      Tab(6).Control(0).Enabled=   0   'False
      Tab(6).Control(1)=   "OpenDeviceETHERParamSub(2)"
      Tab(6).Control(1).Enabled=   0   'False
      Tab(6).Control(2)=   "OpenDeviceETHERParamSub(1)"
      Tab(6).Control(2).Enabled=   0   'False
      Tab(6).Control(3)=   "OpenDeviceETHERParamSub(0)"
      Tab(6).Control(3).Enabled=   0   'False
      Tab(6).Control(4)=   "OpenDeviceETHERParam1(3)"
      Tab(6).Control(4).Enabled=   0   'False
      Tab(6).Control(5)=   "OpenDeviceETHERParam1(2)"
      Tab(6).Control(5).Enabled=   0   'False
      Tab(6).Control(6)=   "OpenDeviceETHERParam1(1)"
      Tab(6).Control(6).Enabled=   0   'False
      Tab(6).Control(7)=   "OpenDeviceETHERParam1(0)"
      Tab(6).Control(7).Enabled=   0   'False
      Tab(6).Control(8)=   "CloseDevice"
      Tab(6).Control(8).Enabled=   0   'False
      Tab(6).Control(9)=   "OpenDeviceETHER"
      Tab(6).Control(9).Enabled=   0   'False
      Tab(6).Control(10)=   "OpenDeviceUSB"
      Tab(6).Control(10).Enabled=   0   'False
      Tab(6).ControlCount=   11
      Begin VB.CommandButton GetCalcData 
         Caption         =   "Measured value output (multiple)(LKIF2_GetCalcDataMulti)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -74880
         TabIndex        =   392
         Top             =   1320
         Width           =   5300
      End
      Begin VB.ComboBox GetAnalogChannelParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":00C4
         Left            =   -69480
         List            =   "Form1.frx":00EE
         TabIndex        =   364
         Text            =   "SetAnalogChannelParam1"
         Top             =   6480
         Width           =   2175
      End
      Begin VB.TextBox SetCalcMethodParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   9960
         TabIndex        =   391
         Text            =   "0"
         Top             =   7320
         Width           =   2055
      End
      Begin VB.Frame ParamFrame 
         Caption         =   "GetOutOperationParam2"
         Enabled         =   0   'False
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   1215
         Index           =   4
         Left            =   -68160
         TabIndex        =   302
         Top             =   7560
         Width           =   3615
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT12"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   11
            Left            =   2640
            TabIndex        =   313
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT11"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   10
            Left            =   1800
            TabIndex        =   312
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT10"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   9
            Left            =   960
            TabIndex        =   311
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT9"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   8
            Left            =   120
            TabIndex        =   310
            Top             =   960
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT4"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   3
            Left            =   2640
            TabIndex        =   305
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT8"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   7
            Left            =   2640
            TabIndex        =   309
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT7"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   6
            Left            =   1800
            TabIndex        =   308
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT6"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   5
            Left            =   960
            TabIndex        =   307
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT5"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   4
            Left            =   120
            TabIndex        =   306
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT3"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   2
            Left            =   1800
            TabIndex        =   304
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT2"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   1
            Left            =   960
            TabIndex        =   303
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox GetOutOperationParam2 
            Caption         =   "OUT1"
            Enabled         =   0   'False
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   0
            Left            =   120
            TabIndex        =   340
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.TextBox GetLightParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Left            =   -68160
         TabIndex        =   99
         Text            =   "0"
         Top             =   11160
         Width           =   1215
      End
      Begin VB.TextBox DataStorageGetDataParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Left            =   -68160
         TabIndex        =   91
         Text            =   "0"
         Top             =   10200
         Width           =   1575
      End
      Begin MSComDlg.CommonDialog CommonDialogForDataStorage 
         Left            =   -62880
         Top             =   9000
         _ExtentX        =   847
         _ExtentY        =   847
         _Version        =   393216
         FileName        =   "DataStorageLog.csv"
         Filter          =   "CSV|*.csv|"
      End
      Begin VB.TextBox SetNumOfUsedAnalogChParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   -69480
         TabIndex        =   238
         Text            =   "0"
         Top             =   8040
         Width           =   1215
      End
      Begin VB.TextBox SetNumOfUsedOutParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   -69480
         TabIndex        =   236
         Text            =   "0"
         Top             =   7545
         Width           =   1215
      End
      Begin VB.TextBox SetNumOfUsedHeadsParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   -69480
         TabIndex        =   234
         Text            =   "0"
         Top             =   7185
         Width           =   1215
      End
      Begin VB.ComboBox SetCalcTargetParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":016D
         Left            =   6840
         List            =   "Form1.frx":018F
         TabIndex        =   160
         Text            =   "SetCalcTargetParam"
         Top             =   7680
         Width           =   3000
      End
      Begin VB.ComboBox SetCalcTargetParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":028F
         Left            =   5520
         List            =   "Form1.frx":02B9
         TabIndex        =   159
         Text            =   "SetCalcTargetParam"
         Top             =   7680
         Width           =   1215
      End
      Begin VB.ComboBox SyncParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0308
         Left            =   -69480
         List            =   "Form1.frx":0312
         TabIndex        =   78
         Text            =   "SyncParam"
         Top             =   7680
         Width           =   1215
      End
      Begin VB.Frame ParamFrame 
         Caption         =   "SetOutOperationParam2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   1215
         Index           =   3
         Left            =   6840
         TabIndex        =   167
         Top             =   8400
         Width           =   3615
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT1"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   0
            Left            =   120
            TabIndex        =   168
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT2"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   1
            Left            =   960
            TabIndex        =   169
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT3"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   2
            Left            =   1800
            TabIndex        =   170
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT5"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   4
            Left            =   120
            TabIndex        =   172
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT6"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   5
            Left            =   960
            TabIndex        =   173
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT7"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   6
            Left            =   1800
            TabIndex        =   174
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT8"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   7
            Left            =   2640
            TabIndex        =   175
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT4"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   3
            Left            =   2640
            TabIndex        =   171
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT9"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   8
            Left            =   120
            TabIndex        =   176
            Top             =   960
            Width           =   735
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT10"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   9
            Left            =   960
            TabIndex        =   177
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT11"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   10
            Left            =   1800
            TabIndex        =   178
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox SetOutOperationParam2 
            Caption         =   "OUT12"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   11
            Left            =   2640
            TabIndex        =   179
            Top             =   960
            Width           =   855
         End
      End
      Begin VB.ComboBox GetStorageTargetParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":031F
         Left            =   -69480
         List            =   "Form1.frx":0349
         TabIndex        =   350
         Text            =   "GetStorageTargetParam1"
         Top             =   3920
         Width           =   1215
      End
      Begin VB.ComboBox GetSynchronizationParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0398
         Left            =   -69480
         List            =   "Form1.frx":03C2
         TabIndex        =   347
         Text            =   "GetSynchronizationParam1"
         Top             =   3570
         Width           =   1215
      End
      Begin VB.ComboBox GetMeasureTypeParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0411
         Left            =   -69480
         List            =   "Form1.frx":043B
         TabIndex        =   344
         Text            =   "GetMeasureTypeParam1"
         Top             =   3220
         Width           =   1215
      End
      Begin VB.ComboBox GetDisplayUnitParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":048A
         Left            =   -69480
         List            =   "Form1.frx":04B4
         TabIndex        =   341
         Text            =   "GetDisplayUnitParam1"
         Top             =   2880
         Width           =   1215
      End
      Begin VB.ComboBox GetAnalogScalingParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0503
         Left            =   -69480
         List            =   "Form1.frx":052D
         TabIndex        =   334
         Text            =   "GetAnalogScalingParam1"
         Top             =   2160
         Width           =   1215
      End
      Begin VB.ComboBox GetCalcModeParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":057C
         Left            =   -69480
         List            =   "Form1.frx":05A6
         TabIndex        =   331
         Text            =   "GetCalcModeParam1"
         Top             =   1820
         Width           =   1215
      End
      Begin VB.ComboBox GetOffsetParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":05F5
         Left            =   -69480
         List            =   "Form1.frx":061F
         TabIndex        =   328
         Text            =   "GetOffsetParam1"
         Top             =   1460
         Width           =   1215
      End
      Begin VB.ComboBox GetTriggerModeParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":066E
         Left            =   -69480
         List            =   "Form1.frx":0698
         TabIndex        =   325
         Text            =   "GetTriggerModeParam1"
         Top             =   1110
         Width           =   1215
      End
      Begin VB.ComboBox GetFilterParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":06E7
         Left            =   -69480
         List            =   "Form1.frx":0711
         TabIndex        =   321
         Text            =   "GetFilterParam1"
         Top             =   750
         Width           =   1215
      End
      Begin VB.ComboBox GetScalingParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0760
         Left            =   -69480
         List            =   "Form1.frx":078A
         TabIndex        =   315
         Text            =   "GetScalingParam1"
         Top             =   8835
         Width           =   1215
      End
      Begin VB.ComboBox GetOutOperationParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":07D9
         Left            =   -69480
         List            =   "Form1.frx":0803
         TabIndex        =   301
         Text            =   "GetOutOperationParam1"
         Top             =   7680
         Width           =   1215
      End
      Begin VB.ComboBox GetOutAddSubParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0852
         Left            =   -69480
         List            =   "Form1.frx":087C
         TabIndex        =   297
         Text            =   "GetOutAddSubParam1"
         Top             =   7200
         Width           =   1215
      End
      Begin VB.ComboBox GetCalcTargetParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":08CB
         Left            =   -69480
         List            =   "Form1.frx":08F5
         TabIndex        =   294
         Text            =   "GetWaveMethodParam1"
         Top             =   6840
         Width           =   1215
      End
      Begin VB.ComboBox GetCalcMethodParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0944
         Left            =   -69480
         List            =   "Form1.frx":096E
         TabIndex        =   290
         Text            =   "GetCalcMethodParam1"
         Top             =   6480
         Width           =   1215
      End
      Begin VB.ComboBox GetMutualInterferencePreventionGroupParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":09BD
         Left            =   -69480
         List            =   "Form1.frx":09E7
         TabIndex        =   287
         Text            =   "GetMutualInterferencePreventionGroupParam1"
         Top             =   5760
         Width           =   1215
      End
      Begin VB.ComboBox GetRangeParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0A42
         Left            =   -69480
         List            =   "Form1.frx":0A6C
         TabIndex        =   284
         Text            =   "GetRangeParam1"
         Top             =   5320
         Width           =   1215
      End
      Begin VB.ComboBox GetLaserCtrlGroupParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0AC7
         Left            =   -69480
         List            =   "Form1.frx":0AF1
         TabIndex        =   281
         Text            =   "GetLaserCtrlGroupParam1"
         Top             =   4960
         Width           =   1215
      End
      Begin VB.ComboBox GetMedianParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0B4C
         Left            =   -69480
         List            =   "Form1.frx":0B76
         TabIndex        =   278
         Text            =   "GetMedianParam1"
         Top             =   4620
         Width           =   1215
      End
      Begin VB.ComboBox GetMaskParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0BD1
         Left            =   -69480
         List            =   "Form1.frx":0BFB
         TabIndex        =   273
         Text            =   "GetMaskParam1"
         Top             =   4280
         Width           =   1215
      End
      Begin VB.ComboBox GetReflectionModeParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0C56
         Left            =   -69480
         List            =   "Form1.frx":0C80
         TabIndex        =   270
         Text            =   "GetReflectionModeParam1"
         Top             =   3920
         Width           =   1215
      End
      Begin VB.ComboBox GetAlarmLevelParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0CDB
         Left            =   -69480
         List            =   "Form1.frx":0D05
         TabIndex        =   267
         Text            =   "GetAlarmLevelParam1"
         Top             =   3580
         Width           =   1215
      End
      Begin VB.ComboBox GetNumRecoveryParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0D60
         Left            =   -69480
         List            =   "Form1.frx":0D8A
         TabIndex        =   264
         Text            =   "GetNumRecoveryParam1"
         Top             =   3220
         Width           =   1215
      End
      Begin VB.ComboBox GetBasicPointParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0DE5
         Left            =   -69480
         List            =   "Form1.frx":0E0F
         TabIndex        =   258
         Text            =   "GetBasicPointParam1"
         Top             =   2520
         Width           =   1215
      End
      Begin VB.ComboBox SetToleranceComparatorOutputFormatParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0E6A
         Left            =   -69480
         List            =   "Form1.frx":0E77
         TabIndex        =   222
         Text            =   "SetToleranceComparatorOutputFormatParam"
         Top             =   5040
         Width           =   5685
      End
      Begin VB.ComboBox SetAlarmOutputFormParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":0F0C
         Left            =   -69480
         List            =   "Form1.frx":0F19
         TabIndex        =   232
         Text            =   "SetAlarmOutputFormParam"
         Top             =   6585
         Width           =   3855
      End
      Begin VB.ComboBox SetAnalogChannelParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":0F79
         Left            =   -67920
         List            =   "Form1.frx":0FA3
         TabIndex        =   230
         Text            =   "SetAnalogChannelParam"
         Top             =   6225
         Width           =   1215
      End
      Begin VB.ComboBox SetAnalogChannelParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":0FF2
         Left            =   -69480
         List            =   "Form1.frx":101C
         TabIndex        =   229
         Text            =   "SetAnalogChannelParam"
         Top             =   6225
         Width           =   1455
      End
      Begin VB.ComboBox SetDataStorageParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":109B
         Left            =   -68160
         List            =   "Form1.frx":10C1
         TabIndex        =   227
         Text            =   "SetDataStorageParam2"
         Top             =   5880
         Width           =   3405
      End
      Begin VB.TextBox SetDataStorageParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   -69480
         MaxLength       =   7
         TabIndex        =   226
         Text            =   "Text2"
         Top             =   5895
         Width           =   1215
      End
      Begin VB.ComboBox SetStorobeTimeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":11BD
         Left            =   -69480
         List            =   "Form1.frx":11CD
         TabIndex        =   224
         Text            =   "SetStorobeTimeParam"
         Top             =   5535
         Width           =   3400
      End
      Begin VB.ComboBox SetMutualInterferencePreventionParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":122B
         Left            =   -69480
         List            =   "Form1.frx":1238
         TabIndex        =   220
         Text            =   "SetMutualInterferencePreventionParam"
         Top             =   4440
         Width           =   4725
      End
      Begin VB.ComboBox SetSamplingCycleParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":12BC
         Left            =   -69480
         List            =   "Form1.frx":12DB
         TabIndex        =   218
         Text            =   "SetSamplingCycleParam"
         Top             =   4020
         Width           =   3405
      End
      Begin VB.ComboBox SetStorageTargetParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":13D5
         Left            =   -68160
         List            =   "Form1.frx":13DF
         TabIndex        =   216
         Text            =   "SetStorageTargetParam"
         Top             =   3555
         Width           =   1000
      End
      Begin VB.ComboBox SetStorageTargetParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":13EC
         Left            =   -69480
         List            =   "Form1.frx":1416
         TabIndex        =   215
         Text            =   "SetStorageTargetParam"
         Top             =   3555
         Width           =   1215
      End
      Begin VB.ComboBox SetSynchronizationParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":1465
         Left            =   -68160
         List            =   "Form1.frx":146F
         TabIndex        =   213
         Text            =   "SetSynchronizationParam"
         Top             =   3195
         Width           =   1000
      End
      Begin VB.ComboBox SetSynchronizationParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":147C
         Left            =   -69480
         List            =   "Form1.frx":14A6
         TabIndex        =   212
         Text            =   "SetSynchronizationParam"
         Top             =   3195
         Width           =   1215
      End
      Begin VB.ComboBox SetMeasureTypeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":14F5
         Left            =   -68160
         List            =   "Form1.frx":1502
         TabIndex        =   210
         Text            =   "SetSetMeasureTypeParam"
         Top             =   2835
         Width           =   3400
      End
      Begin VB.ComboBox SetMeasureTypeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":155C
         Left            =   -69480
         List            =   "Form1.frx":1586
         TabIndex        =   209
         Text            =   "SetSetMeasureTypeParam"
         Top             =   2835
         Width           =   1215
      End
      Begin VB.ComboBox SetDisplayUnitParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":15D5
         Left            =   -68160
         List            =   "Form1.frx":15EE
         TabIndex        =   207
         Text            =   "SetDisplayUnitParam"
         Top             =   2475
         Width           =   3400
      End
      Begin VB.ComboBox SetDisplayUnitParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":16B6
         Left            =   -69480
         List            =   "Form1.frx":16E0
         TabIndex        =   206
         Text            =   "SetDisplayUnitParam"
         Top             =   2475
         Width           =   1215
      End
      Begin VB.ComboBox SetAnalogScalingParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":172F
         Left            =   -69480
         List            =   "Form1.frx":1759
         TabIndex        =   200
         Text            =   "SetAnalogScalingParam1"
         Top             =   2140
         Width           =   1215
      End
      Begin VB.TextBox SetAnalogScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   3
         Left            =   -64200
         TabIndex        =   204
         Text            =   "0"
         Top             =   2130
         Width           =   1215
      End
      Begin VB.TextBox SetAnalogScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   2
         Left            =   -65520
         TabIndex        =   203
         Text            =   "0"
         Top             =   2130
         Width           =   1215
      End
      Begin VB.TextBox SetAnalogScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   1
         Left            =   -66840
         TabIndex        =   202
         Text            =   "0"
         Top             =   2140
         Width           =   1215
      End
      Begin VB.TextBox SetAnalogScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   0
         Left            =   -68160
         TabIndex        =   201
         Text            =   "0"
         Top             =   2140
         Width           =   1215
      End
      Begin VB.TextBox SetScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   3
         Left            =   8160
         TabIndex        =   185
         Text            =   "0"
         Top             =   10080
         Width           =   1215
      End
      Begin VB.ComboBox SetCalcModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":17A8
         Left            =   -68160
         List            =   "Form1.frx":17BE
         TabIndex        =   198
         Text            =   "SetCalcModeParam"
         Top             =   1780
         Width           =   3400
      End
      Begin VB.ComboBox SetCalcModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":185F
         Left            =   -69480
         List            =   "Form1.frx":1889
         TabIndex        =   197
         Text            =   "SetCalcModeParam"
         Top             =   1780
         Width           =   1215
      End
      Begin VB.ComboBox SetOffsetParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":18D8
         Left            =   -69480
         List            =   "Form1.frx":1902
         TabIndex        =   194
         Text            =   "SetOffsetParam1"
         Top             =   1440
         Width           =   1215
      End
      Begin VB.ComboBox SetTriggerModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":1951
         Left            =   -68160
         List            =   "Form1.frx":195B
         TabIndex        =   192
         Text            =   "SetTriggerModeParam"
         Top             =   1080
         Width           =   3400
      End
      Begin VB.TextBox SetOffsetParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   -68160
         MaxLength       =   7
         TabIndex        =   195
         Text            =   "0"
         Top             =   1440
         Width           =   1935
      End
      Begin VB.ComboBox SetTriggerModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":198E
         Left            =   -69480
         List            =   "Form1.frx":19B8
         TabIndex        =   191
         Text            =   "SetTriggerModeParam"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.ComboBox SetFilterParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   2
         ItemData        =   "Form1.frx":1A07
         Left            =   -64680
         List            =   "Form1.frx":1A29
         TabIndex        =   189
         Text            =   "SetFilterModeParam"
         Top             =   720
         Width           =   3000
      End
      Begin VB.ComboBox SetFilterParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":1B2D
         Left            =   -68160
         List            =   "Form1.frx":1B3A
         TabIndex        =   188
         Text            =   "SetFilterModeParam"
         Top             =   720
         Width           =   3400
      End
      Begin VB.ComboBox SetFilterParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":1B91
         Left            =   -69480
         List            =   "Form1.frx":1BBB
         TabIndex        =   187
         Text            =   "SetFilterModeParam"
         Top             =   720
         Width           =   1215
      End
      Begin VB.TextBox SetScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   0
         Left            =   6840
         TabIndex        =   182
         Text            =   "0"
         Top             =   9735
         Width           =   1215
      End
      Begin VB.TextBox SetScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   1
         Left            =   8160
         TabIndex        =   183
         Text            =   "0"
         Top             =   9735
         Width           =   1215
      End
      Begin VB.TextBox SetScalingParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   2
         Left            =   6840
         TabIndex        =   184
         Text            =   "0"
         Top             =   10080
         Width           =   1215
      End
      Begin VB.ComboBox SetScalingParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":1C0A
         Left            =   5520
         List            =   "Form1.frx":1C34
         TabIndex        =   181
         Text            =   "SetScalingParam1"
         Top             =   9735
         Width           =   1215
      End
      Begin VB.ComboBox SetOutOperationParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":1C83
         Left            =   5520
         List            =   "Form1.frx":1CAD
         TabIndex        =   166
         Text            =   "SetOutOperationParam"
         Top             =   8520
         Width           =   1215
      End
      Begin VB.ComboBox SetOutAddSubParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   2
         ItemData        =   "Form1.frx":1CFC
         Left            =   8160
         List            =   "Form1.frx":1D26
         TabIndex        =   164
         Text            =   "SetOutAddSubParam"
         Top             =   8040
         Width           =   1215
      End
      Begin VB.ComboBox SetOutAddSubParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":1D75
         Left            =   6840
         List            =   "Form1.frx":1D9F
         TabIndex        =   163
         Text            =   "SetOutAddSubParam"
         Top             =   8040
         Width           =   1215
      End
      Begin VB.ComboBox SetOutAddSubParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":1DEE
         Left            =   5520
         List            =   "Form1.frx":1E18
         TabIndex        =   162
         Text            =   "SetOutAddSubParam"
         Top             =   8040
         Width           =   1215
      End
      Begin VB.ComboBox SetCalcMethodParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":1E67
         Left            =   6840
         List            =   "Form1.frx":1E83
         TabIndex        =   157
         Text            =   "SetCalcMethodParam"
         Top             =   7320
         Width           =   3000
      End
      Begin VB.ComboBox SetCalcMethodParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":1F30
         Left            =   5520
         List            =   "Form1.frx":1F5A
         TabIndex        =   156
         Text            =   "SetCalcMethodParam"
         Top             =   7320
         Width           =   1215
      End
      Begin VB.ComboBox SetMutualInterferencePreventionGroupParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":1FA9
         Left            =   6840
         List            =   "Form1.frx":1FB6
         TabIndex        =   154
         Text            =   "SetMutualInterferencePreventionGroupParam"
         Top             =   6795
         Width           =   4800
      End
      Begin VB.ComboBox SetMutualInterferencePreventionGroupParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":2041
         Left            =   5520
         List            =   "Form1.frx":206B
         TabIndex        =   153
         Text            =   "SetMutualInterferencePreventionGroupParam"
         Top             =   6795
         Width           =   1215
      End
      Begin VB.ComboBox SetRangeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":20C6
         Left            =   6840
         List            =   "Form1.frx":20D0
         TabIndex        =   151
         Text            =   "SetRangeParam"
         Top             =   6320
         Width           =   3000
      End
      Begin VB.ComboBox SetRangeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":20F7
         Left            =   5520
         List            =   "Form1.frx":2121
         TabIndex        =   150
         Text            =   "SetRangeParam"
         Top             =   6320
         Width           =   1215
      End
      Begin VB.ComboBox SetLaserCtrlGroupParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":217C
         Left            =   6840
         List            =   "Form1.frx":2186
         TabIndex        =   148
         Text            =   "SetLaserCtrlGroupParam"
         Top             =   5970
         Width           =   3000
      End
      Begin VB.ComboBox SetLaserCtrlGroupParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":21BC
         Left            =   5520
         List            =   "Form1.frx":21E6
         TabIndex        =   147
         Text            =   "SetLaserCtrlGroupParam"
         Top             =   5970
         Width           =   1215
      End
      Begin VB.ComboBox SetMedianParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":2241
         Left            =   6840
         List            =   "Form1.frx":2251
         TabIndex        =   145
         Text            =   "SetMedianParam"
         Top             =   5620
         Width           =   3000
      End
      Begin VB.ComboBox SetMaskParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":2296
         Left            =   6840
         List            =   "Form1.frx":22A0
         TabIndex        =   140
         Text            =   "SetMaskParam1"
         Top             =   5280
         Width           =   1000
      End
      Begin VB.TextBox SetMaskParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   1
         Left            =   9240
         TabIndex        =   142
         Text            =   "0"
         Top             =   5280
         Width           =   1215
      End
      Begin VB.TextBox SetMaskParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   0
         Left            =   7920
         TabIndex        =   141
         Text            =   "0"
         Top             =   5280
         Width           =   1215
      End
      Begin VB.ComboBox SetMedianParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":22AD
         Left            =   5520
         List            =   "Form1.frx":22D7
         TabIndex        =   144
         Text            =   "SetMedianParam"
         Top             =   5620
         Width           =   1215
      End
      Begin VB.TextBox SetNumRecoveryParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   6840
         TabIndex        =   127
         Text            =   "0"
         Top             =   3180
         Width           =   855
      End
      Begin VB.ComboBox SetNumRecoveryParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2332
         Left            =   5520
         List            =   "Form1.frx":235C
         TabIndex        =   126
         Text            =   "SetNumRecoveryParam1"
         Top             =   3180
         Width           =   1215
      End
      Begin VB.ComboBox SetBasicPointParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":23B7
         Left            =   6840
         List            =   "Form1.frx":23C1
         TabIndex        =   121
         Text            =   "SetBasicPointParam"
         Top             =   2480
         Width           =   3000
      End
      Begin VB.ComboBox SetBasicPointParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":23F1
         Left            =   5520
         List            =   "Form1.frx":241B
         TabIndex        =   120
         Text            =   "SetBasicPointParam"
         Top             =   2480
         Width           =   1215
      End
      Begin VB.ComboBox SetPanelParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":2476
         Left            =   7080
         List            =   "Form1.frx":24A4
         TabIndex        =   103
         Text            =   "SetPanelParam"
         Top             =   720
         Width           =   1455
      End
      Begin VB.ComboBox SetPanelParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":24FF
         Left            =   5520
         List            =   "Form1.frx":252D
         TabIndex        =   102
         Text            =   "SetPanelParam"
         Top             =   720
         Width           =   1455
      End
      Begin VB.ComboBox MultiParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2588
         Left            =   -65760
         List            =   "Form1.frx":2592
         TabIndex        =   73
         Text            =   "MultiParam2"
         Top             =   6000
         Width           =   1215
      End
      Begin VB.ComboBox SingleParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":259F
         Left            =   -68160
         List            =   "Form1.frx":25A9
         TabIndex        =   57
         Text            =   "SingleParam"
         Top             =   5160
         Width           =   1215
      End
      Begin VB.ComboBox SetProgramNoParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":25B6
         Left            =   -69480
         List            =   "Form1.frx":25D2
         TabIndex        =   83
         Text            =   "SetProgramNoParam1"
         Top             =   8400
         Width           =   1215
      End
      Begin VB.ComboBox SetPanelLockParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2606
         Left            =   -69480
         List            =   "Form1.frx":2610
         TabIndex        =   81
         Text            =   "SetPanelLockParam1"
         Top             =   8040
         Width           =   1215
      End
      Begin VB.CommandButton GetNumOfUsedAnalogCh 
         Caption         =   "Get number of used analog output channels (LKIF2_GetNumOfUsedAnalogCh)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   480
         Left            =   -74880
         TabIndex        =   372
         Top             =   7935
         Width           =   5295
      End
      Begin VB.CommandButton GetNumOfUsedOut 
         Caption         =   "Get number of used OUT (LKIF2_GetNumOfUsedOut)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   370
         Top             =   7590
         Width           =   5295
      End
      Begin VB.CommandButton GetNumOfUsedHeads 
         Caption         =   "Get number of used heads (LKIF2_GetNumOfUsedHeads)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   368
         Top             =   7245
         Width           =   5295
      End
      Begin VB.CommandButton GetAlarmOutputForm 
         Caption         =   "Get alarm output form (LKIF2_GetAlarmOutputForm)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   366
         Top             =   6885
         Width           =   5295
      End
      Begin VB.CommandButton GetAnalogChannel 
         Caption         =   "Get analog output channel (LKIF2_GetAnalogChannel)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   363
         Top             =   6540
         Width           =   5295
      End
      Begin VB.CommandButton GetStorageTarget 
         Caption         =   "Get storage target (LKIF2_GetStorageTarget)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   349
         Top             =   3960
         Width           =   5295
      End
      Begin VB.CommandButton GetScaling 
         Caption         =   "Get scaling (LKIF2_GetScaling)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   314
         Top             =   8880
         Width           =   5295
      End
      Begin VB.CommandButton GetOutOperation 
         Caption         =   "Get OUT to be calculated (AVE/MAX/MIN/P-P)  (LKIF2_GetOutOperation)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   480
         Left            =   -74880
         TabIndex        =   300
         Top             =   7575
         Width           =   5295
      End
      Begin VB.CommandButton GetOutAddSub 
         Caption         =   "Get OUT to be calculated (ADD/SUB) (LKIF2_GetOutAddSub)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   296
         Top             =   7230
         Width           =   5295
      End
      Begin VB.CommandButton GetCalcTarget 
         Caption         =   "Get target surface (LKIF2_GetCalcTarget)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   293
         Top             =   6885
         Width           =   5295
      End
      Begin VB.CommandButton GetCalcMethod 
         Caption         =   "Get calculation method (LKIF2_GetCalcMethod)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   289
         Top             =   6525
         Width           =   5295
      End
      Begin VB.CommandButton GetMutualInterferencePreventionGroup 
         Caption         =   "Get mutual interference prevention group (LKIF2_GetMutualInterferencePreventionGroup)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -74880
         TabIndex        =   286
         Top             =   5700
         Width           =   5295
      End
      Begin VB.CommandButton GetRange 
         Caption         =   "Get range (LKIF2_GetRange)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   283
         Top             =   5350
         Width           =   5295
      End
      Begin VB.CommandButton GetLaserCtrlGroup 
         Caption         =   "Get LASER CTRL group (LKIF2_GetLaserCtrlGroup)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   280
         Top             =   5000
         Width           =   5295
      End
      Begin VB.CommandButton GetAlarmLevel 
         Caption         =   "Get alarm level (LKIF2_GetAlarmLevel)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   266
         Top             =   3600
         Width           =   5295
      End
      Begin VB.CommandButton GetNumRecovery 
         Caption         =   "Get number of times of alarm recovery (LKIF2_GetNumRecovery)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   263
         Top             =   3250
         Width           =   5295
      End
      Begin VB.CommandButton GetBasicPoint 
         Caption         =   "Get basic point (LKIF2_GetBasicPoint)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   257
         Top             =   2550
         Width           =   5295
      End
      Begin VB.CommandButton SetMutualInterferencePreventionGroup 
         Caption         =   "Set mutual interference prevention group (LKIF2_SetMutualInterferencePreventionGroup)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   120
         TabIndex        =   152
         Top             =   6700
         Width           =   5295
      End
      Begin VB.CommandButton SetRange 
         Caption         =   "Set range (LKIF2_SetRange)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   149
         Top             =   6350
         Width           =   5295
      End
      Begin VB.CommandButton SetLaserCtrlGroup 
         Caption         =   "Set LASER CTRL group (LKIF2_SetLaserCtrlGroup)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   146
         Top             =   6000
         Width           =   5295
      End
      Begin VB.CommandButton SetNumOfUsedAnalogCh 
         Caption         =   "Set number of used analog output channels (LKIF2_SetNumOfUsedAnalogCh)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -74880
         TabIndex        =   237
         Top             =   7890
         Width           =   5295
      End
      Begin VB.CommandButton SetNumOfUsedOut 
         Caption         =   "Set number of used OUT (LKIF2_SetNumOfUsedOut)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   235
         Top             =   7545
         Width           =   5295
      End
      Begin VB.CommandButton SetNumOfUsedHeads 
         Caption         =   "Set number of used heads (LKIF2_SetNumOfUsedHeads)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   233
         Top             =   7185
         Width           =   5295
      End
      Begin VB.CommandButton SetAlarmOutputForm 
         Caption         =   "Set alarm output form (LKIF2_SetAlarmOutputForm)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   231
         Top             =   6600
         Width           =   5295
      End
      Begin VB.CommandButton SetAnalogChannel 
         Caption         =   "Set analog output channel (LKIF2_SetAnalogChannel)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   228
         Top             =   6255
         Width           =   5295
      End
      Begin VB.CommandButton SetStorageTarget 
         Caption         =   "Set storage target (LKIF2_SetStorageTarget)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   214
         Top             =   3555
         Width           =   5295
      End
      Begin VB.CommandButton SetMeasureType 
         Caption         =   "Set measurement type (LKIF2_SetMeasureType)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   208
         Top             =   2850
         Width           =   5295
      End
      Begin VB.CommandButton SetStorobeTime 
         Caption         =   "Set strobe time (LKIF2_SetStorobeTime)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   223
         Top             =   5550
         Width           =   5295
      End
      Begin VB.CommandButton SetToleranceComparatorOutputFormat 
         Caption         =   "Set comparator output format (LKIF2_SetToleranceComparatorOutputFormat)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -74880
         TabIndex        =   221
         Top             =   4965
         Width           =   5295
      End
      Begin VB.CommandButton SetMutualInterferencePrevention 
         Caption         =   "Set mutual interference prevention (LKIF2_SetMutualInterferencePrevention)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -74880
         TabIndex        =   219
         Top             =   4365
         Width           =   5295
      End
      Begin VB.CommandButton SetSynchronization 
         Caption         =   "Set synchronization (LKIF2_SetSynchronization)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   211
         Top             =   3195
         Width           =   5295
      End
      Begin VB.CommandButton SetDisplayUnit 
         Caption         =   "Set minimum display unit (LKIF2_SetDisplayUnit)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   205
         Top             =   2505
         Width           =   5295
      End
      Begin VB.CommandButton SetSamplingCycle 
         Caption         =   "Set sampling cycle (LKIF2_SetSamplingCycle)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   217
         Top             =   4020
         Width           =   5295
      End
      Begin VB.CommandButton SetCalcMode 
         Caption         =   "Set calculation mode (LKIF2_SetCalcMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   196
         Top             =   1800
         Width           =   5295
      End
      Begin VB.CommandButton SetDataStorage 
         Caption         =   "Set data storage (LKIF2_SetDataStorage)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   225
         Top             =   5895
         Width           =   5295
      End
      Begin VB.CommandButton SetAnalogScaling 
         Caption         =   "Set analog output scaling (LKIF2_SetAnalogScaling)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   199
         Top             =   2150
         Width           =   5295
      End
      Begin VB.CommandButton SetFilter 
         Caption         =   "Set filter mode (LKIF2_SetFilter)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   186
         Top             =   750
         Width           =   5295
      End
      Begin VB.CommandButton SetOffset 
         Caption         =   "Set offset (LKIF2_SetOffset)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   193
         Top             =   1450
         Width           =   5295
      End
      Begin VB.CommandButton SetTriggerMode 
         Caption         =   "Set trigger mode (LKIF2_SetTriggerMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   190
         Top             =   1100
         Width           =   5295
      End
      Begin VB.CommandButton GetStorobeTime 
         Caption         =   "Get strobe time (LKIF2_GetStrobeTime)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   358
         Top             =   5835
         Width           =   5295
      End
      Begin VB.CommandButton GetSynchronization 
         Caption         =   "Get synchronization (LKIF2_GetSynchronization)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   346
         Top             =   3615
         Width           =   5295
      End
      Begin VB.CommandButton GetMutualInterferencePrevention 
         Caption         =   "Get mutual interference prevention (LKIF2_GetMutualInterferencePrevention)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -74880
         TabIndex        =   354
         Top             =   4665
         Width           =   5295
      End
      Begin VB.CommandButton GetSamplingCycle 
         Caption         =   "Get sampling cycle (LKIF2_GetSamplingCycle)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   352
         Top             =   4305
         Width           =   5295
      End
      Begin VB.CommandButton GetCalcMode 
         Caption         =   "Get calculation mode (LKIF2_GetCalcMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   330
         Top             =   1850
         Width           =   5295
      End
      Begin VB.CommandButton GetAnalogScaling 
         Caption         =   "Get analog output scaling (LKIF2_GetAnalogScaling)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   333
         Top             =   2200
         Width           =   5295
      End
      Begin VB.CommandButton GetTriggerMode 
         Caption         =   "Get trigger mode (LKIF2_GetTriggerMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   324
         Top             =   1150
         Width           =   5295
      End
      Begin VB.CommandButton GetOffset 
         Caption         =   "Get offset (LKIF2_GetOffset)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   327
         Top             =   1500
         Width           =   5295
      End
      Begin VB.CommandButton GetMeasureType 
         Caption         =   "Get measurement type (LKIF2_GetMeasureType)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   343
         Top             =   3255
         Width           =   5295
      End
      Begin VB.CommandButton GetDataStorage 
         Caption         =   "Get data storage (LKIF2_GetDataStorage)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   360
         Top             =   6195
         Width           =   5295
      End
      Begin VB.CommandButton GetDisplayUnit 
         Caption         =   "Get minimum display unit (LKIF2_GetDisplayUnit)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   339
         Top             =   2910
         Width           =   5295
      End
      Begin VB.CommandButton GetToleranceComparatorOutputFormat 
         Caption         =   "Get comparator output format (LKIF2_GetToleranceComparatorOutputFormat)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -74880
         TabIndex        =   356
         Top             =   5250
         Width           =   5295
      End
      Begin VB.CommandButton GetFilter 
         Caption         =   "Get filter mode (LKIF2_GetFilter)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   320
         Top             =   800
         Width           =   5295
      End
      Begin VB.CommandButton StartMeasure 
         Caption         =   "Start measurement (Mode switch)(LKIF2_StartMeasure)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   374
         Top             =   750
         Width           =   5295
      End
      Begin VB.CommandButton StopMeasure 
         Caption         =   "Stop measurement (Mode switch)(LKIF2_StopMeasure)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   375
         Top             =   1080
         Width           =   5295
      End
      Begin VB.CommandButton OpenDeviceUSB 
         Caption         =   "Open USB device (LKIF2_OpenDeviceUSB)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   376
         Top             =   750
         Width           =   5295
      End
      Begin VB.CommandButton OpenDeviceETHER 
         Caption         =   "Open Ethernet device (LKIF2_OpenDeviceETHER)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   377
         Top             =   1100
         Width           =   5295
      End
      Begin VB.CommandButton CloseDevice 
         Caption         =   "Close device (LKIF2_CloseDevice)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   382
         Top             =   1680
         Width           =   5295
      End
      Begin VB.TextBox OpenDeviceETHERParam1 
         Alignment       =   1  '右揃え
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         Left            =   -69000
         MaxLength       =   3
         TabIndex        =   378
         Text            =   "192"
         Top             =   1095
         Width           =   495
      End
      Begin VB.TextBox OpenDeviceETHERParam1 
         Alignment       =   1  '右揃え
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         Left            =   -68400
         MaxLength       =   3
         TabIndex        =   379
         Text            =   "168"
         Top             =   1095
         Width           =   495
      End
      Begin VB.TextBox OpenDeviceETHERParam1 
         Alignment       =   1  '右揃え
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   2
         Left            =   -67800
         MaxLength       =   3
         TabIndex        =   380
         Text            =   "10"
         Top             =   1095
         Width           =   495
      End
      Begin VB.TextBox OpenDeviceETHERParam1 
         Alignment       =   1  '右揃え
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   3
         Left            =   -67200
         MaxLength       =   3
         TabIndex        =   381
         Text            =   "10"
         Top             =   1095
         Width           =   495
      End
      Begin VB.ComboBox GetNumAlarmParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":261D
         Left            =   -69480
         List            =   "Form1.frx":2647
         TabIndex        =   261
         Text            =   "GetNumAlarmParam1"
         Top             =   2880
         Width           =   1215
      End
      Begin VB.ComboBox GetMeasureModeParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":26A2
         Left            =   -69480
         List            =   "Form1.frx":26CC
         TabIndex        =   255
         Text            =   "GetMeasureModeParam1"
         Top             =   2180
         Width           =   1215
      End
      Begin VB.ComboBox GetAbleMinMaxParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2727
         Left            =   -69480
         List            =   "Form1.frx":2751
         TabIndex        =   251
         Text            =   "GetAbleMinMaxParam1"
         Top             =   1820
         Width           =   1215
      End
      Begin VB.ComboBox GetAbleModeParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":27AC
         Left            =   -69480
         List            =   "Form1.frx":27D6
         TabIndex        =   248
         Text            =   "GetAbleModeParam1"
         Top             =   1480
         Width           =   1215
      End
      Begin VB.ComboBox GetToleranceParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2831
         Left            =   -69480
         List            =   "Form1.frx":285B
         TabIndex        =   243
         Text            =   "GetToleranceParam1"
         Top             =   1120
         Width           =   1215
      End
      Begin VB.CommandButton GetMedian 
         Caption         =   "Get median (LKIF2_GetMedian)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   277
         Top             =   4650
         Width           =   5295
      End
      Begin VB.CommandButton GetMask 
         Caption         =   "Get mask (LKIF2_GetMask)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   272
         Top             =   4300
         Width           =   5295
      End
      Begin VB.CommandButton GetAbleMinMax 
         Caption         =   "Get ABLE control range (LKIF2_GetAbleMinMax)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   250
         Top             =   1850
         Width           =   5295
      End
      Begin VB.CommandButton GetAbleMode 
         Caption         =   "Get ABLE (LKIF2_GetAbleMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   247
         Top             =   1500
         Width           =   5295
      End
      Begin VB.CommandButton GetPanel 
         Caption         =   "Display panel check (LKIF2_GetPanel)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   239
         Top             =   800
         Width           =   5295
      End
      Begin VB.CommandButton GetTolerance 
         Caption         =   "Get tolerance (LKIF2_GetTolerance)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   242
         Top             =   1150
         Width           =   5295
      End
      Begin VB.CommandButton GetNumAlarm 
         Caption         =   "Get number of times of alarm processing (LKIF2_GetNumAlarm)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   260
         Top             =   2900
         Width           =   5295
      End
      Begin VB.CommandButton GetReflectionMode 
         Caption         =   "Get mounting mode (LKIF2_GetReflectionMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   269
         Top             =   3950
         Width           =   5295
      End
      Begin VB.CommandButton GetMeasureMode 
         Caption         =   "Get measurement mode (LKIF2_GetMeasureMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   254
         Top             =   2200
         Width           =   5295
      End
      Begin VB.CommandButton SetOutOperation 
         Caption         =   "Set OUT to be calculated (AVE/MAX/MIN/P-P) (LKIF2_SetOutOperation)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   120
         TabIndex        =   165
         Top             =   8520
         Width           =   5295
      End
      Begin VB.CommandButton SetOutAddSub 
         Caption         =   "Set OUT to be calculated (ADD/SUB) (LKIF2_SetOutAddSub)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   161
         Top             =   8040
         Width           =   5295
      End
      Begin VB.CommandButton SetCalcTarget 
         Caption         =   "Set target surface (LKIF2_SetCalcTarget)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   158
         Top             =   7680
         Width           =   5295
      End
      Begin VB.CommandButton SetMedian 
         Caption         =   "Set median (LKIF2_SetMedian)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   143
         Top             =   5650
         Width           =   5295
      End
      Begin VB.CommandButton SetMask 
         Caption         =   "Set mask (LKIF2_SetMask)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   138
         Top             =   5300
         Width           =   5295
      End
      Begin VB.CommandButton SetNumRecovery 
         Caption         =   "Set number of times of alarm recovery(LKIF2_SetNumRecovery)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   125
         Top             =   3200
         Width           =   5295
      End
      Begin VB.CommandButton SetBasicPoint 
         Caption         =   "Set basic point(LKIF2_SetBasicPoint)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   119
         Top             =   2500
         Width           =   5295
      End
      Begin VB.CheckBox checkSaveGetLight 
         Caption         =   "save in CSV format"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -66720
         TabIndex        =   100
         Top             =   11160
         Value           =   1  'ﾁｪｯｸ
         Width           =   2055
      End
      Begin VB.CheckBox checkSaveDataStorageGetData 
         Caption         =   "save in CSV format"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -64560
         TabIndex        =   93
         Top             =   10080
         Value           =   1  'ﾁｪｯｸ
         Width           =   2055
      End
      Begin VB.ComboBox GetLightParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":28AA
         Left            =   -69480
         List            =   "Form1.frx":28D4
         TabIndex        =   98
         Text            =   "GetLightParam"
         Top             =   11145
         Width           =   1215
      End
      Begin VB.ComboBox SingleParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":292F
         Left            =   -69480
         List            =   "Form1.frx":2959
         TabIndex        =   56
         Text            =   "SingleParam"
         Top             =   5160
         Width           =   1215
      End
      Begin VB.CommandButton SetReset 
         Caption         =   "Measured value reset (single)(LKIF2_SetReset)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -74880
         TabIndex        =   58
         Top             =   5520
         Width           =   5300
      End
      Begin VB.CommandButton SetZero 
         Caption         =   "Auto-zero ON/OFF (single)(LKIF2_SetZeroSingle)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -74880
         TabIndex        =   55
         Top             =   5160
         Width           =   5300
      End
      Begin VB.CommandButton SetTiming 
         Caption         =   "Timing ON/OFF (single)(LKIF2_SetTimingSingle)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -74880
         TabIndex        =   54
         Top             =   4800
         Width           =   5300
      End
      Begin VB.CommandButton SetReset 
         Caption         =   "Measured value reset (synchronous)(LKIF2_SetResetSync)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -74880
         TabIndex        =   79
         Top             =   7680
         Width           =   5300
      End
      Begin VB.CommandButton SetReset 
         Caption         =   "Measured value reset (multiple)(LKIF2_SetResetMulti)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -74880
         TabIndex        =   75
         Top             =   6600
         Width           =   5300
      End
      Begin VB.CommandButton SetZero 
         Caption         =   "Auto-zero ON/OFF (synchronous)(LKIF2_SetZeroSync)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -74880
         TabIndex        =   77
         Top             =   7320
         Width           =   5300
      End
      Begin VB.CommandButton SetZero 
         Caption         =   "Auto-zero ON/OFF (multiple)(LKIF2_SetZeroMulti)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -74880
         TabIndex        =   74
         Top             =   6240
         Width           =   5300
      End
      Begin VB.Frame ParamFrame 
         Caption         =   "MultiParam1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   1215
         Index           =   2
         Left            =   -69480
         TabIndex        =   60
         Top             =   5760
         Width           =   3615
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT12"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   11
            Left            =   2640
            TabIndex        =   72
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT11"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   10
            Left            =   1800
            TabIndex        =   71
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT10"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   9
            Left            =   960
            TabIndex        =   70
            Top             =   960
            Width           =   855
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT9"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   8
            Left            =   120
            TabIndex        =   69
            Top             =   960
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT4"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   3
            Left            =   2640
            TabIndex        =   64
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT8"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   7
            Left            =   2640
            TabIndex        =   68
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT7"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   6
            Left            =   1800
            TabIndex        =   67
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT6"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   5
            Left            =   960
            TabIndex        =   66
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT5"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   4
            Left            =   120
            TabIndex        =   65
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT3"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   2
            Left            =   1800
            TabIndex        =   63
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT2"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   1
            Left            =   960
            TabIndex        =   62
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox MultiParam1 
            Caption         =   "OUT1"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   0
            Left            =   120
            TabIndex        =   61
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.CommandButton SetTiming 
         Caption         =   "Timing ON/OFF (synchronous)(LKIF2_SetTimingSync)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -74880
         TabIndex        =   76
         Top             =   6960
         Width           =   5300
      End
      Begin VB.CommandButton SetTiming 
         Caption         =   "Timing ON/OFF (multiple)(LKIF2_SetTimingMulti)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -74880
         TabIndex        =   59
         Top             =   5880
         Width           =   5300
      End
      Begin VB.Frame ParamFrame 
         Caption         =   "GetCalcDataAllParam"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   1695
         Index           =   1
         Left            =   -69480
         TabIndex        =   29
         Top             =   3000
         Width           =   7575
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT12"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   11
            Left            =   5160
            TabIndex        =   52
            Top             =   1320
            Width           =   525
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT11"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   10
            Left            =   2760
            TabIndex        =   50
            Top             =   1320
            Width           =   525
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT10"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   9
            Left            =   360
            TabIndex        =   48
            Top             =   1320
            Width           =   525
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT9"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   8
            Left            =   5160
            TabIndex        =   46
            Top             =   960
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT8"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   7
            Left            =   2760
            TabIndex        =   44
            Top             =   960
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT7"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   6
            Left            =   360
            TabIndex        =   42
            Top             =   960
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT6"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   5
            Left            =   5160
            TabIndex        =   40
            Top             =   600
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT5"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   4
            Left            =   2760
            TabIndex        =   38
            Top             =   600
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT4"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   3
            Left            =   360
            TabIndex        =   36
            Top             =   600
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT3"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   2
            Left            =   5160
            TabIndex        =   34
            Top             =   240
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT2"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   1
            Left            =   2760
            TabIndex        =   32
            Top             =   240
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam1 
            Caption         =   "OUT1"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   0
            Left            =   360
            TabIndex        =   30
            Top             =   240
            Width           =   495
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value1"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   0
            Left            =   1080
            TabIndex        =   31
            Top             =   240
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value2"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   1
            Left            =   3480
            TabIndex        =   33
            Top             =   240
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value3"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   2
            Left            =   5880
            TabIndex        =   35
            Top             =   240
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value6"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   5
            Left            =   5880
            TabIndex        =   41
            Top             =   600
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value11"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   10
            Left            =   3480
            TabIndex        =   51
            Top             =   1320
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value5"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   4
            Left            =   3480
            TabIndex        =   39
            Top             =   600
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value10"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   9
            Left            =   1080
            TabIndex        =   49
            Top             =   1320
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value9"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   8
            Left            =   5880
            TabIndex        =   47
            Top             =   960
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value7"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   6
            Left            =   1080
            TabIndex        =   43
            Top             =   960
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value4"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   3
            Left            =   1080
            TabIndex        =   37
            Top             =   600
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value8"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   7
            Left            =   3480
            TabIndex        =   45
            Top             =   960
            Width           =   1455
         End
         Begin VB.Label GetCalcDataAllParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value12"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   11
            Left            =   5880
            TabIndex        =   53
            Top             =   1320
            Width           =   1455
         End
      End
      Begin VB.CommandButton GetCalcData 
         Caption         =   "Measured value output (ALL)(LKIF2_GetCalcDataAll)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -74880
         TabIndex        =   28
         Top             =   3120
         Width           =   5300
      End
      Begin VB.Frame ParamFrame 
         Caption         =   "GetCalcDataMultiParam"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   1695
         Index           =   0
         Left            =   -69480
         TabIndex        =   4
         Top             =   1200
         Width           =   7575
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT12"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   11
            Left            =   5040
            TabIndex        =   26
            Top             =   1320
            Width           =   855
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT11"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   10
            Left            =   2640
            TabIndex        =   24
            Top             =   1320
            Width           =   855
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT10"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   9
            Left            =   120
            TabIndex        =   22
            Top             =   1320
            Width           =   855
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT9"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   8
            Left            =   5040
            TabIndex        =   20
            Top             =   960
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT4"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   3
            Left            =   120
            TabIndex        =   11
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT8"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   7
            Left            =   2640
            TabIndex        =   18
            Top             =   960
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT7"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   6
            Left            =   120
            TabIndex        =   16
            Top             =   960
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT6"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   5
            Left            =   5040
            TabIndex        =   15
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT5"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   4
            Left            =   2640
            TabIndex        =   13
            Top             =   600
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT3"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   2
            Left            =   5040
            TabIndex        =   9
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT2"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   1
            Left            =   2640
            TabIndex        =   7
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox GetCalcDataMultiParam1 
            Caption         =   "OUT1"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   180
            Index           =   0
            Left            =   120
            TabIndex        =   5
            Top             =   240
            Width           =   735
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value5"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   4
            Left            =   3480
            TabIndex        =   14
            Top             =   600
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value12"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   11
            Left            =   5880
            TabIndex        =   27
            Top             =   1320
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value8"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   7
            Left            =   3480
            TabIndex        =   19
            Top             =   960
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value4"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   3
            Left            =   1080
            TabIndex        =   12
            Top             =   600
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value7"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   6
            Left            =   1080
            TabIndex        =   17
            Top             =   960
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value9"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   8
            Left            =   5880
            TabIndex        =   21
            Top             =   960
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value10"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   9
            Left            =   1080
            TabIndex        =   23
            Top             =   1320
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value11"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   10
            Left            =   3480
            TabIndex        =   25
            Top             =   1320
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value6"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   5
            Left            =   5880
            TabIndex        =   386
            Top             =   600
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value3"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   2
            Left            =   5880
            TabIndex        =   10
            Top             =   240
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value2"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   1
            Left            =   3480
            TabIndex        =   8
            Top             =   240
            Width           =   1455
         End
         Begin VB.Label GetCalcDataMultiParam2 
            BorderStyle     =   1  '実線
            Caption         =   "Value1"
            BeginProperty Font 
               Name            =   "Tahoma"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Index           =   0
            Left            =   1080
            TabIndex        =   6
            Top             =   240
            Width           =   1455
         End
      End
      Begin VB.ComboBox GetCalcDataSingleParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":29A8
         Left            =   -69480
         List            =   "Form1.frx":29D2
         TabIndex        =   2
         Text            =   "GetCalcDataSingleParam1"
         Top             =   760
         Width           =   1215
      End
      Begin VB.CommandButton GetLight 
         Caption         =   "Get received light waveform(LKIF2_GetLight)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   97
         Top             =   11160
         Width           =   5300
      End
      Begin VB.ComboBox DataStorageGetDataParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2A21
         Left            =   -69480
         List            =   "Form1.frx":2A4B
         TabIndex        =   90
         Text            =   "DataStorageGetDataParam1"
         Top             =   10200
         Width           =   1215
      End
      Begin VB.CommandButton DataStorageGetStatus 
         Caption         =   "Data storage accumulation status output(LKIF2_DataStorageGetStatus)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   -74880
         TabIndex        =   94
         Top             =   10560
         Width           =   5300
      End
      Begin VB.CommandButton DataStorageGetData 
         Caption         =   "Data storage output(LKIF2_DataStorageGetData)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   89
         Top             =   10200
         Width           =   5300
      End
      Begin VB.CommandButton DataStorageInit 
         Caption         =   "Initialize data storage(LKIF2_DataStorageInit)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   88
         Top             =   9840
         Width           =   5300
      End
      Begin VB.CommandButton DataStorageStop 
         Caption         =   "Stop data storage(LKIF2_DataStorageStop)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   87
         Top             =   9480
         Width           =   5300
      End
      Begin VB.CommandButton DataStorageStart 
         Caption         =   "Start data storage(LKIF2_DataStorageStart)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   86
         Top             =   9120
         Width           =   5300
      End
      Begin VB.CommandButton GetCalcData 
         Caption         =   "Measured value output (single)(LKIF2_GetCalcDataSingle)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -74880
         TabIndex        =   1
         Top             =   800
         Width           =   5300
      End
      Begin VB.CommandButton SetPanelLock 
         Caption         =   "Panel lock(LKIF2_SetPanelLock)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   80
         Top             =   8040
         Width           =   5300
      End
      Begin VB.CommandButton GetProgramNo 
         Caption         =   "Program check(LKIF2_GetProgramNo)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   84
         Top             =   8760
         Width           =   5300
      End
      Begin VB.CommandButton SetProgramNo 
         Caption         =   "Program change(LKIF2_SetProgramNo)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -74880
         TabIndex        =   82
         Top             =   8400
         Width           =   5300
      End
      Begin VB.TextBox SetToleranceParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   2
         Left            =   9480
         TabIndex        =   108
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.TextBox SetToleranceParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   1
         Left            =   8160
         TabIndex        =   107
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.ComboBox SetToleranceParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2A9A
         Left            =   5520
         List            =   "Form1.frx":2AC4
         TabIndex        =   105
         Text            =   "SetToleranceParam1"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.TextBox SetToleranceParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   0
         Left            =   6840
         TabIndex        =   106
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.TextBox SetNumAlarmParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   6840
         TabIndex        =   124
         Text            =   "0"
         Top             =   2830
         Width           =   855
      End
      Begin VB.ComboBox SetNumAlarmParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2B13
         Left            =   5520
         List            =   "Form1.frx":2B3D
         TabIndex        =   123
         Text            =   "SetNumAlarmParam1"
         Top             =   2830
         Width           =   1215
      End
      Begin VB.CommandButton SetTolerance 
         Caption         =   "Set tolerance(LKIF2_SetTolerance)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   104
         Top             =   1100
         Width           =   5295
      End
      Begin VB.CommandButton SetNumAlarm 
         Caption         =   "Set number of times of alarm processing(LKIF2_SetNumAlarm)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   122
         Top             =   2850
         Width           =   5295
      End
      Begin VB.CommandButton SetScaling 
         Caption         =   "Set scaling (LKIF2_SetScaling)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   180
         Top             =   9765
         Width           =   5295
      End
      Begin VB.ComboBox SetReflectionModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":2B98
         Left            =   5520
         List            =   "Form1.frx":2BC2
         TabIndex        =   136
         Text            =   "SetReflectionModeParam"
         Top             =   4920
         Width           =   1215
      End
      Begin VB.ComboBox SetMeasureModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":2C1D
         Left            =   5520
         List            =   "Form1.frx":2C47
         TabIndex        =   117
         Text            =   "SetMeasureModeParam"
         Top             =   2120
         Width           =   1215
      End
      Begin VB.ComboBox SetAlarmLevelParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2CA2
         Left            =   5520
         List            =   "Form1.frx":2CCC
         TabIndex        =   129
         Text            =   "SetAlarmLevelParam1"
         Top             =   3520
         Width           =   1215
      End
      Begin VB.TextBox SetAlarmLevelParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Left            =   6840
         TabIndex        =   130
         Text            =   "4"
         Top             =   3520
         Width           =   855
      End
      Begin VB.ComboBox SetMeasureModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":2D27
         Left            =   6840
         List            =   "Form1.frx":2D3A
         TabIndex        =   118
         Text            =   "SetMeasureModeParam"
         Top             =   2120
         Width           =   3000
      End
      Begin VB.ComboBox SetReflectionModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":2DBB
         Left            =   6840
         List            =   "Form1.frx":2DC5
         TabIndex        =   137
         Text            =   "SetReflectionModeParam"
         Top             =   4920
         Width           =   3360
      End
      Begin VB.CommandButton SetAlarmLevel 
         Caption         =   "Set alarm level (LKIF2_SetAlarmLevel)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   128
         Top             =   3550
         Width           =   5295
      End
      Begin VB.CommandButton SetMeasureMode 
         Caption         =   "Set measurement mode(LKIF2_SetMeasureMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   116
         Top             =   2150
         Width           =   5295
      End
      Begin VB.CommandButton SetReflectionMode 
         Caption         =   "Set mounting mode (LKIF2_SetReflectionMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   135
         Top             =   4950
         Width           =   5295
      End
      Begin VB.CommandButton SetPanel 
         Caption         =   "Display panel switch(LKIF2_SetPanel)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   101
         Top             =   750
         Width           =   5295
      End
      Begin VB.CommandButton SetAbleMode 
         Caption         =   "Set ABLE(LKIF2_SetAbleMode)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   109
         Top             =   1450
         Width           =   5295
      End
      Begin VB.CommandButton SetAbleMinMax 
         Caption         =   "Set ABLE control range(LKIF2_SetAbleMinMax)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   112
         Top             =   1800
         Width           =   5295
      End
      Begin VB.CommandButton AbleStart 
         Caption         =   "Start ABLE calibration (LKIF2_AbleStart)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   131
         Top             =   3900
         Width           =   5295
      End
      Begin VB.CommandButton AbleStop 
         Caption         =   "Finish ABLE calibration (LKIF2_AbleStop)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   133
         Top             =   4250
         Width           =   5295
      End
      Begin VB.CommandButton AbleCancel 
         Caption         =   "Cancel ABLE calibration (LKIF2_AbleCancel)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   134
         Top             =   4600
         Width           =   5295
      End
      Begin VB.CommandButton SetCalcMethod 
         Caption         =   "Set calculation method (LKIF2_SetCalcMethod)"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   155
         Top             =   7320
         Width           =   5295
      End
      Begin VB.ComboBox SetAbleModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":2E04
         Left            =   5520
         List            =   "Form1.frx":2E2E
         TabIndex        =   110
         Text            =   "SetAbleModeParam"
         Top             =   1420
         Width           =   1215
      End
      Begin VB.ComboBox SetAbleMinMaxParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2E89
         Left            =   5520
         List            =   "Form1.frx":2EB3
         TabIndex        =   113
         Text            =   "SetAbleMinMaxParam1"
         Top             =   1760
         Width           =   1215
      End
      Begin VB.ComboBox SetAbleModeParam 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   1
         ItemData        =   "Form1.frx":2F0E
         Left            =   6840
         List            =   "Form1.frx":2F18
         TabIndex        =   111
         Text            =   "SetAbleModeParam"
         Top             =   1420
         Width           =   3000
      End
      Begin VB.TextBox SetAbleMinMaxParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   0
         Left            =   6840
         TabIndex        =   114
         Text            =   "0"
         Top             =   1800
         Width           =   615
      End
      Begin VB.TextBox SetAbleMinMaxParam2 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   285
         Index           =   1
         Left            =   7560
         TabIndex        =   115
         Text            =   "0"
         Top             =   1800
         Width           =   615
      End
      Begin VB.ComboBox AbleStartParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         ItemData        =   "Form1.frx":2F46
         Left            =   5520
         List            =   "Form1.frx":2F70
         TabIndex        =   132
         Text            =   "AbleStartParam1"
         Top             =   3900
         Width           =   1215
      End
      Begin VB.ComboBox SetMaskParam1 
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Index           =   0
         ItemData        =   "Form1.frx":2FCB
         Left            =   5520
         List            =   "Form1.frx":2FF5
         TabIndex        =   139
         Text            =   "SetMaskParam1"
         Top             =   5280
         Width           =   1215
      End
      Begin MSComDlg.CommonDialog CommonDialogForLight 
         Left            =   -63480
         Top             =   9000
         _ExtentX        =   847
         _ExtentY        =   847
         _Version        =   393216
         FileName        =   "LightLog.csv"
         Filter          =   "CSV|*.csv|"
      End
      Begin VB.Label GetNumOfUsedAnalogChParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   373
         Top             =   8040
         Width           =   2205
      End
      Begin VB.Label GetNumOfUsedOutParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   371
         Top             =   7590
         Width           =   2205
      End
      Begin VB.Label GetNumOfUsedHeadsParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   369
         Top             =   7245
         Width           =   2205
      End
      Begin VB.Label GetAlarmOutputFormParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   367
         Top             =   6885
         Width           =   3405
      End
      Begin VB.Label GetAnalogChannelParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -67200
         TabIndex        =   365
         Top             =   6540
         Width           =   2205
      End
      Begin VB.Label GetDataStorageParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -67200
         TabIndex        =   362
         Top             =   6180
         Width           =   3405
      End
      Begin VB.Label GetDataStorageParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -69480
         TabIndex        =   361
         Top             =   6180
         Width           =   2205
      End
      Begin VB.Label GetStorobeTimeParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   359
         Top             =   5835
         Width           =   3405
      End
      Begin VB.Label GetToleranceComparatorOutputFormatParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   357
         Top             =   5400
         Width           =   5685
      End
      Begin VB.Label GetMutualInterferencePreventionParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   355
         Top             =   4800
         Width           =   4725
      End
      Begin VB.Label GetSamplingCycleParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   353
         Top             =   4320
         Width           =   3405
      End
      Begin VB.Label GetStorageTargetParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   351
         Top             =   3960
         Width           =   2200
      End
      Begin VB.Label GetSynchronizationParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   348
         Top             =   3600
         Width           =   2200
      End
      Begin VB.Label GetMeasureTypeParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   345
         Top             =   3240
         Width           =   3400
      End
      Begin VB.Label GetDisplayUnitParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   342
         Top             =   2925
         Width           =   3400
      End
      Begin VB.Label GetAnalogScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value4"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   3
         Left            =   -65880
         TabIndex        =   338
         Top             =   2520
         Width           =   2200
      End
      Begin VB.Label GetAnalogScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value3"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -68160
         TabIndex        =   337
         Top             =   2520
         Width           =   2200
      End
      Begin VB.Label GetAnalogScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -65880
         TabIndex        =   336
         Top             =   2205
         Width           =   2200
      End
      Begin VB.Label GetAnalogScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -68160
         TabIndex        =   335
         Top             =   2205
         Width           =   2200
      End
      Begin VB.Label GetCalcModeParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   332
         Top             =   1845
         Width           =   3400
      End
      Begin VB.Label GetOffsetParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   329
         Top             =   1500
         Width           =   2200
      End
      Begin VB.Label GetTriggerModeParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   326
         Top             =   1140
         Width           =   3400
      End
      Begin VB.Label GetFilterParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -64680
         TabIndex        =   323
         Top             =   765
         Width           =   3165
      End
      Begin VB.Label GetFilterParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -68160
         TabIndex        =   322
         Top             =   765
         Width           =   3400
      End
      Begin VB.Label GetScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value4"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   3
         Left            =   -65880
         TabIndex        =   319
         Top             =   9240
         Width           =   2205
      End
      Begin VB.Label GetScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value3"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -68160
         TabIndex        =   318
         Top             =   9240
         Width           =   2205
      End
      Begin VB.Label GetScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -65880
         TabIndex        =   317
         Top             =   8880
         Width           =   2205
      End
      Begin VB.Label GetScalingParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -68160
         TabIndex        =   316
         Top             =   8880
         Width           =   2205
      End
      Begin VB.Label GetOutAddSubParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -65880
         TabIndex        =   299
         Top             =   7200
         Width           =   2205
      End
      Begin VB.Label GetOutAddSubParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -68160
         TabIndex        =   298
         Top             =   7200
         Width           =   2205
      End
      Begin VB.Label GetCalcTargetParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   295
         Top             =   6840
         Width           =   3000
      End
      Begin VB.Label GetCalcMethodParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -65040
         TabIndex        =   292
         Top             =   6510
         Width           =   2040
      End
      Begin VB.Label GetCalcMethodParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -68160
         TabIndex        =   291
         Top             =   6510
         Width           =   3000
      End
      Begin VB.Label GetMutualInterferencePreventionGroupParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   288
         Top             =   5760
         Width           =   4800
      End
      Begin VB.Label GetRangeParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   285
         Top             =   5355
         Width           =   3000
      End
      Begin VB.Label GetLaserCtrlGroupParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   282
         Top             =   4995
         Width           =   3000
      End
      Begin VB.Label GetMedianParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   279
         Top             =   4650
         Width           =   3000
      End
      Begin VB.Label GetMaskParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value3"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -63600
         TabIndex        =   276
         Top             =   4320
         Width           =   2100
      End
      Begin VB.Label GetMaskParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -65880
         TabIndex        =   275
         Top             =   4320
         Width           =   2205
      End
      Begin VB.Label GetMaskParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -68160
         TabIndex        =   274
         Top             =   4320
         Width           =   2200
      End
      Begin VB.Label GetReflectionModeParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   271
         Top             =   3960
         Width           =   3360
      End
      Begin VB.Label GetAlarmLevelParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   268
         Top             =   3630
         Width           =   2200
      End
      Begin VB.Label GetNumRecoveryParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   265
         Top             =   3280
         Width           =   2200
      End
      Begin VB.Label GetBasicPointParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   259
         Top             =   2565
         Width           =   3400
      End
      Begin VB.Label GetPanelParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -67200
         TabIndex        =   241
         Top             =   780
         Width           =   2205
      End
      Begin VB.Label OpenDeviceETHERParamSub 
         Alignment       =   1  '右揃え
         Caption         =   "IP:"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -69360
         TabIndex        =   390
         Top             =   1125
         Width           =   255
      End
      Begin VB.Label OpenDeviceETHERParamSub 
         Alignment       =   1  '右揃え
         Caption         =   "."
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   375
         Index           =   1
         Left            =   -68520
         TabIndex        =   389
         Top             =   1245
         Width           =   135
      End
      Begin VB.Label OpenDeviceETHERParamSub 
         Alignment       =   1  '右揃え
         Caption         =   "."
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   375
         Index           =   2
         Left            =   -67920
         TabIndex        =   388
         Top             =   1245
         Width           =   135
      End
      Begin VB.Label OpenDeviceETHERParamSub 
         Alignment       =   1  '右揃え
         Caption         =   "."
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   375
         Index           =   3
         Left            =   -67320
         TabIndex        =   387
         Top             =   1245
         Width           =   135
      End
      Begin VB.Label GetPanelParam 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -69480
         TabIndex        =   240
         Top             =   780
         Width           =   2200
      End
      Begin VB.Label GetNumAlarmParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   262
         Top             =   2920
         Width           =   2200
      End
      Begin VB.Label GetMeasureModeParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   256
         Top             =   2220
         Width           =   3400
      End
      Begin VB.Label GetAbleMinMaxParam3 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -65880
         TabIndex        =   253
         Top             =   1875
         Width           =   2200
      End
      Begin VB.Label GetAbleMinMaxParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   252
         Top             =   1875
         Width           =   2200
      End
      Begin VB.Label GetAbleModeParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68160
         TabIndex        =   249
         Top             =   1515
         Width           =   3400
      End
      Begin VB.Label GetToleranceParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value3"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   2
         Left            =   -63600
         TabIndex        =   246
         Top             =   1155
         Width           =   2100
      End
      Begin VB.Label GetToleranceParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value2"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   1
         Left            =   -65880
         TabIndex        =   245
         Top             =   1155
         Width           =   2200
      End
      Begin VB.Label GetToleranceParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Index           =   0
         Left            =   -68160
         TabIndex        =   244
         Top             =   1155
         Width           =   2200
      End
      Begin VB.Label DataStorageGetStatusParam2 
         BorderStyle     =   1  '実線
         Caption         =   "num"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -67440
         TabIndex        =   96
         Top             =   10680
         Width           =   1815
      End
      Begin VB.Label DataStorageGetStatusParam1 
         BorderStyle     =   1  '実線
         Caption         =   "status"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   95
         Top             =   10680
         Width           =   1935
      End
      Begin VB.Label DataStorageGetDataParam3 
         BorderStyle     =   1  '実線
         Caption         =   "num"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -66480
         TabIndex        =   92
         Top             =   10200
         Width           =   1815
      End
      Begin VB.Label GetCalcDataSingleParam2 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -68040
         TabIndex        =   3
         Top             =   795
         Width           =   1935
      End
      Begin VB.Label GetProgramNoParam1 
         BorderStyle     =   1  '実線
         Caption         =   "Value1"
         BeginProperty Font 
            Name            =   "Tahoma"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   -69480
         TabIndex        =   85
         Top             =   8760
         Width           =   1935
      End
   End
   Begin VB.Label ResultText 
      Caption         =   "Result"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   385
      Top             =   11880
      Width           =   9495
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private Declare Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteA" (ByVal hwnd As Long, ByVal lpOperation As String, ByVal lpFile As String, ByVal lpParameters As String, ByVal lpDirectory As String, ByVal nShowCmd As Long) As Long
Private Declare Function inet_addr Lib "wsock32.dll" (ByVal cp As String) As Long
Private Const SW_SHOW As Long = 5
Private Const MAX_OUT  As Long = 12
Private Const WAVEDATA_MAX As Long = 2048
Private Const PEEK_MAX As Long = 4
Private Const FILTER_PARAM_MAX As Long = 10
Private Const DISPLAYUNIT_MAX As Long = 7
Private FILETER_PARAM_STRINGS() As Variant
Private DISPLAY_UNIT_PARAM_STRINGS() As Variant

Private Sub ResetListForDisplayUnitParam()
Dim measureType As Long
Dim cnt As Long
    
    measureType = SetMeasureTypeParam(1).ItemData(SetMeasureTypeParam(1).ListIndex)
    For cnt = 0 To DISPLAYUNIT_MAX - 1
        SetDisplayUnitParam(1).List(cnt) = DISPLAY_UNIT_PARAM_STRINGS(measureType)(cnt)
    Next
End Sub

Private Function GetDisplayUnitParamString(ByVal measureType As LKIF_MEASURETYPE, displayUnitParam As LKIF_DISPLAYUNIT) As String
    GetDisplayUnitParamString = DISPLAY_UNIT_PARAM_STRINGS(measureType)(displayUnitParam)
End Function

Private Sub ResetListForFileterParam()
Dim filterMode As Long
Dim cnt As Long
    
    filterMode = 0
    If SetFilterParam(1).ItemData(SetFilterParam(1).ListIndex) > 0 Then
        filterMode = 1
    End If
    
    For cnt = 0 To FILTER_PARAM_MAX - 1
           SetFilterParam(2).List(cnt) = FILETER_PARAM_STRINGS(filterMode)(cnt)
    Next
End Sub

Private Function GetFileterParamString(ByVal filterMode As LKIF_FILTERMODE, ByVal filterParam As LKIF_FILTERPARA) As String

    If filterMode > LKIF_FILTERMODE_LOWPASS Then
         filterMode = LKIF_FILTERMODE_LOWPASS
    End If
    
    GetFileterParamString = FILETER_PARAM_STRINGS(filterMode)(filterParam)
End Function

Private Sub FuncQuit(ByVal funcName As String, ByVal returnCode As RC)
   If IsSuccess(returnCode) Then
        Call ShowSuccessMsg(funcName, returnCode)
   Else
        Call ShowFailureMsg(funcName, returnCode)
   End If
End Sub

Private Function IsSuccess(ByVal returnCode As RC) As Boolean
     IsSuccess = False
     If returnCode = RC_OK Then
        IsSuccess = True
     End If
End Function

Private Sub ShowSuccessMsg(ByVal funcName As String, ByVal returnCode As RC)
    ResultText = funcName & " is succeeded. RC = 0x" & Hex(returnCode)
End Sub

Private Sub ShowFailureMsg(ByVal funcName As String, ByVal returnCode As RC)
    Dim MessageText As String
    MessageText = "Failed in " & funcName & ". RC = 0x" & Hex(returnCode)
    If IsDisplayMessageBox.value = 1 Then
        MsgBox MessageText
    End If
    ResultText = MessageText
End Sub

Private Function MakeOutNoFromCheckBoxes(checkBoxes As Variant) As LKIF_OUTNO
    Dim checkBox As checkBox
    Dim OutNoTable() As Variant
    OutNoTable = Array(LKIF_OUTNO_01, LKIF_OUTNO_02, LKIF_OUTNO_03, LKIF_OUTNO_04, _
                       LKIF_OUTNO_05, LKIF_OUTNO_06, LKIF_OUTNO_07, LKIF_OUTNO_08, _
                       LKIF_OUTNO_09, LKIF_OUTNO_10, LKIF_OUTNO_11, LKIF_OUTNO_12)
    
    For Each checkBox In checkBoxes
        If checkBox.value = 1 Then
            MakeOutNoFromCheckBoxes = MakeOutNoFromCheckBoxes Or OutNoTable(checkBox.Index)
        End If
    Next
End Function

Private Sub SetCheckBoxesForOutNo(checkBoxes As Variant, outNo As LKIF_OUTNO)
Dim checkBox As checkBox
    Dim temp As LKIF_OUTNO
    Dim OutNoTable() As Variant
    OutNoTable = Array(LKIF_OUTNO_01, LKIF_OUTNO_02, LKIF_OUTNO_03, LKIF_OUTNO_04, _
                       LKIF_OUTNO_05, LKIF_OUTNO_06, LKIF_OUTNO_07, LKIF_OUTNO_08, _
                       LKIF_OUTNO_09, LKIF_OUTNO_10, LKIF_OUTNO_11, LKIF_OUTNO_12)
    
    For Each checkBox In checkBoxes
            If (outNo And OutNoTable(checkBox.Index)) = OutNoTable(checkBox.Index) Then
                checkBox.value = 1
            Else
                checkBox.value = 0
            End If
    Next
End Sub

Private Function CountCheckFromCheckBoxes(checkBoxes As Variant) As Long
    Dim checkBox As checkBox
    Dim OutNoTable(0 To 11) As LKIF_OUTNO
    
    For Each checkBox In checkBoxes
        If checkBox.value = 1 Then
            CountCheckFromCheckBoxes = CountCheckFromCheckBoxes + 1
        End If
    Next
End Function

Private Function FloatResultValueOutToText(FloatValue As LKIF_FLOATVALUE_OUT) As String
    If FloatValue.FloatResult = LKIF_FLOATRESULT_VALID Then
        FloatResultValueOutToText = FloatValue.value
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_RANGEOVER_P Then
        FloatResultValueOutToText = "+FFFFFFF"
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_RANGEOVER_N Then
        FloatResultValueOutToText = "-FFFFFFF"
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_WAITING Then
        FloatResultValueOutToText = "--------"
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_ALARM Then
        FloatResultValueOutToText = "alarm"
    Else
        FloatResultValueOutToText = "Invalid"
    End If
End Function
Private Function FloatResultValueToText(FloatValue As LKIF_FLOATVALUE) As String
    If FloatValue.FloatResult = LKIF_FLOATRESULT_VALID Then
        FloatResultValueToText = "LKIF_FLOATRESULT_VALID"
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_RANGEOVER_P Then
        FloatResultValueToText = "LKIF_FLOATRESULT_RANGEOVER_P"
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_RANGEOVER_N Then
        FloatResultValueToText = "LKIF_FLOATRESULT_RANGEOVER_N"
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_WAITING Then
        FloatResultValueToText = "LKIF_FLOATRESULT_WAITING"
    ElseIf FloatValue.FloatResult = LKIF_FLOATRESULT_ALARM Then
        FloatResultValueToText = "LKIF_FLOATRESULT_ALARM"
    Else
        FloatResultValueToText = "LKIF_FLOATRESULT_INVALID"
    End If
End Function
Private Function GetTextData(txt As TextBox, ByRef value As Long) As Boolean
On Error GoTo GetTextDataError
    GetTextData = True
    value = CLng(txt.Text)
Exit Function
GetTextDataError:
    MsgBox ("illegal input")
    GetTextData = False
End Function

Private Function GetComboData(cmb As ComboBox, ByRef value As Long) As Boolean
On Error GoTo GetComboDataError
    GetComboData = True

    If cmb.ListIndex = -1 Then
          value = CLng(cmb.Text)
    Else
          value = cmb.ItemData(cmb.ListIndex)
    End If
Exit Function
GetComboDataError:
    MsgBox ("illegal input")
    GetComboData = False
End Function

Private Sub CloseButton_Click()
    Unload Me
End Sub

Private Sub Form_Initialize()

    DISPLAY_UNIT_PARAM_STRINGS = Array( _
                                        Array("LKIF_DISPLAYUNIT_0000_01MM", "LKIF_DISPLAYUNIT_000_001MM", _
                                              "LKIF_DISPLAYUNIT_00_0001MM", "LKIF_DISPLAYUNIT_0_00001MM", _
                                              "LKIF_DISPLAYUNIT_00000_1UM", "LKIF_DISPLAYUNIT_0000_01UM", _
                                              "LKIF_DISPLAYUNIT_000_001UM"), _
                                        Array("LKIF_DISPLAYUNIT_00000_1M_S", "LKIF_DISPLAYUNIT_0000_01M_S", _
                                              "LKIF_DISPLAYUNIT_000_001M_S", "LKIF_DISPLAYUNIT_00000_1MM_S", _
                                              "LKIF_DISPLAYUNIT_0000_01MM_S", "LKIF_DISPLAYUNIT_000_001MM_S", _
                                              "LKIF_DISPLAYUNIT_00_0001MM_S"), _
                                        Array("LKIF_DISPLAYUNIT_00000_1KM_S2", "LKIF_DISPLAYUNIT_0000_01KM_S2", _
                                              "LKIF_DISPLAYUNIT_000_001KM_S2", "LKIF_DISPLAYUNIT_00000_1M_S2", _
                                              "LKIF_DISPLAYUNIT_0000_01M_S2", "LKIF_DISPLAYUNIT_000_001M_S2", _
                                              "LKIF_DISPLAYUNIT_00_0001M_S2") _
                                             )
    FILETER_PARAM_STRINGS = Array( _
                                  Array("LKIF_FILTERPARA_AVE_1", "LKIF_FILTERPARA_AVE_4", _
                                        "LKIF_FILTERPARA_AVE_16", "LKIF_FILTERPARA_AVE_64", _
                                        "LKIF_FILTERPARA_AVE_256", "LKIF_FILTERPARA_AVE_1024", _
                                        "LKIF_FILTERPARA_AVE_4096", "LKIF_FILTERPARA_AVE_16384", _
                                        "LKIF_FILTERPARA_AVE_65536", "LKIF_FILTERPARA_AVE_262144"), _
                                  Array("LKIF_FILTERPARA_COFF_3000", "LKIF_FILTERPARA_COFF_1000", _
                                        "LKIF_FILTERPARA_COFF_300", "LKIF_FILTERPARA_COFF_100", _
                                        "LKIF_FILTERPARA_COFF_30", "LKIF_FILTERPARA_COFF_10", _
                                        "LKIF_FILTERPARA_COFF_3", "LKIF_FILTERPARA_COFF_1", _
                                        "LKIF_FILTERPARA_COFF_0_3", "LKIF_FILTERPARA_COFF_0_1") _
                                )
                                
End Sub

Private Sub Form_Load()
Dim ctl As Control
Dim cmb As ComboBox
    
    For Each ctl In Form1.Controls
        If TypeOf ctl Is ComboBox Then
            Set cmb = ctl
            cmb.ListIndex = 0
        End If
    Next ctl

    SetAbleMinMaxParam2(0).Text = 1
    SetAbleMinMaxParam2(1).Text = 99
    SetNumAlarmParam2.Text = 8
    SetAlarmLevelParam2.Text = 4
    SetDataStorageParam1.Text = 100000
    SetToleranceParam2(0).Text = 50000
    SetToleranceParam2(1).Text = -50000
    SetToleranceParam2(2).Text = 0
    SetScalingParam2(0) = 0
    SetScalingParam2(1) = 0
    SetScalingParam2(2) = 1000
    SetScalingParam2(3) = 1000
    SetFilterParam(2).ListIndex = 4
    SetAnalogScalingParam2(0).Text = 1000
    SetAnalogScalingParam2(1).Text = 1000
    SetAnalogScalingParam2(2).Text = -1000
    SetAnalogScalingParam2(3).Text = -1000
    SetOffsetParam2.Text = 0
    SetDisplayUnitParam(1).ListIndex = 1
    SetPanelLockParam1.ListIndex = 1
    SetNumOfUsedHeadsParam.Text = 2
    SetNumOfUsedOutParam.Text = 2
    SetNumOfUsedAnalogChParam.Text = 2
    SetSamplingCycleParam.ListIndex = 6
    SetOutAddSubParam(1).ListIndex = 1
    SetOutAddSubParam(2).ListIndex = 1
    SetMaskParam1(1).ListIndex = 1
    SetPanelParam(0).ListIndex = 1
    SetPanelParam(1).ListIndex = 2
Set ctl = Nothing
Set cmb = Nothing
End Sub

Private Sub GetCalcData_Click(Index As Integer)
    Select Case Index
        Case 0
           Call GetCalcDataSimple
        Case 1
           Call GetCalcDataMulti
        Case 2
           Call GetCalcDataAll
    End Select
End Sub

Private Sub GetCalcDataSimple()
Dim outNo As Long
Dim result As RC
Dim calcData As LKIF_FLOATVALUE_OUT
   
   If GetComboData(GetCalcDataSingleParam1, outNo) Then
       result = LKIF2_GetCalcDataSingle(outNo, calcData)
   Else
      Exit Sub
   End If
   
   If IsSuccess(result) Then
        GetCalcDataSingleParam2 = FloatResultValueOutToText(calcData)
   End If
   
   Call FuncQuit("LKIF2_GetCalcDataSingle", result)
End Sub

Private Sub GetCalcDataMulti()
Dim outNo As LKIF_OUTNO
Dim cnt As Long
Dim result As RC
Dim calcData(MAX_OUT) As LKIF_FLOATVALUE_OUT

   outNo = MakeOutNoFromCheckBoxes(GetCalcDataMultiParam1)
   result = LKIF2_GetCalcDataMulti(outNo, calcData(0))
   
   If IsSuccess(result) Then
        For cnt = 0 To CountCheckFromCheckBoxes(GetCalcDataMultiParam1) - 1
            GetCalcDataMultiParam2(calcData(cnt).outNo).Caption = FloatResultValueOutToText(calcData(cnt))
        Next
   End If
   
   Call FuncQuit("LKIF2_GetCalcDataMulti", result)
End Sub

Private Sub GetCalcDataAll()
Dim cnt As Long
Dim result As RC
Dim calcData(MAX_OUT) As LKIF_FLOATVALUE_OUT
Dim maxOut  As Long

   result = LKIF2_GetCalcDataALL(maxOut, calcData(0))
   
   If IsSuccess(result) Then
        For cnt = 0 To MAX_OUT - 1
          If cnt < maxOut Then
            GetCalcDataAllParam2(cnt).Caption = FloatResultValueOutToText(calcData(cnt))
          Else
            GetCalcDataAllParam2(cnt).Caption = "------"
          End If
        Next
   End If
   
   Call FuncQuit("LKIF2_GetCalcDataALL", result)
End Sub




Private Sub SetFilterParam_Click(Index As Integer)
    If Index = 1 Then
        Call ResetListForFileterParam
    End If
End Sub


Private Sub SetMeasureTypeParam_Click(Index As Integer)
    If Index = 1 Then
        Call ResetListForDisplayUnitParam
    End If
End Sub

Private Sub SetTiming_Click(Index As Integer)
    Select Case Index
        Case 0
           SetTimingSingle
        Case 1
           SetTimingMulti
        Case 2
           SetTimingSync
    End Select
End Sub

Private Sub SetTimingSingle()
Dim outNo As Long
Dim onOff As Long
Dim result As RC

   If GetComboData(SingleParam(0), outNo) And GetComboData(SingleParam(1), onOff) Then
       result = LKIF2_SetTimingSingle(outNo, onOff)
    Else
      Exit Sub
   End If
      
Call FuncQuit("LKIF2_SetTimingSingle", result)
End Sub

Private Sub SetTimingMulti()
Dim outNo As LKIF_OUTNO
Dim onOff As Long
Dim result As RC

   outNo = MakeOutNoFromCheckBoxes(MultiParam1)

   If GetComboData(MultiParam2, onOff) Then
       result = LKIF2_SetTimingMulti(outNo, onOff)
   Else
      Exit Sub
   End If
   
   Call FuncQuit("LKIF2_SetTimingMulti", result)
End Sub

Private Sub SetTimingSync()
Dim result As RC
Dim onOff As Long
   If GetComboData(SyncParam, onOff) Then
       result = LKIF2_SetTimingSync(onOff)
   Else
      Exit Sub
   End If
   
   Call FuncQuit("LKIF2_SetTimingSync", result)
End Sub

Private Sub SetZero_Click(Index As Integer)
    Select Case Index
        Case 0
           SetZeroSingle
        Case 1
           SetZeroMulti
        Case 2
           SetZeroSync
    End Select
End Sub

Private Sub SetZeroSingle()
Dim outNo As Long
Dim onOff As Long
Dim result As RC
   If GetComboData(SingleParam(0), outNo) And GetComboData(SingleParam(1), onOff) Then
       result = LKIF2_SetZeroSingle(outNo, onOff)
    Else
      Exit Sub
   End If
      
   Call FuncQuit("LKIF2_SetZeroSingle", result)
End Sub

Private Sub SetZeroMulti()
Dim outNo As LKIF_OUTNO
Dim onOff As Long
Dim result As RC

   outNo = MakeOutNoFromCheckBoxes(MultiParam1)

   If GetComboData(MultiParam2, onOff) Then
       result = LKIF2_SetZeroMulti(outNo, onOff)
   Else
      Exit Sub
   End If
   
   Call FuncQuit("LKIF2_SetZeroMulti", result)
End Sub

Private Sub SetZeroSync()
Dim result As RC
Dim onOff As Long

   
   If GetComboData(SyncParam, onOff) Then
       result = LKIF2_SetZeroSync(onOff)
   Else
      Exit Sub
   End If
   
   Call FuncQuit("LKIF2_SetZeroSync", result)
End Sub

Private Sub SetReset_Click(Index As Integer)
    Select Case Index
        Case 0
           SetResetSingle
        Case 1
           SetResetMulti
        Case 2
           SetResetSync
    End Select
End Sub

Private Sub SetResetSingle()
Dim outNo As Long
Dim onOff As Long
Dim result As RC
   
   If GetComboData(SingleParam(0), outNo) Then
       result = LKIF2_SetResetSingle(outNo)
    Else
      Exit Sub
   End If
   
   Call FuncQuit("LKIF2_SetResetSingle", result)
End Sub

Private Sub SetResetMulti()
Dim outNo As LKIF_OUTNO
Dim onOff As Long
Dim result As RC

   outNo = MakeOutNoFromCheckBoxes(MultiParam1)
   result = LKIF2_SetResetMulti(outNo)
      
   Call FuncQuit("LKIF2_SetResetMulti", result)
End Sub

Private Sub SetResetSync()
   Call FuncQuit("LKIF2_SetResetSync", LKIF2_SetResetSync())
End Sub

Private Sub SetPanelLock_Click()
Dim result As RC
Dim onOff As Long

   If GetComboData(SetPanelLockParam1, onOff) Then
       result = LKIF2_SetPanelLock(onOff)
   Else
      Exit Sub
   End If
   
   Call FuncQuit("LKIF2_SetPanelLock", result)
End Sub

Private Sub SetProgramNo_Click()
Dim result As RC
Dim programNo As Long

   If GetComboData(SetProgramNoParam1, programNo) Then
       result = LKIF2_SetProgramNo(programNo)
   Else
      Exit Sub
   End If
   
   Call FuncQuit("LKIF2_SetProgramNo", result)
End Sub

Private Sub GetProgramNo_Click()
Dim result As RC
Dim programNo As Long

   result = LKIF2_GetProgramNo(programNo)
   
   If IsSuccess(result) Then
        GetProgramNoParam1.Caption = SetProgramNoParam1.List(programNo)
   End If

   Call FuncQuit("LKIF2_GetProgramNo", result)
End Sub

Private Sub DataStorageStart_Click()
    Call FuncQuit("LKIF2_DataStorageStart", LKIF2_DataStorageStart())
End Sub

Private Sub DataStorageStop_Click()
    Call FuncQuit("LKIF2_DataStorageStop", LKIF2_DataStorageStop())
End Sub

Private Sub DataStorageInit_Click()
    Call FuncQuit("LKIF2_DataStorageInit", LKIF2_DataStorageInit())
End Sub

Private Sub DataStorageGetData_Click()
Dim outNo As Long
Dim numOutBuffer As Long
Dim OutBuffer(1200000) As LKIF_FLOATVALUE
Dim numReceived As Long
Dim cnt As Long

Dim result As RC
    If GetComboData(DataStorageGetDataParam1, outNo) And GetTextData(DataStorageGetDataParam2, numOutBuffer) Then
        result = LKIF2_DataStorageGetData(outNo, numOutBuffer, OutBuffer(0), numReceived)
    Else
        Exit Sub
    End If

    If IsSuccess(result) Then
        DataStorageGetDataParam3.Caption = numReceived
        
        If checkSaveDataStorageGetData.value = 1 Then
            On Error Resume Next
            CommonDialogForDataStorage.ShowSave
            If Err.Number <> cdlCancel Then
                
                Open CommonDialogForDataStorage.FileName For Output As #1
                Print #1, "OutNo"
                Print #1, outNo
                Print #1, "Data,FloatResult"
                For cnt = 0 To numReceived - 1
                   Print #1, OutBuffer(cnt).value & "," & FloatResultValueToText(OutBuffer(cnt))
                Next
                Close #1
            Else
                Err.Clear
            End If
            
            If Err.Number <> 0 Then
                  MsgBox "Failed in saving the File."
            End If
            
            On Error GoTo 0
        End If
    End If
  
  Call FuncQuit("LKIF2_DataStorageGetStatus", result)
End Sub

Private Sub DataStorageGetStatus_Click()
Dim isStorage As Long
Dim numStorageData As Long
Dim StorageMessage()  As Variant
Dim result As RC
    StorageMessage = Array("Stopping in progress", "Storage in progress")
    
    result = LKIF2_DataStorageGetStatus(isStorage, numStorageData)
  
    If IsSuccess(result) Then
        DataStorageGetStatusParam1.Caption = StorageMessage(isStorage)
        DataStorageGetStatusParam2.Caption = numStorageData
    End If
  
  Call FuncQuit("LKIF2_DataStorageGetStatus", result)
End Sub

Private Sub GetLight_Click()
Dim headNo As Long
Dim peekNo As Long
Dim measurePosition(PEEK_MAX) As Long
Dim waveData(WAVEDATA_MAX) As Byte
Dim cnt As Long

Dim result As RC
    If GetComboData(GetLightParam1, headNo) And GetTextData(GetLightParam2, peekNo) Then
        result = LKIF2_GetLight(headNo, peekNo, measurePosition(0), waveData(0))
    Else
        Exit Sub
    End If

    If IsSuccess(result) Then
        If checkSaveGetLight.value = 1 Then
            On Error Resume Next
            CommonDialogForLight.ShowSave
            If Err.Number <> cdlCancel Then
                Open CommonDialogForLight.FileName For Output As #1
                Print #1, "HeadNo," & waveData(0)
                Print #1, headNo & "," & waveData(1)
                Print #1, "PeekNo," & waveData(2)
                Print #1, peekNo & "," & waveData(3)
                Print #1, "MeasurePosition," & waveData(4)
                Print #1, measurePosition(0) & "," & waveData(5)
                Print #1, measurePosition(1) & "," & waveData(6)
                Print #1, measurePosition(2) & "," & waveData(7)
                Print #1, measurePosition(3) & "," & waveData(8)
                For cnt = 9 To WAVEDATA_MAX - 1
                   Print #1, "," & waveData(cnt)
                Next
                Close #1
            Else
                 Err.Clear
            End If
            
            If Err.Number <> 0 Then
                  MsgBox "Failed in saving the File."
            End If
            
            On Error GoTo 0
        End If
    End If
  Call FuncQuit("LKIF2_GetLight", result)
End Sub

Private Sub SetPanel_Click()
Dim upperDisp As Long
Dim lowerDisp As Long
Dim result As RC
    If GetComboData(SetPanelParam(0), upperDisp) And GetComboData(SetPanelParam(1), lowerDisp) Then
        result = LKIF2_SetPanel(upperDisp, lowerDisp)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetPanel", result)
End Sub

Private Sub SetTolerance_Click()
Dim outNo As Long
Dim upperLimit As Long
Dim lowerLimit As Long
Dim hystersis As Long
Dim result As RC
    If GetComboData(SetToleranceParam1, outNo) And GetTextData(SetToleranceParam2(0), upperLimit) And _
       GetTextData(SetToleranceParam2(1), lowerLimit) And GetTextData(SetToleranceParam2(2), hystersis) _
    Then
        result = LKIF2_SetTolerance(outNo, upperLimit, lowerLimit, hystersis)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetTolerance", result)

End Sub

Private Sub SetAbleMode_Click()
Dim headNo As Long
Dim ableMode As LKIF_ABLEMODE
Dim result As RC
    If GetComboData(SetAbleModeParam(0), headNo) And GetComboData(SetAbleModeParam(1), ableMode) Then
        result = LKIF2_SetAbleMode(headNo, ableMode)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetAbleMode", result)
End Sub

Private Sub SetAbleMinMax_Click()
Dim headNo As Long
Dim min As Long
Dim max As Long
Dim result As RC
    If GetComboData(SetAbleMinMaxParam1, headNo) And GetTextData(SetAbleMinMaxParam2(0), min) And GetTextData(SetAbleMinMaxParam2(1), max) Then
        result = LKIF2_SetAbleMinMax(headNo, min, max)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetAbleMinMax", result)
End Sub

Private Sub SetMeasureMode_Click()
Dim headNo As Long
Dim mesureMode As LKIF_MEASUREMODE
Dim result As RC
    If GetComboData(SetMeasureModeParam(0), headNo) And GetComboData(SetMeasureModeParam(1), mesureMode) Then
        result = LKIF2_SetMeasureMode(headNo, mesureMode)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetMeasureMode", result)
End Sub

Private Sub SetBasicPoint_Click()
Dim headNo As Long
Dim basicPoint As LKIF_BASICPOINT
Dim result As RC
    If GetComboData(SetBasicPointParam(0), headNo) And GetComboData(SetBasicPointParam(1), basicPoint) Then
        result = LKIF2_SetBasicPoint(headNo, basicPoint)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetBasicPoint", result)
End Sub

Private Sub SetNumAlarm_Click()
Dim headNo As Long
Dim numAlarm As Long
Dim result As RC
   
   If GetComboData(SetNumAlarmParam1, headNo) And GetTextData(SetNumAlarmParam2, numAlarm) Then
        result = LKIF2_SetNumAlarm(headNo, numAlarm)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetNumAlarm", result)
End Sub

Private Sub SetNumRecovery_Click()
Dim headNo As Long
Dim numRecovery As Long
Dim result As RC
   
   If GetComboData(SetNumRecoveryParam1, headNo) And GetTextData(SetNumRecoveryParam2, numRecovery) Then
        result = LKIF2_SetNumRecovery(headNo, numRecovery)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetNumRecovery", result)
End Sub

Private Sub SetAlarmLevel_Click()
Dim headNo As Long
Dim alaramLevel As Long
Dim result As RC
   
   If GetComboData(SetAlarmLevelParam1, headNo) And GetTextData(SetAlarmLevelParam2, alaramLevel) Then
        result = LKIF2_SetAlarmLevel(headNo, alaramLevel)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetAlarmLevel", result)
End Sub

Private Sub AbleStart_Click()
Dim headNo As Long
Dim result As RC
   
   If GetComboData(AbleStartParam1, headNo) Then
        result = LKIF2_AbleStart(headNo)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_AbleStart", result)
End Sub

Private Sub AbleCancel_Click()
    Call FuncQuit("LKIF2_AbleCancel", LKIF2_AbleCancel())
End Sub

Private Sub AbleStop_Click()
    Call FuncQuit("LKIF2_AbleStop", LKIF2_AbleStop())
End Sub

Private Sub SetReflectionMode_Click()
Dim headNo As Long
Dim reflectionMode As LKIF_REFLECTIONMODE
Dim result As RC
    If GetComboData(SetReflectionModeParam(0), headNo) And GetComboData(SetReflectionModeParam(1), reflectionMode) Then
        result = LKIF2_SetReflectionMode(headNo, reflectionMode)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetReflectionMode", result)
End Sub

Private Sub SetMask_Click()
Dim headNo As Long
Dim onOff As Long
Dim pos1 As Long
Dim pos2 As Long
Dim result As RC
    If GetComboData(SetMaskParam1(0), headNo) And GetComboData(SetMaskParam1(1), onOff) And _
       GetTextData(SetMaskParam2(0), pos1) And GetTextData(SetMaskParam2(1), pos2) _
    Then
        result = LKIF2_SetMask(headNo, onOff, pos1, pos2)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetMask", result)
End Sub

Private Sub SetMedian_Click()
Dim headNo As Long
Dim median As LKIF_MEDIAN
Dim result As RC
    If GetComboData(SetMedianParam(0), headNo) And GetComboData(SetMedianParam(1), median) Then
        result = LKIF2_SetMedian(headNo, median)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetMedian", result)
End Sub

Private Sub SetLaserCtrlGroup_Click()
Dim headNo As Long
Dim laserCtrlGroup As LKIF_LASER_CTRL_GROUP
Dim result As RC
    If GetComboData(SetLaserCtrlGroupParam(0), headNo) And GetComboData(SetLaserCtrlGroupParam(1), laserCtrlGroup) Then
        result = LKIF2_SetLaserCtrlGroup(headNo, laserCtrlGroup)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetLaserCtrlGroup", result)
End Sub

Private Sub SetRange_Click()
Dim headNo As Long
Dim range As LKIF_RANGE
Dim result As RC
    If GetComboData(SetRangeParam(0), headNo) And GetComboData(SetRangeParam(1), range) Then
        result = LKIF2_SetRange(headNo, range)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetRange", result)
End Sub

Private Sub SetMutualInterferencePreventionGroup_Click()
Dim headNo As Long
Dim group As LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP
Dim result As RC
    If GetComboData(SetMutualInterferencePreventionGroupParam(0), headNo) And GetComboData(SetMutualInterferencePreventionGroupParam(1), group) Then
        result = LKIF2_SetMutualInterferencePreventionGroup(headNo, group)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetMutualInterferencePreventionGroup", result)
End Sub

Private Sub SetCalcMethod_Click()
Dim outNo As Long
Dim head_Out_No As Long
Dim method As LKIF_CALCMETHOD
Dim result As RC

    If GetComboData(SetCalcMethodParam1(0), outNo) And GetComboData(SetCalcMethodParam1(1), method) And GetTextData(SetCalcMethodParam2, head_Out_No) Then
        result = LKIF2_SetCalcMethod(outNo, method, head_Out_No)
    Else
        Exit Sub
   End If
   Call FuncQuit("LKIF2_SetCalcMethod", result)
End Sub

Private Sub SetCalcTarget_Click()
Dim outNo As Long
Dim target As LKIF_CALCTARGET
Dim result As RC
    If GetComboData(SetCalcTargetParam(0), outNo) And GetComboData(SetCalcTargetParam(1), target) Then
        result = LKIF2_SetCalcTarget(outNo, target)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetCalcTarget", result)
End Sub

Private Sub SetOutAddSub_Click()
Dim outNo As Long
Dim addSub1 As Long
Dim addSub2 As Long
Dim result As RC
    If GetComboData(SetOutAddSubParam(0), outNo) And GetComboData(SetOutAddSubParam(1), addSub1) And GetComboData(SetOutAddSubParam(2), addSub2) Then
        result = LKIF2_SetOutAddSub(outNo, addSub1, addSub2)
    Else
        Exit Sub
    End If

    Call FuncQuit("LKIF2_SetOutAddSub", result)
End Sub

Private Sub SetOutOperation_Click()
Dim outNo As Long
Dim targetOut As LKIF_OUTNO
Dim result As RC

    targetOut = MakeOutNoFromCheckBoxes(SetOutOperationParam2)
    If GetComboData(SetOutOperationParam1, outNo) Then
        result = LKIF2_SetOutOperation(outNo, targetOut)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetOutOperation", result)
End Sub

Private Sub SetScaling_Click()
Dim outNo As Long
Dim inputValue1 As Long
Dim outputValue1 As Long
Dim inputValue2 As Long
Dim outputValue2 As Long
Dim result As RC
    If GetComboData(SetScalingParam1, outNo) And _
       GetTextData(SetScalingParam2(0), inputValue1) And GetTextData(SetScalingParam2(1), outputValue1) And _
       GetTextData(SetScalingParam2(2), inputValue2) And GetTextData(SetScalingParam2(3), outputValue2) _
    Then
        result = LKIF2_SetScaling(outNo, inputValue1, outputValue1, inputValue2, outputValue2)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetScaling", result)
End Sub

Private Sub SetFilter_Click()
Dim outNo As Long
Dim result As RC
Dim filterPara As LKIF_FILTERPARA
Dim filterMode As LKIF_FILTERMODE
    If GetComboData(SetFilterParam(0), outNo) And GetComboData(SetFilterParam(1), filterMode) And GetComboData(SetFilterParam(2), filterPara) Then
        result = LKIF2_SetFilter(outNo, filterMode, filterPara)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetFilter", result)
End Sub

Private Sub SetTriggerMode_Click()
Dim outNo As Long
Dim result As RC
Dim triggerMode As LKIF_TRIGGERMODE
    If GetComboData(SetTriggerModeParam(0), outNo) And GetComboData(SetTriggerModeParam(1), triggerMode) Then
        result = LKIF2_SetTriggerMode(outNo, triggerMode)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetTriggerMode", result)
End Sub

Private Sub SetOffset_Click()
Dim outNo As Long
Dim result As RC
Dim offset As Long
    If GetComboData(SetOffsetParam1, outNo) And GetTextData(SetOffsetParam2, offset) Then
        result = LKIF2_SetOffset(outNo, offset)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetOffset", result)
End Sub

Private Sub SetCalcMode_Click()
Dim outNo As Long
Dim result As RC
Dim CalcMode As LKIF_CALCMODE
    If GetComboData(SetCalcModeParam(0), outNo) And GetComboData(SetCalcModeParam(1), CalcMode) Then
        result = LKIF2_SetCalcMode(outNo, CalcMode)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetCalcMode", result)
End Sub

Private Sub SetAnalogScaling_Click()
Dim outNo As Long
Dim inputValue1 As Long
Dim outputVoltage1 As Long
Dim inputValue2 As Long
Dim outputVoltage2 As Long
Dim result As RC
    If GetComboData(SetAnalogScalingParam1, outNo) And _
       GetTextData(SetAnalogScalingParam2(0), inputValue1) And GetTextData(SetAnalogScalingParam2(1), outputVoltage1) And _
       GetTextData(SetAnalogScalingParam2(2), inputValue2) And GetTextData(SetAnalogScalingParam2(3), outputVoltage2) _
    Then
        result = LKIF2_SetAnalogScaling(outNo, inputValue1, outputVoltage1, inputValue2, outputVoltage2)
    Else
        Exit Sub
    End If
    
    Call FuncQuit("LKIF2_SetAnalogScaling", result)
End Sub

Private Sub SetDisplayUnit_Click()
Dim outNo As Long
Dim result As RC
Dim displayUnit As LKIF_DISPLAYUNIT
    If GetComboData(SetDisplayUnitParam(0), outNo) And GetComboData(SetDisplayUnitParam(1), displayUnit) Then
        result = LKIF2_SetDisplayUnit(outNo, displayUnit)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetDisplayUnit", result)
End Sub

Private Sub SetMeasureType_Click()
Dim outNo As Long
Dim result As RC
Dim measureType As LKIF_MEASURETYPE
    If GetComboData(SetMeasureTypeParam(0), outNo) And GetComboData(SetMeasureTypeParam(1), measureType) Then
        result = LKIF2_SetMeasureType(outNo, measureType)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetMeasureType", result)
End Sub

Private Sub SetSynchronization_Click()
Dim outNo As Long
Dim result As RC
Dim onOff As Long
    If GetComboData(SetSynchronizationParam(0), outNo) And GetComboData(SetSynchronizationParam(1), onOff) Then
        result = LKIF2_SetSynchronization(outNo, onOff)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetSynchronization", result)
End Sub

Private Sub SetStorageTarget_Click()
Dim outNo As Long
Dim result As RC
Dim onOff As Long
    If GetComboData(SetStorageTargetParam(0), outNo) And GetComboData(SetStorageTargetParam(1), onOff) Then
        result = LKIF2_SetStorageTarget(outNo, onOff)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetStorageTarget", result)
End Sub

Private Sub SetSamplingCycle_Click()
Dim result As RC
Dim samplingCycle As LKIF_SAMPLINGCYCLE
    If GetComboData(SetSamplingCycleParam, samplingCycle) Then
        result = LKIF2_SetSamplingCycle(samplingCycle)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetSamplingCycle", result)
End Sub

Private Sub SetMutualInterferencePrevention_Click()
Dim result As RC
Dim onOff As LKIF_MUTUAL_INTERFERENCE_PREVENTION
    If GetComboData(SetMutualInterferencePreventionParam, onOff) Then
        result = LKIF2_SetMutualInterferencePrevention(onOff)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetMutualInterferencePrevention", result)
End Sub

Private Sub SetToleranceComparatorOutputFormat_Click()
Dim result As RC
Dim toleranceComparatorOutputFormat As LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT
    If GetComboData(SetToleranceComparatorOutputFormatParam, toleranceComparatorOutputFormat) Then
        result = LKIF2_SetToleranceComparatorOutputFormat(toleranceComparatorOutputFormat)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetToleranceComparatorOutputFormat", result)
End Sub

Private Sub SetStorobeTime_Click()
Dim result As RC
Dim storobeTime As LKIF_STOROBETIME
    If GetComboData(SetStorobeTimeParam, storobeTime) Then
        result = LKIF2_SetStrobeTime(storobeTime)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetStrobeTime", result)
End Sub

Private Sub SetDataStorage_Click()
Dim result As RC
Dim storageNum As Long
Dim storageCycle  As LKIF_STORAGECYCLE
    If GetTextData(SetDataStorageParam1, storageNum) And GetComboData(SetDataStorageParam2, storageCycle) Then
        result = LKIF2_SetDataStorage(storageNum, storageCycle)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetDataStorage", result)
End Sub

Private Sub SetAnalogChannel_Click()
Dim chNo As Long
Dim result As RC
Dim outNo As Long
    If GetComboData(SetAnalogChannelParam(0), chNo) And GetComboData(SetAnalogChannelParam(1), outNo) Then
        result = LKIF2_SetAnalogChannel(chNo, outNo)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetAnalogChannel", result)
End Sub

Private Sub SetAlarmOutputForm_Click()
Dim alarmOutputForm As LKIF_ALARM_OUTPUT_FORM
Dim result As RC
    If GetComboData(SetAlarmOutputFormParam, alarmOutputForm) Then
        result = LKIF2_SetAlarmOutputForm(alarmOutputForm)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetAlarmOutputForm", result)
End Sub

Private Sub SetNumOfUsedHeads_Click()
Dim numOfUsedHeads As Long
Dim result As RC
    If GetTextData(SetNumOfUsedHeadsParam, numOfUsedHeads) Then
        result = LKIF2_SetNumOfUsedHeads(numOfUsedHeads)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetNumOfUsedHeads", result)
End Sub

Private Sub SetNumOfUsedOut_Click()
Dim numOfUsedOut As Long
Dim result As RC
    If GetTextData(SetNumOfUsedOutParam, numOfUsedOut) Then
        result = LKIF2_SetNumOfUsedOut(numOfUsedOut)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetNumOfUsedOut", result)
End Sub

Private Sub SetNumOfUsedAnalogCh_Click()
Dim numOfUsedAnalogCh As Long
Dim result As RC
    If GetTextData(SetNumOfUsedAnalogChParam, numOfUsedAnalogCh) Then
        result = LKIF2_SetNumOfUsedAnalogCh(numOfUsedAnalogCh)
    Else
        Exit Sub
    End If
    Call FuncQuit("LKIF2_SetNumOfUsedAnalogCh", result)
End Sub

Private Sub GetPanel_Click()
Dim upperDisp As Long
Dim lowerDisp As Long
Dim result As RC

    result = LKIF2_GetPanel(upperDisp, lowerDisp)

    If IsSuccess(result) Then
        upperDisp = upperDisp + 1
        lowerDisp = lowerDisp + 1
        GetPanelParam(0).Caption = SetPanelParam(0).List(upperDisp)
        GetPanelParam(1).Caption = SetPanelParam(1).List(lowerDisp)
    End If
    Call FuncQuit("LKIF2_GetPanel", result)
End Sub

Private Sub GetTolerance_Click()
Dim outNo As Long
Dim upperLimit As Long
Dim lowerLimit As Long
Dim hysteresis As Long
Dim result As RC
    If GetComboData(GetToleranceParam1, outNo) Then
        result = LKIF2_GetTolerance(outNo, upperLimit, lowerLimit, hysteresis)
    Else
        Exit Sub
    End If
    If IsSuccess(result) Then
        GetToleranceParam2(0).Caption = upperLimit
        GetToleranceParam2(1).Caption = lowerLimit
        GetToleranceParam2(2).Caption = hysteresis
    End If
    
    Call FuncQuit("LKIF2_GetTolerance", result)
End Sub

Private Sub GetAbleMode_Click()
Dim headNo As Long
Dim ableMode As LKIF_ABLEMODE
Dim result As RC
    If GetComboData(GetAbleModeParam1, headNo) Then
        result = LKIF2_GetAbleMode(headNo, ableMode)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetAbleModeParam2.Caption = SetAbleModeParam(1).List(ableMode)
    End If
    
    Call FuncQuit("LKIF2_GetAbleMode", result)
End Sub

Private Sub GetAbleMinMax_Click()
Dim headNo As Long
Dim min As Long
Dim max As Long
Dim result As RC
    If GetComboData(GetAbleMinMaxParam1, headNo) Then
        result = LKIF2_GetAbleMinMax(headNo, min, max)
    Else
        Exit Sub
    End If
    If IsSuccess(result) Then
        GetAbleMinMaxParam2.Caption = min
        GetAbleMinMaxParam3.Caption = max
    End If
    
    Call FuncQuit("LKIF2_GetAbleMinMax", result)
End Sub

Private Sub GetMeasureMode_Click()
Dim headNo As Long
Dim measureMode As LKIF_MEASUREMODE
Dim result As RC

    If GetComboData(GetMeasureModeParam1, headNo) Then
        result = LKIF2_GetMeasureMode(headNo, measureMode)
    Else
        Exit Sub
    End If
    If IsSuccess(result) Then
        GetMeasureModeParam2.Caption = SetMeasureModeParam(1).List(measureMode)
    End If
    
    Call FuncQuit("LKIF2_GetMeasureMode", result)
End Sub

Private Sub GetBasicPoint_Click()
Dim headNo As Long
Dim basicPoint As LKIF_BASICPOINT
Dim result As RC
    If GetComboData(GetBasicPointParam1, headNo) Then
        result = LKIF2_GetBasicPoint(headNo, basicPoint)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetBasicPointParam2.Caption = SetBasicPointParam(1).List(basicPoint)
    End If
        
    Call FuncQuit("LKIF2_GetBasicPoint", result)
End Sub

Private Sub GetNumAlarm_Click()
Dim headNo As Long
Dim numAlarm As Long
Dim result As RC
    If GetComboData(GetNumAlarmParam1, headNo) Then
        result = LKIF2_GetNumAlarm(headNo, numAlarm)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetNumAlarmParam2.Caption = numAlarm
    End If
        
    Call FuncQuit("LKIF2_GetNumAlarm", result)
End Sub

Private Sub GetNumRecovery_Click()
Dim headNo As Long
Dim recoveryNum As Long
Dim result As RC
    If GetComboData(GetNumRecoveryParam1, headNo) Then
        result = LKIF2_GetNumRecovery(headNo, recoveryNum)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetNumRecoveryParam2.Caption = recoveryNum
    End If
        
    Call FuncQuit("LKIF2_GetNumRecovery", result)
End Sub

Private Sub GetAlarmLevel_Click()
Dim headNo As Long
Dim alaramLevel As Long
Dim result As RC
    If GetComboData(GetAlarmLevelParam1, headNo) Then
        result = LKIF2_GetAlarmLevel(headNo, alaramLevel)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetAlarmLevelParam2.Caption = alaramLevel
    End If
        
    Call FuncQuit("LKIF2_GetAlarmLevel", result)
End Sub

Private Sub GetReflectionMode_Click()
Dim headNo As Long
Dim reflectionMode As LKIF_REFLECTIONMODE
Dim result As RC
    If GetComboData(GetReflectionModeParam1, headNo) Then
        result = LKIF2_GetReflectionMode(headNo, reflectionMode)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetReflectionModeParam2.Caption = SetReflectionModeParam(1).List(reflectionMode)
    End If
        
    Call FuncQuit("LKIF2_GetReflectionMode", result)
End Sub

Private Sub GetMask_Click()
Dim headNo As Long
Dim onOff As Long
Dim pos1 As Long
Dim pos2 As Long
Dim result As RC
    If GetComboData(GetMaskParam1, headNo) Then
        result = LKIF2_GetMask(headNo, onOff, pos1, pos2)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
         onOff = -(onOff - 1)
        GetMaskParam2(0).Caption = SetMaskParam1(1).List(onOff)
        GetMaskParam2(1).Caption = pos1
        GetMaskParam2(2).Caption = pos2
    End If
        
    Call FuncQuit("LKIF2_GetMask", result)
End Sub

Private Sub GetMedian_Click()
Dim headNo As Long
Dim median As LKIF_MEDIAN
Dim result As RC
    If GetComboData(GetMedianParam1, headNo) Then
        result = LKIF2_GetMedian(headNo, median)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetMedianParam2.Caption = SetMedianParam(1).List(median)
    End If
        
    Call FuncQuit("LKIF2_GetMedian", result)
End Sub

Private Sub GetLaserCtrlGroup_Click()
Dim headNo As Long
Dim laserCtrlGroup As LKIF_LASER_CTRL_GROUP
Dim result As RC
    If GetComboData(GetLaserCtrlGroupParam1, headNo) Then
        result = LKIF2_GetLaserCtrlGroup(headNo, laserCtrlGroup)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetLaserCtrlGroupParam2.Caption = SetLaserCtrlGroupParam(1).List(laserCtrlGroup)
    End If
        
    Call FuncQuit("LKIF2_GetLaserCtrlGroup", result)
End Sub

Private Sub GetRange_Click()
Dim headNo As Long
Dim range As LKIF_RANGE
Dim result As RC

    If GetComboData(GetRangeParam1, headNo) Then
        result = LKIF2_GetRange(headNo, range)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetRangeParam2.Caption = SetRangeParam(1).List(range)
    End If
        
    Call FuncQuit("LKIF2_GetRange", result)
End Sub

Private Sub GetMutualInterferencePreventionGroup_Click()
Dim headNo As Long
Dim group As LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP
Dim result As RC
    
    If GetComboData(GetMutualInterferencePreventionGroupParam1, headNo) Then
        result = LKIF2_GetMutualInterferencePreventionGroup(headNo, group)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetMutualInterferencePreventionGroupParam2.Caption = SetMutualInterferencePreventionGroupParam(1).List(group)
    End If
    
    Call FuncQuit("LKIF2_GetMutualInterferencePreventionGroup", result)
End Sub

Private Sub GetCalcMethod_Click()
Dim outNo As Long
Dim calcMethod As LKIF_CALCMETHOD
Dim head_Out_No As Long
Dim result As RC

    If GetComboData(GetCalcMethodParam1, outNo) Then
        result = LKIF2_GetCalcMethod(outNo, calcMethod, head_Out_No)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetCalcMethodParam2(0).Caption = SetCalcMethodParam1(1).List(calcMethod)
        GetCalcMethodParam2(1).Caption = head_Out_No
    End If
    
     Call FuncQuit("LKIF2_GetCalcMethod", result)
End Sub

Private Sub GetCalcTarget_Click()
Dim outNo As Long
Dim calcTarget As LKIF_CALCTARGET
Dim result As RC

    If GetComboData(GetCalcTargetParam1, outNo) Then
        result = LKIF2_GetCalcTarget(outNo, calcTarget)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
         GetCalcTargetParam2.Caption = SetCalcTargetParam(1).List(calcTarget)
    End If
    
     Call FuncQuit("LKIF2_GetCalcTarget", result)
End Sub

Private Sub GetOutAddSub_Click()
Dim outNo As Long
Dim addSub1 As Long
Dim addSub2 As Long
Dim result As RC

    If GetComboData(GetOutAddSubParam1, outNo) Then
        result = LKIF2_GetOutAddSub(outNo, addSub1, addSub2)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
         GetOutAddSubParam2(0).Caption = SetOutAddSubParam(1).List(addSub1)
         GetOutAddSubParam2(1).Caption = SetOutAddSubParam(2).List(addSub2)
    End If
    
     Call FuncQuit("LKIF2_GetOutAddSub", result)
End Sub

Private Sub GetOutOperation_Click()
Dim outNo As Long
Dim targetOut As LKIF_OUTNO
Dim result As RC

    If GetComboData(GetOutOperationParam1, outNo) Then
        result = LKIF2_GetOutOperation(outNo, targetOut)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        Call SetCheckBoxesForOutNo(GetOutOperationParam2, targetOut)
    End If
    
     Call FuncQuit("LKIF2_GetOutOperation", result)
End Sub

Private Sub GetScaling_Click()
Dim outNo As Long
Dim inputValue1 As Long
Dim outputValue1 As Long
Dim inputValue2 As Long
Dim outputValue2 As Long
Dim result As RC

    If GetComboData(GetScalingParam1, outNo) Then
        result = LKIF2_GetScaling(outNo, inputValue1, outputValue1, inputValue2, outputValue2)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetScalingParam2(0).Caption = inputValue1
        GetScalingParam2(1).Caption = outputValue1
        GetScalingParam2(2).Caption = inputValue2
        GetScalingParam2(3).Caption = outputValue2
    End If
    
     Call FuncQuit("LKIF2_GetScaling", result)
End Sub

Private Sub GetFilter_Click()
Dim outNo As Long
Dim filterMode As LKIF_FILTERMODE
Dim filterPara As LKIF_FILTERPARA
Dim result As RC
    If GetComboData(GetFilterParam1, outNo) Then
        result = LKIF2_GetFilter(outNo, filterMode, filterPara)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetFilterParam2(0).Caption = SetFilterParam(1).List(filterMode)
        GetFilterParam2(1).Caption = GetFileterParamString(filterMode, filterPara)
    End If
    
    Call FuncQuit("LKIF2_GetFilter", result)
End Sub

Private Sub GetTriggerMode_Click()
Dim outNo As Long
Dim triggerMode As LKIF_TRIGGERMODE
Dim result As RC

    If GetComboData(GetTriggerModeParam1, outNo) Then
        result = LKIF2_GetTriggerMode(outNo, triggerMode)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetTriggerModeParam2.Caption = SetTriggerModeParam(1).List(triggerMode)
    End If
    
     Call FuncQuit("LKIF2_GetTriggerMode", result)
End Sub

Private Sub GetOffset_Click()
Dim outNo As Long
Dim offset As Long
Dim result As RC

    If GetComboData(GetOffsetParam1, outNo) Then
        result = LKIF2_GetOffset(outNo, offset)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetOffsetParam2.Caption = offset
    End If
    
     Call FuncQuit("LKIF2_GetOffset", result)
End Sub

Private Sub GetCalcMode_Click()
Dim outNo As Long
Dim CalcMode As LKIF_CALCMODE
Dim result As RC

    If GetComboData(GetCalcModeParam1, outNo) Then
        result = LKIF2_GetCalcMode(outNo, CalcMode)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetCalcModeParam2.Caption = SetCalcModeParam(1).List(CalcMode)
    End If
    
     Call FuncQuit("LKIF2_GetCalcMode", result)
End Sub

Private Sub GetAnalogScaling_Click()
Dim outNo As Long
Dim inputValue1 As Long
Dim outputVoltage1 As Long
Dim inputValue2 As Long
Dim outputVoltage2 As Long
Dim result As RC

    If GetComboData(GetAnalogScalingParam1, outNo) Then
        result = LKIF2_GetAnalogScaling(outNo, inputValue1, outputVoltage1, inputValue2, outputVoltage2)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetAnalogScalingParam2(0).Caption = inputValue1
        GetAnalogScalingParam2(1).Caption = outputVoltage1
        GetAnalogScalingParam2(2).Caption = inputValue2
        GetAnalogScalingParam2(3).Caption = outputVoltage2
    End If
    
     Call FuncQuit("LKIF2_GetAnalogScaling", result)
End Sub

Private Sub GetDisplayUnit_Click()
Dim outNo As Long
Dim measureType As LKIF_MEASURETYPE
Dim displayUnit As LKIF_DISPLAYUNIT
Dim result(2) As RC

    If GetComboData(GetDisplayUnitParam1, outNo) Then
        result(0) = LKIF2_GetMeasureType(outNo, measureType)
        result(1) = LKIF2_GetDisplayUnit(outNo, displayUnit)
    Else
        Exit Sub
    End If
    
    
    If IsSuccess(result(0)) And IsSuccess(result(1)) Then
        GetDisplayUnitParam2.Caption = GetDisplayUnitParamString(measureType, displayUnit)
    End If

    
    Call FuncQuit("LKIF2_GetDisplayUnit", result(1))
End Sub

Private Sub GetMeasureType_Click()
Dim outNo As Long
Dim measureType As LKIF_MEASURETYPE
Dim result As RC

    If GetComboData(GetMeasureTypeParam1, outNo) Then
        result = LKIF2_GetMeasureType(outNo, measureType)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetMeasureTypeParam2.Caption = SetMeasureTypeParam(1).List(measureType)
    End If
    
    Call FuncQuit("LKIF2_GetMeasureType", result)
End Sub

Private Sub GetSynchronization_Click()
Dim outNo As Long
Dim onOff As Long
Dim result As RC

    If GetComboData(GetSynchronizationParam1, outNo) Then
        result = LKIF2_GetSynchronization(outNo, onOff)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        onOff = -(onOff - 1)
        GetSynchronizationParam2.Caption = SetSynchronizationParam(1).List(onOff)
    End If
    
    Call FuncQuit("LKIF2_GetSynchronization", result)
End Sub

Private Sub GetStorageTarget_Click()
Dim outNo As Long
Dim onOff As Long
Dim result As RC

    If GetComboData(GetStorageTargetParam1, outNo) Then
        result = LKIF2_GetStorageTarget(outNo, onOff)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        onOff = -(onOff - 1)
        GetStorageTargetParam2.Caption = SetStorageTargetParam(1).List(onOff)
    End If
    
    Call FuncQuit("LKIF2_GetStorageTarget", result)
End Sub

Private Sub GetSamplingCycle_Click()
Dim samplingCycle As LKIF_SAMPLINGCYCLE
Dim result As RC

    result = LKIF2_GetSamplingCycle(samplingCycle)
    
    If IsSuccess(result) Then
        GetSamplingCycleParam.Caption = SetSamplingCycleParam.List(samplingCycle)
    End If
    
    Call FuncQuit("LKIF2_GetSamplingCycle", result)
End Sub

Private Sub GetMutualInterferencePrevention_Click()
Dim onOff As LKIF_MUTUAL_INTERFERENCE_PREVENTION
Dim result As RC

    result = LKIF2_GetMutualInterferencePrevention(onOff)
    
    If IsSuccess(result) Then
        GetMutualInterferencePreventionParam.Caption = SetMutualInterferencePreventionParam.List(onOff)
    End If
    
    Call FuncQuit("LKIF2_GetMutualInterferencePrevention", result)
End Sub

Private Sub GetToleranceComparatorOutputFormat_Click()
Dim toleranceComparatorOutputFormat As LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT
Dim result As RC

    result = LKIF2_GetToleranceComparatorOutputFormat(toleranceComparatorOutputFormat)
    
    If IsSuccess(result) Then
        GetToleranceComparatorOutputFormatParam.Caption = SetToleranceComparatorOutputFormatParam.List(toleranceComparatorOutputFormat)
    End If
    
    Call FuncQuit("LKIF2_GetToleranceComparatorOutputFormat", result)
End Sub

Private Sub GetStorobeTime_Click()
Dim storobeTime As LKIF_STOROBETIME
Dim result As RC

    result = LKIF2_GetStrobeTime(storobeTime)
    
    If IsSuccess(result) Then
        GetStorobeTimeParam.Caption = SetStorobeTimeParam.List(storobeTime)
    End If
    
    Call FuncQuit("LKIF2_GetStrobeTime", result)
End Sub

Private Sub GetDataStorage_Click()
Dim storageNum As Long
Dim storageCycle As LKIF_STORAGECYCLE
Dim result As RC
    result = LKIF2_GetDataStorage(storageNum, storageCycle)
    
    If IsSuccess(result) Then
        GetDataStorageParam(0).Caption = storageNum
        GetDataStorageParam(1).Caption = SetDataStorageParam2.List(storageCycle)
    End If
    
    Call FuncQuit("LKIF2_GetDataStorage", result)
End Sub

Private Sub GetAnalogChannel_Click()
Dim chNo As Long
Dim outNo As Long
Dim result As RC

    If GetComboData(GetAnalogChannelParam1, chNo) Then
        result = LKIF2_GetAnalogChannel(chNo, outNo)
    Else
        Exit Sub
    End If
    
    If IsSuccess(result) Then
        GetAnalogChannelParam2.Caption = SetAnalogChannelParam(1).List(outNo)
    End If
    
    Call FuncQuit("LKIF2_GetAnalogChannel", result)
End Sub


Private Sub GetAlarmOutputForm_Click()
Dim alarmOutputForm As LKIF_ALARM_OUTPUT_FORM
Dim result As RC

    result = LKIF2_GetAlarmOutputForm(alarmOutputForm)
    
    If IsSuccess(result) Then
        GetAlarmOutputFormParam.Caption = SetAlarmOutputFormParam.List(alarmOutputForm)
    End If
    
    Call FuncQuit("LKIF2_GetAlarmOutputForm", result)
End Sub

Private Sub GetNumOfUsedHeads_Click()
Dim numOfUsedHeads As Long
Dim result As RC

    result = LKIF2_GetNumOfUsedHeads(numOfUsedHeads)
    
    If IsSuccess(result) Then
        GetNumOfUsedHeadsParam.Caption = numOfUsedHeads
    End If
    
    Call FuncQuit("LKIF2_GetNumOfUsedHeads", result)
End Sub

Private Sub GetNumOfUsedOut_Click()
Dim numOfUsedOut As Long
Dim result As RC

    result = LKIF2_GetNumOfUsedOut(numOfUsedOut)
    
    If IsSuccess(result) Then
        GetNumOfUsedOutParam.Caption = numOfUsedOut
    End If
    
    Call FuncQuit("LKIF2_GetNumOfUsedOut", result)
End Sub

Private Sub GetNumOfUsedAnalogCh_Click()
Dim numOfUsedAnalogCh As Long
Dim result As RC

    result = LKIF2_GetNumOfUsedAnalogCh(numOfUsedAnalogCh)
    
    If IsSuccess(result) Then
        GetNumOfUsedAnalogChParam.Caption = numOfUsedAnalogCh
    End If
    
    Call FuncQuit("LKIF2_GetNumOfUsedAnalogCh", result)
End Sub

Private Sub StartMeasure_Click()
     Call FuncQuit("LKIF2_StartMeasure", LKIF2_StartMeasure())
End Sub

Private Sub StopMeasure_Click()
    Call FuncQuit("LKIF2_StopMeasure", LKIF2_StopMeasure())
End Sub
Private Sub OpenDeviceUSB_Click()
    Call FuncQuit("LKIF2_OpenDeviceUsb", LKIF2_OpenDeviceUsb())
End Sub

Private Sub OpenDeviceETHER_Click()
Dim ipAddres As String
Dim openParam As LKIF_OPENPARAM_ETHERNET
 'example:"192.168.1.1"
 ipAddres = OpenDeviceETHERParam1(0) & "." & OpenDeviceETHERParam1(1) & "." & OpenDeviceETHERParam1(2) & "." & OpenDeviceETHERParam1(3)
 openParam.IPAddress.S_addr = inet_addr(ipAddres)

 Call FuncQuit("LKIF2_OpenDeviceETHER", LKIF2_OpenDeviceETHER(openParam))
End Sub

Private Sub CloseDevice_Click()
     Call FuncQuit("LKIF2_CloseDevice", LKIF2_CloseDevice())
End Sub

Private Sub TabControl_Click(PreviousTab As Integer)
Dim ctl As Control
Dim cmb As ComboBox
    'SSTab's bug fix
    For Each ctl In Form1.Controls
        If TypeOf ctl Is ComboBox Then
            Set cmb = ctl
            cmb.SelLength = 0
        End If
    Next ctl
Set ctl = Nothing
Set cmb = Nothing
End Sub
