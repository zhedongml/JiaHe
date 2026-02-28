#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "DlgGetTab2.h"
#include "LkIF2TestVCDlg.h"
#include "LKIF2Tools.h"

const int GET_TAB2_COMBO_NUM = 10;
const int GET_TAB2_COMBO_IDS[GET_TAB2_COMBO_NUM] =
							{IDC_COMBO_GET_FILTER_PARAM,IDC_COMBO_GET_TRIGGER_PARAM,
							 IDC_COMBO_GET_OFFSET_PARAM,IDC_COMBO_GET_CALCMODE_PARAM,
							 IDC_COMBO_GET_ANALOG_SCALING_PARAM,IDC_COMBO_GET_UNIT_PARAM,
							 IDC_COMBO_GET_MEASURETYPE_PARAM,IDC_COMBO_GET_SYNC_PARAM,
							 IDC_COMBO_GET_STORAGE_TARGET_PARAM,IDC_COMBO_GET_ANALOG_CH_PARAM};

IMPLEMENT_DYNAMIC(CDlgGetTab2, CDialog)

CDlgGetTab2::CDlgGetTab2(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGetTab2::IDD, pParent)
{

}

CDlgGetTab2::~CDlgGetTab2()
{
}

void CDlgGetTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGetTab2, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_GET_NUM_CH, &CDlgGetTab2::OnBnClickedButtonGetNumCh)
	ON_BN_CLICKED(IDC_BUTTON_GET_FILTER, &CDlgGetTab2::OnBnClickedButtonGetFilter)
	ON_BN_CLICKED(IDC_BUTTON_GET_TRIGGER, &CDlgGetTab2::OnBnClickedButtonGetTrigger)
	ON_BN_CLICKED(IDC_BUTTON_GET_OFFSET, &CDlgGetTab2::OnBnClickedButtonGetOffset)
	ON_BN_CLICKED(IDC_BUTTON_GET_CALCMODE, &CDlgGetTab2::OnBnClickedButtonGetCalcmode)
	ON_BN_CLICKED(IDC_BUTTON_GET_ANALOG_SCALING, &CDlgGetTab2::OnBnClickedButtonGetAnalogScaling)
	ON_BN_CLICKED(IDC_BUTTON_GET_UNIT, &CDlgGetTab2::OnBnClickedButtonGetUnit)
	ON_BN_CLICKED(IDC_BUTTON_GET_MEASURETYPE, &CDlgGetTab2::OnBnClickedButtonGetMeasuretype)
	ON_BN_CLICKED(IDC_BUTTON_GET_STORAGE_TARGET, &CDlgGetTab2::OnBnClickedButtonGetStorageTarget)
	ON_BN_CLICKED(IDC_BUTTON_GET_SAMPLING, &CDlgGetTab2::OnBnClickedButtonGetSampling)
	ON_BN_CLICKED(IDC_BUTTON_GET_MUTUAL_PREV, &CDlgGetTab2::OnBnClickedButtonGetMutualPrev)
	ON_BN_CLICKED(IDC_BUTTON_GET_TOLERANCE_FORMAT, &CDlgGetTab2::OnBnClickedButtonGetToleranceFormat)
	ON_BN_CLICKED(IDC_BUTTON_GET_STROBE, &CDlgGetTab2::OnBnClickedButtonGetStrobe)
	ON_BN_CLICKED(IDC_BUTTON_GET_STORAGE, &CDlgGetTab2::OnBnClickedButtonGetStorage)
	ON_BN_CLICKED(IDC_BUTTON_GET_ANALOG_CH, &CDlgGetTab2::OnBnClickedButtonGetAnalogCh)
	ON_BN_CLICKED(IDC_BUTTON_GET_ALARM_FORM, &CDlgGetTab2::OnBnClickedButtonGetAlarmForm)
	ON_BN_CLICKED(IDC_BUTTON_GET_HEAD, &CDlgGetTab2::OnBnClickedButtonGetHead)
	ON_BN_CLICKED(IDC_BUTTON_GET_OUT, &CDlgGetTab2::OnBnClickedButtonGetOut)
	ON_BN_CLICKED(IDC_BUTTON_GET_SYNC, &CDlgGetTab2::OnBnClickedButtonGetSync)
END_MESSAGE_MAP()

