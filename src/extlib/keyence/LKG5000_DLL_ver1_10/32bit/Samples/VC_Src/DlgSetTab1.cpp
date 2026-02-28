#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "DlgSetTab1.h"
#include "LkIF2TestVCDlg.h"
#include "LKIF2Tools.h"

const int SET_TAB1_COMBO_NUM = 31;
const int SET_TAB1_COMBO_IDS[SET_TAB1_COMBO_NUM] =
							{IDC_COMBO_SET_PANEL_PARAM1,IDC_COMBO_SET_TOLERANCE_PARAM,
							 IDC_COMBO_SET_ABLE_PARAM1,IDC_COMBO_SET_ABLE_MINMAX_PARAM,
							 IDC_COMBO_SET_MEASURE_PARAM1,IDC_COMBO_SET_BASIC_PARAM1,
							 IDC_COMBO_SET_NUM_ALARM_PARAM,IDC_COMBO_SET_RECOVERY_PARAM,
							 IDC_COMBO_SET_LEVEL_PARAM,IDC_COMBO_ABLE_START_PARAM,
							 IDC_COMBO_SET_REFLECTION_PARAM1,IDC_COMBO_SET_MASK_PARAM1,
							 IDC_COMBO_SET_MEDIAN_PARAM1,IDC_COMBO_SET_LASER_GROUP_PARAM1,
							 IDC_COMBO_SET_RANGE_PARAM1,IDC_COMBO_SET_MUTUAL_PARAM1,
							 IDC_COMBO_SET_METHOD_PARAM1,IDC_COMBO_SET_TARGET_PARAM1,
							 IDC_COMBO_SET_ADDSUB_PARAM1,IDC_COMBO_SET_OPERATION_PARAM,
							 IDC_COMBO_SET_SCALING_PARAM,IDC_COMBO_SET_ABLE_PARAM2,
							 IDC_COMBO_SET_MEASURE_PARAM2,IDC_COMBO_SET_BASIC_PARAM2,
							 IDC_COMBO_SET_REFLECTION_PARAM2,IDC_COMBO_SET_MEDIAN_PARAM2,
							 IDC_COMBO_SET_LASER_GROUP_PARAM2,IDC_COMBO_SET_RANGE_PARAM2,
							 IDC_COMBO_SET_MUTUAL_PARAM2,IDC_COMBO_SET_METHOD_PARAM2,
							 IDC_COMBO_SET_TARGET_PARAM2};

const int SET_OPERATION_CHECK_IDS[OUT_NUM_MAX] =
								{IDC_CHECK_SET_OPERATION_PARAM1,IDC_CHECK_SET_OPERATION_PARAM2,
								 IDC_CHECK_SET_OPERATION_PARAM3,IDC_CHECK_SET_OPERATION_PARAM4,
								 IDC_CHECK_SET_OPERATION_PARAM5,IDC_CHECK_SET_OPERATION_PARAM6,
								 IDC_CHECK_SET_OPERATION_PARAM7,IDC_CHECK_SET_OPERATION_PARAM8,
								 IDC_CHECK_SET_OPERATION_PARAM9,IDC_CHECK_SET_OPERATION_PARAM10,
								 IDC_CHECK_SET_OPERATION_PARAM11,IDC_CHECK_SET_OPERATION_PARAM12};

IMPLEMENT_DYNAMIC(CDlgSetTab1, CDialog)

CDlgSetTab1::CDlgSetTab1(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetTab1::IDD, pParent)
{

}

CDlgSetTab1::~CDlgSetTab1()
{
}

void CDlgSetTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSetTab1, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_PANEL, &CDlgSetTab1::OnBnClickedButtonSetPanel)
	ON_BN_CLICKED(IDC_BUTTON_SET_TOLERANCE, &CDlgSetTab1::OnBnClickedButtonSetTolerance)
	ON_BN_CLICKED(IDC_BUTTON_SET_ABLE, &CDlgSetTab1::OnBnClickedButtonSetAble)
	ON_BN_CLICKED(IDC_BUTTON_SET_ABLE_MINMAX, &CDlgSetTab1::OnBnClickedButtonSetAbleMinmax)
	ON_BN_CLICKED(IDC_BUTTON_SET_MEASURE, &CDlgSetTab1::OnBnClickedButtonSetMeasure)
	ON_BN_CLICKED(IDC_BUTTON_SET_BASIC, &CDlgSetTab1::OnBnClickedButtonSetBasic)
	ON_BN_CLICKED(IDC_BUTTON_SET_NUM_ALARM, &CDlgSetTab1::OnBnClickedButtonSetNumAlarm)
	ON_BN_CLICKED(IDC_BUTTON_SET_RECOVERY, &CDlgSetTab1::OnBnClickedButtonSetRecovery)
	ON_BN_CLICKED(IDC_BUTTON_SET_LEVEL, &CDlgSetTab1::OnBnClickedButtonSetLevel)
	ON_BN_CLICKED(IDC_BUTTON_ABLE_START, &CDlgSetTab1::OnBnClickedButtonAbleStart)
	ON_BN_CLICKED(IDC_BUTTON_ABLE_STOP, &CDlgSetTab1::OnBnClickedButtonAbleStop)
	ON_BN_CLICKED(IDC_BUTTON_ABLE_CANCEL, &CDlgSetTab1::OnBnClickedButtonAbleCancel)
	ON_BN_CLICKED(IDC_BUTTON_SET_REFLECTION, &CDlgSetTab1::OnBnClickedButtonSetReflection)
	ON_BN_CLICKED(IDC_BUTTON_SET_MASK, &CDlgSetTab1::OnBnClickedButtonSetMask)
	ON_BN_CLICKED(IDC_BUTTON_SET_MEDIAN, &CDlgSetTab1::OnBnClickedButtonSetMedian)
	ON_BN_CLICKED(IDC_BUTTON_SET_LASER_GROUP, &CDlgSetTab1::OnBnClickedButtonSetLaserGroup)
	ON_BN_CLICKED(IDC_BUTTON_SET_RANGE, &CDlgSetTab1::OnBnClickedButtonSetRange)
	ON_BN_CLICKED(IDC_BUTTON_SET_MUTUAL, &CDlgSetTab1::OnBnClickedButtonSetMutual)
	ON_BN_CLICKED(IDC_BUTTON_SET_METHOD, &CDlgSetTab1::OnBnClickedButtonSetMethod)
	ON_BN_CLICKED(IDC_BUTTON_SET_TARGET, &CDlgSetTab1::OnBnClickedButtonSetTarget)
	ON_BN_CLICKED(IDC_BUTTON_SET_ADDSUB, &CDlgSetTab1::OnBnClickedButtonSetAddsub)
	ON_BN_CLICKED(IDC_BUTTON_SET_OPERATION, &CDlgSetTab1::OnBnClickedButtonSetOperation)
	ON_BN_CLICKED(IDC_BUTTON_SET_SCALING, &CDlgSetTab1::OnBnClickedButtonSetScaling)
END_MESSAGE_MAP()


