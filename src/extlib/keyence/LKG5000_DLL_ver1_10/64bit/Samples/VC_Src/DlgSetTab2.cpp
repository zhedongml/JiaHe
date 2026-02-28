#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "DlgSetTab2.h"
#include "LkIF2TestVCDlg.h"
#include "LKIF2Tools.h"

const int SET_TAB2_COMBO_NUM = 23;
const int SET_TAB2_COMBO_IDS[SET_TAB2_COMBO_NUM] =
							{IDC_COMBO_SET_FILTER_PARAM1,IDC_COMBO_SET_TRIGGER_PARAM1,
							 IDC_COMBO_SET_OFFSET_PARAM,IDC_COMBO_SET_CALCMODE_PARAM1,
							 IDC_COMBO_SET_ANALOG_SCALING_PARAM,IDC_COMBO_SET_UNIT_PARAM1,
							 IDC_COMBO_SET_MEASURETYPE_PARAM1,IDC_COMBO_SET_SYNC_PARAM1,
							 IDC_COMBO_SET_STORAGE_TARGET_PARAM1,IDC_COMBO_SET_MUTUAL_PREV_PARAM,
							 IDC_COMBO_SET_TOLERANCE_FORMAT_PARAM,IDC_COMBO_SET_STROBE_PARAM,
							 IDC_COMBO_SET_STORAGE_PARAM,IDC_COMBO_SET_ANALOG_CH_PARAM1,
							 IDC_COMBO_SET_ANALOG_CH_PARAM2,IDC_COMBO_SET_ALARM_FORM_PARAM,
							 IDC_COMBO_SET_FILTER_PARAM2,IDC_COMBO_SET_TRIGGER_PARAM2,
							 IDC_COMBO_SET_CALCMODE_PARAM2,IDC_COMBO_SET_MEASURETYPE_PARAM2,
							 IDC_COMBO_SET_SYNC_PARAM2,IDC_COMBO_SET_STORAGE_TARGET_PARAM2,
							 IDC_COMBO_SET_ANALOG_CH_PARAM2};

IMPLEMENT_DYNAMIC(CDlgSetTab2, CDialog)

CDlgSetTab2::CDlgSetTab2(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetTab2::IDD, pParent)
{

}

CDlgSetTab2::~CDlgSetTab2()
{
}

void CDlgSetTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSetTab2, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_FILTER, &CDlgSetTab2::OnBnClickedButtonSetFilter)
	ON_BN_CLICKED(IDC_BUTTON_SET_TRIGGER, &CDlgSetTab2::OnBnClickedButtonSetTrigger)
	ON_BN_CLICKED(IDC_BUTTON_SET_OFFSET, &CDlgSetTab2::OnBnClickedButtonSetOffset)
	ON_BN_CLICKED(IDC_BUTTON_SET_CALCMODE, &CDlgSetTab2::OnBnClickedButtonSetCalcmode)
	ON_BN_CLICKED(IDC_BUTTON_SET_ANALOG_SCALING, &CDlgSetTab2::OnBnClickedButtonSetAnalogScaling)
	ON_BN_CLICKED(IDC_BUTTON_SET_UNIT, &CDlgSetTab2::OnBnClickedButtonSetUnit)
	ON_BN_CLICKED(IDC_BUTTON_SET_SYNC, &CDlgSetTab2::OnBnClickedButtonSetSync)
	ON_BN_CLICKED(IDC_BUTTON_SET_MEASURETYPE, &CDlgSetTab2::OnBnClickedButtonSetMeasuretype)
	ON_BN_CLICKED(IDC_BUTTON_SET_STORAGE_TARGET, &CDlgSetTab2::OnBnClickedButtonSetStorageTarget)
	ON_BN_CLICKED(IDC_BUTTON_SET_SAMPLING, &CDlgSetTab2::OnBnClickedButtonSetSampling)
	ON_BN_CLICKED(IDC_BUTTON_SET_MUTUAL_PREV, &CDlgSetTab2::OnBnClickedButtonSetMutualPrev)
	ON_BN_CLICKED(IDC_BUTTON_SET_TOLERANCE_FORMAT, &CDlgSetTab2::OnBnClickedButtonSetToleranceFormat)
	ON_BN_CLICKED(IDC_BUTTON_SET_STROBE, &CDlgSetTab2::OnBnClickedButtonSetStrobe)
	ON_BN_CLICKED(IDC_BUTTON_SET_STORAGE, &CDlgSetTab2::OnBnClickedButtonSetStorage)
	ON_BN_CLICKED(IDC_BUTTON_SET_ANALOG_CH, &CDlgSetTab2::OnBnClickedButtonSetAnalogCh)
	ON_BN_CLICKED(IDC_BUTTON_SET_ALARM_FORM, &CDlgSetTab2::OnBnClickedButtonSetAlarmForm)
	ON_BN_CLICKED(IDC_BUTTON_SET_HEAD, &CDlgSetTab2::OnBnClickedButtonSetHead)
	ON_BN_CLICKED(IDC_BUTTON_SET_OUT, &CDlgSetTab2::OnBnClickedButtonSetOut)
	ON_BN_CLICKED(IDC_BUTTON_SET_NUM_CH, &CDlgSetTab2::OnBnClickedButtonSetNumCh)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_FILTER_PARAM2, &CDlgSetTab2::OnCbnSelchangeComboSetFilterParam2)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_MEASURETYPE_PARAM2, &CDlgSetTab2::OnCbnSelchangeComboSetMeasuretypeParam2)