void CDlgGetTab2::OnBnClickedButtonGetFilter()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_FILTER_PARAM,outNo))
	{
		return;
	}
	LKIF_FILTERMODE filterMode = LKIF_FILTERMODE_MOVING_AVERAGE;
	LKIF_FILTERPARA filterPara = LKIF_FILTERPARA_AVE_1;

	RC rc = LKIF2_GetFilter(outNo,&filterMode,&filterPara);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_FILTER_PARAM1,
					   parent->GetTextFromCombo(IDC_COMBO_SET_FILTER_PARAM2,filterMode));
		SetDlgItemText(IDC_STATIC_GET_FILTER_PARAM2,GET_FILTER_PARAM_STR(filterMode,filterPara));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetFilter"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetTrigger()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_TRIGGER_PARAM,outNo))
	{
		return;
	}

	LKIF_TRIGGERMODE triggerMode = LKIF_TRIGGERMODE_EXT1;
	RC rc = LKIF2_GetTriggerMode(outNo,&triggerMode);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();

	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_TRIGGER_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_TRIGGER_PARAM2,triggerMode));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetTriggerMode"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetOffset()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_OFFSET_PARAM,outNo))
	{
		return;
	}

	int offset = 0;
	RC rc = LKIF2_GetOffset(outNo,&offset);

	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),offset);
		SetDlgItemText(IDC_STATIC_GET_OFFSET_PARAM,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetOffset"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetCalcmode()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_CALCMODE_PARAM,outNo))
	{
		return;
	}

	LKIF_CALCMODE calcMode = LKIF_CALCMODE_NORMAL;
	RC rc = LKIF2_GetCalcMode(outNo,&calcMode);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();

	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_CALCMODE_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_CALCMODE_PARAM2,calcMode));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetCalcMode"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetAnalogScaling()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_ANALOG_SCALING_PARAM,outNo))
	{
		return;
	}

	int inputValue1 = 0;
	int outputVoltage1 = 0;
	int inputValue2 = 0;
	int outputVoltage2 = 0;
	RC rc = LKIF2_GetAnalogScaling(outNo,&inputValue1,&outputVoltage1,&inputValue2,&outputVoltage2);
	
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),inputValue1);
		SetDlgItemText(IDC_STATIC_GET_ANALOG_SCALING_PARAM1,temp);
		temp.Format(_T("%d"),outputVoltage1);
		SetDlgItemText(IDC_STATIC_GET_ANALOG_SCALING_PARAM2,temp);
		temp.Format(_T("%d"),inputValue2);
		SetDlgItemText(IDC_STATIC_GET_ANALOG_SCALING_PARAM3,temp);
		temp.Format(_T("%d"),outputVoltage2);
		SetDlgItemText(IDC_STATIC_GET_ANALOG_SCALING_PARAM4,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetAnalogScaling"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetUnit()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_UNIT_PARAM,outNo))
	{
		return;
	}

	LKIF_DISPLAYUNIT displayUnit = LKIF_DISPLAYUNIT_0000_01MM;
	RC rc1 =LKIF2_GetDisplayUnit(outNo, &displayUnit);
	LKIF_MEASURETYPE measureType = LKIF_MEASURETYPE_DISPLACEMENT;
	RC rc2 = LKIF2_GetMeasureType(outNo,&measureType);
	
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	
	if(IS_RC_SUCCESS(rc1) && IS_RC_SUCCESS(rc2))
	{
		SetDlgItemText(IDC_STATIC_GET_UNIT_PARAM,GET_UNIT_STR(displayUnit,measureType));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetDisplayUnit"),rc1);
}

void CDlgGetTab2::OnBnClickedButtonGetMeasuretype()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_MEASURETYPE_PARAM,outNo))
	{
		return;
	}

	LKIF_MEASURETYPE measureType = LKIF_MEASURETYPE_DISPLACEMENT;
	RC rc = LKIF2_GetMeasureType(outNo,&measureType);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_MEASURETYPE_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_MEASURETYPE_PARAM2,measureType));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetMeasureType"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetSync()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_SYNC_PARAM,outNo))
	{
		return;
	}
	BOOL onOff = 0;
	RC rc = LKIF2_GetSynchronization(outNo,&onOff);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_SYNC_PARAM,
						parent->GetTextFromCombo(IDC_COMBO_SET_SYNC_PARAM2,(onOff?0:1)));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetSynchronization"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetStorageTarget()
{	
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_STORAGE_TARGET_PARAM,outNo))
	{
		return;
	}
	BOOL onOff = 0;
	RC rc = LKIF2_GetStorageTarget(outNo,&onOff);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_STORAGE_TARGET_PARAM,
						parent->GetTextFromCombo(IDC_COMBO_SET_STORAGE_TARGET_PARAM2,(onOff?0:1)));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetStorageTarget"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetSampling()
{
	LKIF_SAMPLINGCYCLE samplingCycle = LKIF_SAMPLINGCYCLE_2_55USEC; 
	RC rc = LKIF2_GetSamplingCycle(&samplingCycle);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_SAMPLING_PARAM,
						parent->GetTextFromCombo(IDC_COMBO_SET_SAMPLING_PARAM,samplingCycle));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetSamplingCycle"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetMutualPrev()
{
	LKIF_MUTUAL_INTERFERENCE_PREVENTION onOff = LKIF_MUTUAL_INTERFERENCE_PREVENTION_OFF;
	RC rc = LKIF2_GetMutualInterferencePrevention(&onOff);
	
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_MUTUAL_PREV_PARAM,
						parent->GetTextFromCombo(IDC_COMBO_SET_MUTUAL_PREV_PARAM,onOff));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetMutualInterferencePrevention"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetToleranceFormat()
{
	LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT outputFormat = LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT_NORMAL;
	RC rc = LKIF2_GetToleranceComparatorOutputFormat(&outputFormat);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_TOLERANCE_FORMAT_PARAM,
						parent->GetTextFromCombo(IDC_COMBO_SET_TOLERANCE_FORMAT_PARAM,outputFormat));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetToleranceComparatorOutputFormat"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetStrobe()
{
	LKIF_STOROBETIME strobeTime = LKIF_STOROBETIME_2MS;
	RC rc = LKIF2_GetStrobeTime(&strobeTime);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_STROBE_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_STROBE_PARAM,strobeTime));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetStrobeTime"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetStorage()
{
	int numStorage = 0;
	LKIF_STORAGECYCLE storageCycle = LKIF_STORAGECYCLE_1;
	RC rc = LKIF2_GetDataStorage(&numStorage,&storageCycle);

	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),numStorage);
		SetDlgItemText(IDC_STATIC_GET_STORAGE_PARAM1,temp);
		SetDlgItemText(IDC_STATIC_GET_STORAGE_PARAM2,
					   parent->GetTextFromCombo(IDC_COMBO_SET_STORAGE_PARAM,storageCycle));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetDataStorage"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetAnalogCh()
{
	int chNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_ANALOG_CH_PARAM,chNo))
	{
		return;
	}
	int outNo = 0;
	RC rc = LKIF2_GetAnalogChannel(chNo,&outNo);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_ANALOG_CH_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_ANALOG_CH_PARAM2,outNo));
	}
	parent->DisplayCommandResult(_T("LKIF2_GetAnalogChannel"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetAlarmForm()
{
	LKIF_ALARM_OUTPUT_FORM  alarmOutputForm = LKIF_ALARM_OUTPUT_FORM_SYSTEM;
	RC rc = LKIF2_GetAlarmOutputForm(&alarmOutputForm);
	CLkIF2TestVCDlg* parent = (CLkIF2TestVCDlg*)GetParent();
	
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_ALARM_FORM_PARAM,
					   parent->GetTextFromCombo(IDC_COMBO_SET_ALARM_FORM_PARAM,alarmOutputForm));
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetAlarmOutputForm"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetHead()
{
	int numOfUsedHeads = 0;
	RC rc = LKIF2_GetNumOfUsedHeads(&numOfUsedHeads);
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),numOfUsedHeads);
		SetDlgItemText(IDC_STATIC_GET_HEAD_PARAM,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetNumOfUsedHeads"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetOut()
{
	int numOfUsedOut = 0;
	RC rc = LKIF2_GetNumOfUsedOut(&numOfUsedOut);
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),numOfUsedOut);
		SetDlgItemText(IDC_STATIC_GET_OUT_PARAM,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetNumOfUsedOut"),rc);
}

void CDlgGetTab2::OnBnClickedButtonGetNumCh()
{
	int numOfUsedAnalogCh = 0;
	RC rc = LKIF2_GetNumOfUsedAnalogCh(&numOfUsedAnalogCh);
	if(IS_RC_SUCCESS(rc))
	{
		CString temp(_T(""));
		temp.Format(_T("%d"),numOfUsedAnalogCh);
		SetDlgItemText(IDC_STATIC_GET_NUM_CH,temp);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetNumOfUsedAnalogCh"),rc);
}


BOOL CDlgGetTab2::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(int i = 0;i < GET_TAB2_COMBO_NUM;i++)
	{
		((CComboBox*)GetDlgItem(GET_TAB2_COMBO_IDS[i]))->SetCurSel(0);
	}
	return TRUE;
}


void CDlgGetTab2::OnOK()
{
}

void CDlgGetTab2::OnCancel()
{
}