void CDlgSetTab1::OnBnClickedButtonSetPanel()
{
	int upperDisp = 0;
	int lowerDisp = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_PANEL_PARAM1,upperDisp)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_PANEL_PARAM2,lowerDisp))
	{
		return;
	}
	if(((CComboBox*)GetDlgItem(IDC_COMBO_SET_PANEL_PARAM1))->GetCurSel() != CB_ERR)
	{
		upperDisp--;
	}
	if(((CComboBox*)GetDlgItem(IDC_COMBO_SET_PANEL_PARAM2))->GetCurSel() != CB_ERR)
	{
		lowerDisp--;
	}

	RC rc = LKIF2_SetPanel(upperDisp,lowerDisp);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetPanel"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetTolerance()
{
	int outNo = 0;
	int upperLimit = 0;
	int lowerLimit = 0;
	int hysteresis = 0;

	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_TOLERANCE_PARAM,outNo)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_TOLERANCE_PARAM1,upperLimit) ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_TOLERANCE_PARAM2,lowerLimit) ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_TOLERANCE_PARAM3,hysteresis))
	{
		return;
	}
	RC rc = LKIF2_SetTolerance(outNo,upperLimit,lowerLimit,hysteresis);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetTolerance"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetAble()
{
	int headNo = 0;
	int ableMode = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ABLE_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ABLE_PARAM2,ableMode))
	{
		return;
	}
	RC rc = LKIF2_SetAbleMode(headNo,(LKIF_ABLEMODE)ableMode);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetAbleMode"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetAbleMinmax()
{
	int headNo = 0;
	int min = 0;
	int max = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ABLE_MINMAX_PARAM,headNo)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_ABLE_MINMAX_PARAM1,min)	||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_ABLE_MINMAX_PARAM2,max))
	{
		return;
	}
	RC rc = LKIF2_SetAbleMinMax(headNo,min,max);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetAbleMinMax"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetMeasure()
{
	int headNo = 0;
	int measureMode  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MEASURE_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MEASURE_PARAM2,measureMode))
	{
		return;
	}
	RC rc = LKIF2_SetMeasureMode(headNo,(LKIF_MEASUREMODE)measureMode);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetMeasureMode"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetBasic()
{
	int headNo = 0;
	int basicPoint  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_BASIC_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_BASIC_PARAM2,basicPoint))
	{
		return;
	}
	
	RC rc = LKIF2_SetBasicPoint(headNo,(LKIF_BASICPOINT)basicPoint);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetBasicPoint"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetNumAlarm()
{
	int headNo = 0;
	int alarmNum  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_NUM_ALARM_PARAM,headNo)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_NUM_ALARM_PARAM,alarmNum))
	{
		return;
	}
	
	RC rc = LKIF2_SetNumAlarm(headNo,alarmNum);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetNumAlarm"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetRecovery()
{
	int headNo = 0;
	int recoreryNum  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_RECOVERY_PARAM,headNo)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_RECOVERY_PARAM,recoreryNum))
	{
		return;
	}
	RC rc = LKIF2_SetNumRecovery(headNo,recoreryNum);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetNumRecovery"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetLevel()
{
	int headNo = 0;
	int alarmLevel = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_LEVEL_PARAM,headNo)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_LEVEL_PARAM,alarmLevel))
	{
		return;
	}
	RC rc = LKIF2_SetAlarmLevel(headNo,alarmLevel);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetAlarmLevel"),rc);
}

void CDlgSetTab1::OnBnClickedButtonAbleStart()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_ABLE_START_PARAM,headNo))
	{
		return;
	}
	RC rc = LKIF2_AbleStart(headNo);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_AbleStart"),rc);
}

void CDlgSetTab1::OnBnClickedButtonAbleStop()
{
	RC rc = LKIF2_AbleStop();
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_AbleStop"),rc);
}

void CDlgSetTab1::OnBnClickedButtonAbleCancel()
{
	RC rc = LKIF2_AbleCancel();
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_AbleCancel"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetReflection()
{
	int headNo = 0;
	int reflectionMode  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_REFLECTION_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_REFLECTION_PARAM2,reflectionMode))
	{
		return;
	}
	RC rc = LKIF2_SetReflectionMode(headNo,(LKIF_REFLECTIONMODE)reflectionMode);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetReflectionMode"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetMask()
{
	int headNo = 0;
	BOOL onOff = 0;
	int pos1 = 0;
	int pos2 = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MASK_PARAM1,headNo)||
	   !GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_MASK_PARAM2,onOff) ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_MASK_PARAM1,pos1)	  ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_MASK_PARAM2,pos2))
	{
		return;
	}
	RC rc = LKIF2_SetMask(headNo,onOff,pos1,pos2);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetMask"),rc);	
}

void CDlgSetTab1::OnBnClickedButtonSetMedian()
{
	int headNo = 0;
	int median  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MEDIAN_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MEDIAN_PARAM2,median))
	{
		return;
	}
	RC rc = LKIF2_SetMedian(headNo,(LKIF_MEDIAN)median);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetMedian"),rc);	
}

void CDlgSetTab1::OnBnClickedButtonSetLaserGroup()
{
	int headNo = 0;
	int laserCtrlGroup  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_LASER_GROUP_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_LASER_GROUP_PARAM2,laserCtrlGroup))
	{
		return;
	}
	RC rc = LKIF2_SetLaserCtrlGroup(headNo,(LKIF_LASER_CTRL_GROUP)laserCtrlGroup);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetLaserCtrlGroup"),rc);	
}