END_MESSAGE_MAP()

void CDlgSetTab2::OnBnClickedButtonSetFilter()
{
	int outNo = 0;
	int filterMode = 0;
	int filterPara = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_FILTER_PARAM1,outNo)		||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_FILTER_PARAM2,filterMode)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_FILTER_PARAM3,filterPara))
	{
		return;
	}
	RC rc = LKIF2_SetFilter(outNo,(LKIF_FILTERMODE)filterMode,(LKIF_FILTERPARA)filterPara);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetFilter"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetTrigger()
{
	int outNo = 0;
	int toriggerMode = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_TRIGGER_PARAM1,outNo)		||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_TRIGGER_PARAM2,toriggerMode))
	{
		return;
	}

	RC rc = LKIF2_SetTriggerMode(outNo,(LKIF_TRIGGERMODE)toriggerMode);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetTriggerMode"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetOffset()
{
	int outNo = 0;
	int offset = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_OFFSET_PARAM,outNo)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_OFFSET_PARAM,offset))
	{
		return;
	}
	RC rc = LKIF2_SetOffset(outNo,offset);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetOffset"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetCalcmode()
{
	int outNo = 0;
	int calcMode = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_CALCMODE_PARAM1,outNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_CALCMODE_PARAM2,calcMode))
	{
		return;
	}
	RC rc = LKIF2_SetCalcMode(outNo,(LKIF_CALCMODE)calcMode);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetCalcMode"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetAnalogScaling()
{
	int outNo = 0;
	int inputValue1 = 0;
	int outputVoltage1 = 0;
	int inputValue2 = 0;
	int outputVoltage2 = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ANALOG_SCALING_PARAM,outNo)		   ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_ANALOG_SCALING_PARAM1,inputValue1)   ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_ANALOG_SCALING_PARAM2,outputVoltage1)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_ANALOG_SCALING_PARAM3,inputValue2)   ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_ANALOG_SCALING_PARAM4,outputVoltage2))
	{
		return;
	}
	
	RC rc = LKIF2_SetAnalogScaling(outNo,inputValue1,outputVoltage1,inputValue2,outputVoltage2);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetAnalogScaling"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetUnit()
{
	int outNo = 0;
	int displayUnit = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_UNIT_PARAM1,outNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_UNIT_PARAM2,displayUnit))
	{
		return;
	}
	
	RC rc = LKIF2_SetDisplayUnit(outNo,(LKIF_DISPLAYUNIT)displayUnit);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetDisplayUnit"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetSync()
{
	int outNo = 0;
	BOOL onOff = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_SYNC_PARAM1,outNo)||
	   !GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_SYNC_PARAM2,onOff))
	{
		return;
	}
	RC rc = LKIF2_SetSynchronization(outNo,onOff);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetSynchronization"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetMeasuretype()
{
	int outNo = 0;
	int measureType = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MEASURETYPE_PARAM1,outNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MEASURETYPE_PARAM2,measureType))
	{
		return;
	}
	RC rc = LKIF2_SetMeasureType(outNo,(LKIF_MEASURETYPE)measureType);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetMeasureType"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetStorageTarget()
{
	int outNo = 0;
	BOOL onOff = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_STORAGE_TARGET_PARAM1,outNo)||
	   !GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_STORAGE_TARGET_PARAM2,onOff))
	{
		return;
	}
	RC rc = LKIF2_SetStorageTarget(outNo,onOff);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetStorageTarget"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetSampling()
{
	int samplingCycle = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_SAMPLING_PARAM,samplingCycle))
	{
		return;
	}
	RC rc = LKIF2_SetSamplingCycle((LKIF_SAMPLINGCYCLE)samplingCycle);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetSamplingCycle"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetMutualPrev()
{
	int onOff = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MUTUAL_PREV_PARAM,onOff))
	{
		return;
	}
	RC rc = LKIF2_SetMutualInterferencePrevention((LKIF_MUTUAL_INTERFERENCE_PREVENTION)onOff);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetMutualInterferencePrevention"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetToleranceFormat()
{
	int outputFormat = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_TOLERANCE_FORMAT_PARAM,outputFormat))
	{
		return;
	}
	RC rc = LKIF2_SetToleranceComparatorOutputFormat((LKIF_TOLERANCE_COMPARATOR_OUTPUT_FORMAT)outputFormat);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetToleranceComparatorOutputFormat"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetStrobe()
{
	int strobeTime = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_STROBE_PARAM,strobeTime))
	{
		return;
	}
	RC rc = LKIF2_SetStrobeTime((LKIF_STOROBETIME)strobeTime);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetStrobeTime"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetStorage()
{
	int numStorage = 0;
	int storageCycle = 0;
	if(!GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_STORAGE_PARAM,numStorage)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_STORAGE_PARAM,storageCycle))
	{
		return;
	}
	RC rc = LKIF2_SetDataStorage(numStorage,(LKIF_STORAGECYCLE)storageCycle);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetDataStorage"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetAnalogCh()
{	
	int chNo = 0;
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ANALOG_CH_PARAM1,chNo)||
	   !GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ANALOG_CH_PARAM2,outNo))
	{
		return;
	}
	RC rc = LKIF2_SetAnalogChannel(chNo,outNo);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetAnalogChannel"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetAlarmForm()
{
	int alarmOutputForm = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_ALARM_FORM_PARAM,alarmOutputForm))
	{
		return;
	}
	RC rc = LKIF2_SetAlarmOutputForm((LKIF_ALARM_OUTPUT_FORM)alarmOutputForm);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetAlarmOutputForm"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetHead()
{
	int numOfUsedHeads = 0;
	if(!GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_HEAD_PARAM,numOfUsedHeads))
	{
		return;
	}
	RC rc = LKIF2_SetNumOfUsedHeads(numOfUsedHeads);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetNumOfUsedHeads"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetOut()
{
	int numOfUsedOut = 0;
	if(!GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_OUT_PARAM,numOfUsedOut))
	{
		return;
	}
	RC rc = LKIF2_SetNumOfUsedOut(numOfUsedOut);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetNumOfUsedOut"),rc);
}

