#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "DlgGetTab1.h"
#include "LkIF2TestVCDlg.h"
#include "LKIF2Tools.h"

const int GET_TAB1_COMBO_NUM = 19;
const int GET_TAB1_COMBO_IDS[GET_TAB1_COMBO_NUM] =
							{IDC_COMBO_GET_TOLERANCE_PARAM,IDC_COMBO_GET_ABLE_PARAM,
							 IDC_COMBO_GET_ABLE_MINMAX_PARAM,IDC_COMBO_GET_MEASURE_PARAM,
							 IDC_COMBO_GET_BASIC_PARAM,IDC_COMBO_GET_NUM_ALARM_PARAM,
							 IDC_COMBO_GET_RECOVERY_PARAM,IDC_COMBO_GET_LEVEL_PARAM,
							 IDC_COMBO_GET_REFLECTION_PARAM,IDC_COMBO_GET_MASK_PARAM,
							 IDC_COMBO_GET_MEDIAN_PARAM,IDC_COMBO_GET_LASER_GROUP_PARAM,
							 IDC_COMBO_GET_RANGE_PARAM,IDC_COMBO_GET_MUTUAL_PARAM,
							 IDC_COMBO_GET_METHOD_PARAM,IDC_COMBO_GET_TARGET_PARAM,
							 IDC_COMBO_GET_ADDSUB_PARAM,IDC_COMBO_GET_OPERATION_PARAM,
							 IDC_COMBO_GET_SCALING_PARAM};

const int GET_OPERATION_CHECK_IDS[OUT_NUM_MAX] =
								{IDC_CHECK_GET_OPERATION_PARAM1,IDC_CHECK_GET_OPERATION_PARAM2,
								 IDC_CHECK_GET_OPERATION_PARAM3,IDC_CHECK_GET_OPERATION_PARAM4,
								 IDC_CHECK_GET_OPERATION_PARAM5,IDC_CHECK_GET_OPERATION_PARAM6,
								 IDC_CHECK_GET_OPERATION_PARAM7,IDC_CHECK_GET_OPERATION_PARAM8,
								 IDC_CHECK_GET_OPERATION_PARAM9,IDC_CHECK_GET_OPERATION_PARAM10,
								 IDC_CHECK_GET_OPERATION_PARAM11,IDC_CHECK_GET_OPERATION_PARAM12};

IMPLEMENT_DYNAMIC(CDlgGetTab1, CDialog)

CDlgGetTab1::CDlgGetTab1(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGetTab1::IDD, pParent)
{

}

CDlgGetTab1::~CDlgGetTab1()
{
}

void CDlgGetTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGetTab1, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_GET_PANEL, &CDlgGetTab1::OnBnClickedButtonGetPanel)
	ON_BN_CLICKED(IDC_BUTTON_GET_TOLERANCE, &CDlgGetTab1::OnBnClickedButtonGetTolerance)
	ON_BN_CLICKED(IDC_BUTTON_GET_ABLE, &CDlgGetTab1::OnBnClickedButtonGetAble)
	ON_BN_CLICKED(IDC_BUTTON_GET_ABLE_MINMAX, &CDlgGetTab1::OnBnClickedButtonGetAbleMinmax)
	ON_BN_CLICKED(IDC_BUTTON_GET_MEASURE, &CDlgGetTab1::OnBnClickedButtonGetMeasure)
	ON_BN_CLICKED(IDC_BUTTON_GET_BASIC, &CDlgGetTab1::OnBnClickedButtonGetBasic)
	ON_BN_CLICKED(IDC_BUTTON_GET_NUM_ALARM, &CDlgGetTab1::OnBnClickedButtonGetNumAlarm)
	ON_BN_CLICKED(IDC_BUTTON_GET_RECOVERY, &CDlgGetTab1::OnBnClickedButtonGetRecovery)
	ON_BN_CLICKED(IDC_BUTTON_GET_LEVEL, &CDlgGetTab1::OnBnClickedButtonGetLevel)
	ON_BN_CLICKED(IDC_BUTTON_GET_REFLECTION, &CDlgGetTab1::OnBnClickedButtonGetReflection)
	ON_BN_CLICKED(IDC_BUTTON_GET_MASK, &CDlgGetTab1::OnBnClickedButtonGetMask)
	ON_BN_CLICKED(IDC_BUTTON_GET_MEDIAN, &CDlgGetTab1::OnBnClickedButtonGetMedian)
	ON_BN_CLICKED(IDC_BUTTON_GET_LASER_GROUP, &CDlgGetTab1::OnBnClickedButtonGetLaserGroup)
	ON_BN_CLICKED(IDC_BUTTON_GET_RANGE, &CDlgGetTab1::OnBnClickedButtonGetRange)
	ON_BN_CLICKED(IDC_BUTTON_GET_MUTUAL, &CDlgGetTab1::OnBnClickedButtonGetMutual)
	ON_BN_CLICKED(IDC_BUTTON_GET_METHOD, &CDlgGetTab1::OnBnClickedButtonGetMethod)
	ON_BN_CLICKED(IDC_BUTTON_GET_TARGET, &CDlgGetTab1::OnBnClickedButtonGetTarget)
	ON_BN_CLICKED(IDC_BUTTON_GET_ADDSUB, &CDlgGetTab1::OnBnClickedButtonGetAddsub)
	ON_BN_CLICKED(IDC_BUTTON_GET_OPERATION, &CDlgGetTab1::OnBnClickedButtonGetOperation)
	ON_BN_CLICKED(IDC_BUTTON_GET_SCALING, &CDlgGetTab1::OnBnClickedButtonGetScaling)