void CDlgSetTab1::OnBnClickedButtonSetRange()
{
	int headNo = 0;
	int range  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_RANGE_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_RANGE_PARAM2,range))
	{
		return;
	}
	RC rc = LKIF2_SetRange(headNo,(LKIF_RANGE)range);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetRange"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetMutual()
{
	int headNo = 0;
	int group  = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MUTUAL_PARAM1,headNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MUTUAL_PARAM2,group))
	{
		return;
	}
	RC rc = LKIF2_SetMutualInterferencePreventionGroup(headNo,(LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP)group);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetMutualInterferencePreventionGroup"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetMethod()
{
	int outNo = 0;
	int calcMethod = 0;
	int headOutNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_METHOD_PARAM1,outNo)		||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_METHOD_PARAM2,calcMethod)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_METHOD_PARAM,headOutNo))
	{
		return;
	}
	RC rc = LKIF2_SetCalcMethod(outNo,(LKIF_CALCMETHOD)calcMethod,headOutNo);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetCalcMethod"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetTarget()
{
	int outNo = 0;
	int calcTarget = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_TARGET_PARAM1,outNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_TARGET_PARAM2,calcTarget))
	{
		return;
	}
	RC rc = LKIF2_SetCalcTarget(outNo,(LKIF_CALCTARGET)calcTarget);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetCalcTarget"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetAddsub()
{
	int outNo = 0;
	int addSub1 = 0;
	int addSub2 = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ADDSUB_PARAM1,outNo)	  ||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ADDSUB_PARAM2,addSub1) ||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ADDSUB_PARAM3,addSub2))
	{
		return;
	}
	RC rc = LKIF2_SetOutAddSub(outNo,addSub1,addSub2);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetOutAddSub"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetOperation()
{
	int outNo = 0;
	int targetOut = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_OPERATION_PARAM,outNo) ||
	   !GET_ITEM_FROM_CHECKS(this,SET_OPERATION_CHECK_IDS,OUT_NUM_MAX,targetOut))
	{
		return;
	}
	RC rc = LKIF2_SetOutOperation(outNo,(LKIF_OUTNO)targetOut);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetOutOperation"),rc);
}

void CDlgSetTab1::OnBnClickedButtonSetScaling()
{
	int outNo = 0;
	int inputValue1 = 0;
	int outputValue1 = 0;
	int inputValue2 = 0;
	int outputValue2 = 0;

	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_SCALING_PARAM,outNo)		  ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_SCALING_PARAM1,inputValue1) ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_SCALING_PARAM2,outputValue1)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_SCALING_PARAM3,inputValue2) ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_SCALING_PARAM4,outputValue2))
	{
		return;
	}
	RC rc = LKIF2_SetScaling(outNo,inputValue1,outputValue1,inputValue2,outputValue2);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetScaling"),rc);
}

BOOL CDlgSetTab1::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int i = 0;i < SET_TAB1_COMBO_NUM;i++)
	{
		((CComboBox*)GetDlgItem(SET_TAB1_COMBO_IDS[i]))->SetCurSel(0);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_SET_PANEL_PARAM2))->SetCurSel(1);
	((CComboBox*)GetDlgItem(IDC_COMBO_SET_MASK_PARAM2))->SetCurSel(1);
	((CComboBox*)GetDlgItem(IDC_COMBO_SET_ADDSUB_PARAM2))->SetCurSel(1);
	((CComboBox*)GetDlgItem(IDC_COMBO_SET_ADDSUB_PARAM3))->SetCurSel(1);

	SetDlgItemText(IDC_EDIT_SET_TOLERANCE_PARAM1,_T("50000"));
	SetDlgItemText(IDC_EDIT_SET_TOLERANCE_PARAM2,_T("-50000"));
	SetDlgItemText(IDC_EDIT_SET_TOLERANCE_PARAM3,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_ABLE_MINMAX_PARAM1,_T("1"));
	SetDlgItemText(IDC_EDIT_SET_ABLE_MINMAX_PARAM2,_T("99"));
	SetDlgItemText(IDC_EDIT_SET_NUM_ALARM_PARAM,_T("8"));
	SetDlgItemText(IDC_EDIT_SET_RECOVERY_PARAM,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_LEVEL_PARAM,_T("4"));
	SetDlgItemText(IDC_EDIT_SET_MASK_PARAM1,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_MASK_PARAM2,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_METHOD_PARAM,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_SCALING_PARAM1,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_SCALING_PARAM2,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_SCALING_PARAM3,_T("1000"));	
	SetDlgItemText(IDC_EDIT_SET_SCALING_PARAM4,_T("1000"));

	return TRUE;
}

void CDlgSetTab1::OnOK()
{
}

void CDlgSetTab1::OnCancel()
{
}