void CDlgSetTab2::OnBnClickedButtonSetNumCh()
{	
	int numOfUsedAnalogCh = 0;
	if(!GET_VALUE_FROM_EDIT(this,IDC_EDIT_SET_NUM_CH_PARAM,numOfUsedAnalogCh))
	{
		return;
	}
	RC rc = LKIF2_SetNumOfUsedAnalogCh(numOfUsedAnalogCh);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetNumOfUsedAnalogCh"),rc);
}

BOOL CDlgSetTab2::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i = 0;i < SET_TAB2_COMBO_NUM;i++)
	{
		((CComboBox*)GetDlgItem(SET_TAB2_COMBO_IDS[i]))->SetCurSel(0);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_SET_FILTER_PARAM3))->SetCurSel(4);
	((CComboBox*)GetDlgItem(IDC_COMBO_SET_UNIT_PARAM2))->SetCurSel(1);
	((CComboBox*)GetDlgItem(IDC_COMBO_SET_SAMPLING_PARAM))->SetCurSel(6);
	SetDlgItemText(IDC_EDIT_SET_OFFSET_PARAM,_T("0"));
	SetDlgItemText(IDC_EDIT_SET_ANALOG_SCALING_PARAM1,_T("1000"));
	SetDlgItemText(IDC_EDIT_SET_ANALOG_SCALING_PARAM2,_T("1000"));
	SetDlgItemText(IDC_EDIT_SET_ANALOG_SCALING_PARAM3,_T("-1000"));
	SetDlgItemText(IDC_EDIT_SET_ANALOG_SCALING_PARAM4,_T("-1000"));
	SetDlgItemText(IDC_EDIT_SET_STORAGE_PARAM,_T("100000"));
	SetDlgItemText(IDC_EDIT_SET_HEAD_PARAM,_T("2"));
	SetDlgItemText(IDC_EDIT_SET_OUT_PARAM,_T("2"));	
	SetDlgItemText(IDC_EDIT_SET_NUM_CH_PARAM,_T("2"));
	return TRUE;
}


void CDlgSetTab2::OnCbnSelchangeComboSetFilterParam2()
{
	int filterMode = 0;
	GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_FILTER_PARAM2,filterMode);
	RESET_COMBO_FILTER_PARAM(this,IDC_COMBO_SET_FILTER_PARAM3,(LKIF_FILTERMODE)filterMode);

}

void CDlgSetTab2::OnCbnSelchangeComboSetMeasuretypeParam2()
{
	int measureType = 0;
	GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_MEASURETYPE_PARAM2,measureType);
	RESET_COMBO_UNIT(this,IDC_COMBO_SET_UNIT_PARAM2,(LKIF_MEASURETYPE)measureType);
}

void CDlgSetTab2::OnOK()
{
}

void CDlgSetTab2::OnCancel()
{
}