END_MESSAGE_MAP()


void CDlgGetTab1::OnBnClickedButtonGetPanel()
{
	int upperDisp = 0;
	int lowerDisp = 0;
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	RC rc = LKIF2_GetPanel(&upperDisp,&lowerDisp);
	if(IS_RC_SUCCESS(rc))
	{
		upperDisp = upperDisp+1;
		lowerDisp = lowerDisp+1;
		SetDlgItemText(IDC_STATIC_GET_PANEL_PARAM1,
						parent->GetTextFromCombo(IDC_COMBO_SET_PANEL_PARAM1,upperDisp));
		SetDlgItemText(IDC_STATIC_GET_PANEL_PARAM2,
					   parent->GetTextFromCombo(IDC_COMBO_SET_PANEL_PARAM2,lowerDisp));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetPanel"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetTolerance()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_TOLERANCE_PARAM,outNo))
	{
		return;
	}
	int upperLimit = 0;
	int lowerLimit = 0;
	int hysteresis = 0;
	RC rc = LKIF2_GetTolerance(outNo,&upperLimit,&lowerLimit,&hysteresis);
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),upperLimit);
		SetDlgItemText(IDC_STATIC_GET_TOLERANCE_PARAM1,temp);
		temp.Format(_T("%d"),lowerLimit);
		SetDlgItemText(IDC_STATIC_GET_TOLERANCE_PARAM2,temp);
		temp.Format(_T("%d"),hysteresis);
		SetDlgItemText(IDC_STATIC_GET_TOLERANCE_PARAM3,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetTolerance"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetAble()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_ABLE_PARAM,headNo))
	{
		return;
	}

	LKIF_ABLEMODE ableMode = LKIF_ABLEMODE_AUTO;
	RC rc = LKIF2_GetAbleMode(headNo,&ableMode);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_ABLE_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_ABLE_PARAM2,ableMode));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetAbleMode"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetAbleMinmax()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_ABLE_MINMAX_PARAM,headNo))
	{
		return;
	}
	int min = 0;
	int max = 0;
	RC rc = LKIF2_GetAbleMinMax(headNo,&min,&max);
	
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),min);
		SetDlgItemText(IDC_STATIC_GET_ABLE_MINMAX_PARAM1,temp);
		temp.Format(_T("%d"),max);
		SetDlgItemText(IDC_STATIC_GET_ABLE_MINMAX_PARAM2,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetAbleMinMax"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetMeasure()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_MEASURE_PARAM,headNo))
	{
		return;
	}
	LKIF_MEASUREMODE measureMode = LKIF_MEASUREMODE_NORMAL;
	RC rc = LKIF2_GetMeasureMode(headNo,&measureMode);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_MEASURE_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_MEASURE_PARAM2,measureMode));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetMeasureMode"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetBasic()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_BASIC_PARAM,headNo))
	{
		return;
	}
	LKIF_BASICPOINT basicPoint = LKIF_BASICPOINT_NEAR;
	RC rc = LKIF2_GetBasicPoint(headNo,&basicPoint);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_BASIC_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_BASIC_PARAM2,basicPoint));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetBasicPoint"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetNumAlarm()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_NUM_ALARM_PARAM,headNo))
	{
		return;
	}
	int alarmNum = 0;
	RC rc = LKIF2_GetNumAlarm(headNo,&alarmNum);
	
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),alarmNum);
		SetDlgItemText(IDC_STATIC_GET_NUM_ALARM_PARAM,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetNumAlarm"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetRecovery()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_RECOVERY_PARAM,headNo))
	{
		return;
	}
	int recoveryNum = 0;
	RC rc = LKIF2_GetNumRecovery(headNo,&recoveryNum);
	
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),recoveryNum);
		SetDlgItemText(IDC_STATIC_GET_RECOVERY_PARAM,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetNumRecovery"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetLevel()
{
	int headNo =0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_LEVEL_PARAM,headNo))
	{
		return;
	}
	int alarmLevel = 0;
	RC rc = LKIF2_GetAlarmLevel(headNo,&alarmLevel);
	
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),alarmLevel);
		SetDlgItemText(IDC_STATIC_GET_LEVEL_PARAM,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetAlarmLevel"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetReflection()
{
	int headNo =0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_REFLECTION_PARAM,headNo))
	{
		return;
	}
	LKIF_REFLECTIONMODE reflectionMode = LKIF_REFLECTIONMODE_DIFFUSION;
	RC rc = LKIF2_GetReflectionMode(headNo,&reflectionMode);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_REFLECTION_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_REFLECTION_PARAM2,reflectionMode));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetReflectionMode"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetMask()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_MASK_PARAM,headNo))
	{
		return;
	}
	BOOL onOff = 0;
	int pos1 = 0;
	int pos2 = 0;
	RC rc = LKIF2_GetMask(headNo,&onOff,&pos1,&pos2);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		onOff = (onOff)?0:1;
		SetDlgItemText(IDC_STATIC_GET_MASK_PARAM1,
					   parent->GetTextFromCombo(IDC_COMBO_SET_MASK_PARAM2,onOff));
		CString temp(_T(""));
		temp.Format(_T("%d"),pos1);
		SetDlgItemText(IDC_STATIC_GET_MASK_PARAM2,temp);
		temp.Format(_T("%d"),pos2);
		SetDlgItemText(IDC_STATIC_GET_MASK_PARAM3,temp);
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetMask"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetMedian()
{
	int headNo =0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_MEDIAN_PARAM,headNo))
	{
		return;
	}
	LKIF_MEDIAN median = LKIF_MEDIAN_OFF;
	RC rc = LKIF2_GetMedian(headNo,&median);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_MEDIAN_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_MEDIAN_PARAM2,median));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetMedian"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetLaserGroup()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_LASER_GROUP_PARAM,headNo))
	{
		return;
	}
	LKIF_LASER_CTRL_GROUP laserCtrlGroup = LKIF_LASER_CTRL_GROUP_1;
	RC rc = LKIF2_GetLaserCtrlGroup(headNo,&laserCtrlGroup);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_LASER_GROUP_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_LASER_GROUP_PARAM2,laserCtrlGroup));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetLaserCtrlGroup"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetRange()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_RANGE_PARAM,headNo))
	{
		return;
	}
	LKIF_RANGE range = LKIF_RANGE_CENTER; 
	RC rc = LKIF2_GetRange(headNo,&range);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_RANGE_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_RANGE_PARAM2,range));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetRange"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetMutual()
{
	int headNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_MUTUAL_PARAM,headNo))
	{
		return;
	}
	LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP group = LKIF_MUTUAL_INTERFERENCE_PREVENTION_GROUP_A;
	RC rc = LKIF2_GetMutualInterferencePreventionGroup(headNo,&group);
	
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_MUTUAL_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_MUTUAL_PARAM2,group));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetMutualInterferencePreventionGroup"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetMethod()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_METHOD_PARAM,outNo))
	{
		return;
	}
	LKIF_CALCMETHOD calcMethod = LKIF_CALCMETHOD_HEAD;
	int headOutNo = 0;
	RC rc = LKIF2_GetCalcMethod(outNo,&calcMethod,&headOutNo);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_METHOD_PARAM1,
					   parent->GetTextFromCombo(IDC_COMBO_SET_METHOD_PARAM2,calcMethod));
		CString temp(_T(""));
		temp.Format(_T("%d"),headOutNo);
		SetDlgItemText(IDC_STATIC_GET_METHOD_PARAM2,temp);
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetCalcMethod"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetTarget()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_TARGET_PARAM,outNo))
	{
		return;
	}
	LKIF_CALCTARGET calcTarget = LKIF_CALCTARGET_PEAK_1;
	RC rc = LKIF2_GetCalcTarget(outNo,&calcTarget);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_TARGET_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_TARGET_PARAM2,calcTarget));
	}	
	parent->DisplayCommandResult(_T("LKIF2_GetCalcTarget"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetAddsub()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_ADDSUB_PARAM,outNo))
	{
		return;
	}
	int addSub1 = 0;
	int addSub2 = 0;
	RC rc = LKIF2_GetOutAddSub(outNo,&addSub1,&addSub2);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_ADDSUB_PARAM1,
					  parent->GetTextFromCombo(IDC_COMBO_SET_ADDSUB_PARAM2,addSub1));
		SetDlgItemText(IDC_STATIC_GET_ADDSUB_PARAM2,
					  parent->GetTextFromCombo(IDC_COMBO_SET_ADDSUB_PARAM3,addSub2));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetOutAddSub"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetOperation()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_OPERATION_PARAM,outNo))
	{
		return;
	}
	LKIF_OUTNO targetOut;
	RC rc = LKIF2_GetOutOperation(outNo,&targetOut);
	if(IS_RC_SUCCESS(rc))
	{
		 
		for(int index = 0; index < OUT_NUM_MAX;index++)
		{
			CButton* check = (CButton*)GetDlgItem(GET_OPERATION_CHECK_IDS[index]);
			if(	(targetOut & GET_LKIF_OUT_NUM(index)) == GET_LKIF_OUT_NUM(index))
			{
				
				check->SetCheck(BST_CHECKED);	
			}
			else
			{
				check->SetCheck(BST_UNCHECKED);
			}
		}
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetOutOperation"),rc);
}

void CDlgGetTab1::OnBnClickedButtonGetScaling()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_SCALING_PARAM,outNo))
	{
		return;
	}
	int inputValue1 = 0;
	int outputValue1 = 0;
	int inputValue2 = 0;
	int outputValue2 = 0;
	RC rc = LKIF2_GetScaling(outNo,&inputValue1,&outputValue1,&inputValue2,&outputValue2);
	
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),inputValue1);
		SetDlgItemText(IDC_STATIC_GET_SCALING_PARAM1,temp);
		temp.Format(_T("%d"),outputValue1);
		SetDlgItemText(IDC_STATIC_GET_SCALING_PARAM2,temp);
		temp.Format(_T("%d"),inputValue2);
		SetDlgItemText(IDC_STATIC_GET_SCALING_PARAM3,temp);
		temp.Format(_T("%d"),outputValue2);
		SetDlgItemText(IDC_STATIC_GET_SCALING_PARAM4,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetScaling"),rc);
}

BOOL CDlgGetTab1::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int i = 0;i < GET_TAB1_COMBO_NUM;i++)
	{
		((CComboBox*)GetDlgItem(GET_TAB1_COMBO_IDS[i]))->SetCurSel(0);
	}
	for(int i = 0;i < OUT_NUM_MAX;i++)
	{
		((CButton*)GetDlgItem(GET_OPERATION_CHECK_IDS[i]))->EnableWindow(false);
	}
	return TRUE;
}

void CDlgGetTab1::OnOK()
{
}

void CDlgGetTab1::OnCancel()
{
}
