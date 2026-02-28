#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "DlgMeasureCtlTab.h"
#include "LkIF2TestVCDlg.h"
#include "LKIF2Tools.h"

const int MEASURE_TAB_COMBO_NUM = 9;
const int MEASURE_TAB_COMBO_IDS[MEASURE_TAB_COMBO_NUM] =
							{IDC_COMBO_SET_SINGLE_PARAM1,IDC_COMBO_SET_SINGLE_PARAM2,
							 IDC_COMBO_SET_MULTI_PARAM,IDC_COMBO_SET_SYNC_PARAM,
							 IDC_COMBO_PANEL_LOCK_PARAM,IDC_COMBO_SET_PROGRAM_PARAM,
							 IDC_COMBO_STORAGE_GET_PARAM,IDC_COMBO_GET_LIGHT_PARAM,
							 IDC_COMBO_GET_CALC_SINGLE_PARAM};

const int CALC_MULTI_CHECK_IDS[OUT_NUM_MAX] =
								{IDC_CHECK_GET_CALC_MULTI_PARAM1,IDC_CHECK_GET_CALC_MULTI_PARAM2,
								 IDC_CHECK_GET_CALC_MULTI_PARAM3,IDC_CHECK_GET_CALC_MULTI_PARAM4,
								 IDC_CHECK_GET_CALC_MULTI_PARAM5,IDC_CHECK_GET_CALC_MULTI_PARAM6,
								 IDC_CHECK_GET_CALC_MULTI_PARAM7,IDC_CHECK_GET_CALC_MULTI_PARAM8,
								 IDC_CHECK_GET_CALC_MULTI_PARAM9,IDC_CHECK_GET_CALC_MULTI_PARAM10,
								 IDC_CHECK_GET_CALC_MULTI_PARAM11,IDC_CHECK_GET_CALC_MULTI_PARAM12};
const int CALC_MULTI_STATIC_IDS[OUT_NUM_MAX] =
								{IDC_STATIC_GET_CALC_MULTI_PARAM1,IDC_STATIC_GET_CALC_MULTI_PARAM2,
								 IDC_STATIC_GET_CALC_MULTI_PARAM3,IDC_STATIC_GET_CALC_MULTI_PARAM4,
								 IDC_STATIC_GET_CALC_MULTI_PARAM5,IDC_STATIC_GET_CALC_MULTI_PARAM6,
								 IDC_STATIC_GET_CALC_MULTI_PARAM7,IDC_STATIC_GET_CALC_MULTI_PARAM8,
								 IDC_STATIC_GET_CALC_MULTI_PARAM9,IDC_STATIC_GET_CALC_MULTI_PARAM10,
								 IDC_STATIC_GET_CALC_MULTI_PARAM11,IDC_STATIC_GET_CALC_MULTI_PARAM12}; 
const int CALC_ALL_STATIC_IDS[OUT_NUM_MAX] =
								{IDC_STATIC_GET_CALC_ALL_PARAM1,IDC_STATIC_GET_CALC_ALL_PARAM2,
								 IDC_STATIC_GET_CALC_ALL_PARAM3,IDC_STATIC_GET_CALC_ALL_PARAM4,
								 IDC_STATIC_GET_CALC_ALL_PARAM5,IDC_STATIC_GET_CALC_ALL_PARAM6,
								 IDC_STATIC_GET_CALC_ALL_PARAM7,IDC_STATIC_GET_CALC_ALL_PARAM8,
								 IDC_STATIC_GET_CALC_ALL_PARAM9,IDC_STATIC_GET_CALC_ALL_PARAM10,
								 IDC_STATIC_GET_CALC_ALL_PARAM11,IDC_STATIC_GET_CALC_ALL_PARAM12}; 
const int SET_MULTI_CHECK_IDS[OUT_NUM_MAX] =
								{IDC_CHECK_SET_MULTI_PARAM1,IDC_CHECK_SET_MULTI_PARAM2,
								 IDC_CHECK_SET_MULTI_PARAM3,IDC_CHECK_SET_MULTI_PARAM4,
								 IDC_CHECK_SET_MULTI_PARAM5,IDC_CHECK_SET_MULTI_PARAM6,
								 IDC_CHECK_SET_MULTI_PARAM7,IDC_CHECK_SET_MULTI_PARAM8,
								 IDC_CHECK_SET_MULTI_PARAM9,IDC_CHECK_SET_MULTI_PARAM10,
								 IDC_CHECK_SET_MULTI_PARAM11,IDC_CHECK_SET_MULTI_PARAM12};								


IMPLEMENT_DYNAMIC(CDlgMeasureCtlTab, CDialog)

CDlgMeasureCtlTab::CDlgMeasureCtlTab(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMeasureCtlTab::IDD, pParent)
{

}

CDlgMeasureCtlTab::~CDlgMeasureCtlTab()
{
}

void CDlgMeasureCtlTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMeasureCtlTab, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SET_RESET_MULTI, &CDlgMeasureCtlTab::OnBnClickedButtonSetResetMulti)
	ON_BN_CLICKED(IDC_BUTTON_GET_CALC_SINGLE, &CDlgMeasureCtlTab::OnBnClickedButtonGetCalcSingle)
	ON_BN_CLICKED(IDC_BUTTON_GET_CALC_MULTI, &CDlgMeasureCtlTab::OnBnClickedButtonGetCalcMulti)
	ON_BN_CLICKED(IDC_BUTTON_GET_CALC_ALL, &CDlgMeasureCtlTab::OnBnClickedButtonGetCalcAll)
	ON_BN_CLICKED(IDC_BUTTON_SET_TIMING_SINGLE, &CDlgMeasureCtlTab::OnBnClickedButtonSetTimingSingle)
	ON_BN_CLICKED(IDC_BUTTON_SET_ZERO_SINGLE, &CDlgMeasureCtlTab::OnBnClickedButtonSetZeroSingle)
	ON_BN_CLICKED(IDC_BUTTON_RESET_SINGLE, &CDlgMeasureCtlTab::OnBnClickedButtonResetSingle)
	ON_BN_CLICKED(IDC_BUTTON_SET_TIMING_MULTI, &CDlgMeasureCtlTab::OnBnClickedButtonSetTimingMulti)
	ON_BN_CLICKED(IDC_BUTTON_SET_ZERO_MULTI, &CDlgMeasureCtlTab::OnBnClickedButtonSetZeroMulti)
	ON_BN_CLICKED(IDC_BUTTON_SET_TIMING_SYNC, &CDlgMeasureCtlTab::OnBnClickedButtonSetTimingSync)
	ON_BN_CLICKED(IDC_BUTTON_SET_ZERO_SYNC, &CDlgMeasureCtlTab::OnBnClickedButtonSetZeroSync)
	ON_BN_CLICKED(IDC_BUTTON_SET_RESET_SYNC, &CDlgMeasureCtlTab::OnBnClickedButtonSetResetSync)
	ON_BN_CLICKED(IDC_BUTTON_SET_PANEL_LOCK, &CDlgMeasureCtlTab::OnBnClickedButtonSetPanelLock)
	ON_BN_CLICKED(IDC_BUTTON_SET_PROGRAM_NO, &CDlgMeasureCtlTab::OnBnClickedButtonSetProgramNo)
	ON_BN_CLICKED(IDC_BUTTON_GET_PROGRAM_NO, &CDlgMeasureCtlTab::OnBnClickedButtonGetProgramNo)
	ON_BN_CLICKED(IDC_BUTTON_STORAGE_START, &CDlgMeasureCtlTab::OnBnClickedButtonStorageStart)
	ON_BN_CLICKED(IDC_BUTTON_STORAGE_STOP, &CDlgMeasureCtlTab::OnBnClickedButtonStorageStop)
	ON_BN_CLICKED(IDC_BUTTON_STORAGE_INIT, &CDlgMeasureCtlTab::OnBnClickedButtonStorageInit)
	ON_BN_CLICKED(IDC_BUTTON_STORAGE_GET_DATA, &CDlgMeasureCtlTab::OnBnClickedButtonStorageGetData)
	ON_BN_CLICKED(IDC_BUTTON_STORAGE_GET_STATUS, &CDlgMeasureCtlTab::OnBnClickedButtonStorageGetStatus)
	ON_BN_CLICKED(IDC_BUTTON_GET_LIGHT, &CDlgMeasureCtlTab::OnBnClickedButtonGetLight)
END_MESSAGE_MAP()


void CDlgMeasureCtlTab::OnBnClickedButtonGetCalcSingle()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_CALC_SINGLE_PARAM,outNo))
	{
		return;
	}

	LKIF_FLOATVALUE_OUT value;
	RC rc = LKIF2_GetCalcDataSingle((outNo),&value);

	ASSERT((outNo)==value.OutNo);
	if(IS_RC_SUCCESS(rc))
	{
		SetDlgItemText(IDC_STATIC_GET_SINGLE_PARAM,GET_STR_FLOAT_VALUE(value));
	}

	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetCalcDataSingle"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonGetCalcMulti()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_CHECKS(this,CALC_MULTI_CHECK_IDS,OUT_NUM_MAX,outNo))
	{
		return;
	}

	LKIF_FLOATVALUE_OUT values[OUT_NUM_MAX];
	RC rc = LKIF2_GetCalcDataMulti((LKIF_OUTNO)outNo,values);
	
	if(IS_RC_SUCCESS(rc))
	{
		for(int cnt = 0; cnt < COUNT_CHECK_FROM_CHECK(this,CALC_MULTI_CHECK_IDS,OUT_NUM_MAX);cnt++)
		{
			SetDlgItemText(CALC_MULTI_STATIC_IDS[(values[cnt].OutNo)],GET_STR_FLOAT_VALUE(values[cnt]));
		}
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetCalcDataMulti"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonGetCalcAll()
{
	LKIF_FLOATVALUE_OUT values[OUT_NUM_MAX];
	int outNo = 0;
	RC rc = LKIF2_GetCalcDataALL(&outNo,values);

	if(IS_RC_SUCCESS(rc))
	{
		for(int cnt = 0; cnt < OUT_NUM_MAX;cnt++)
		{
			if(cnt < outNo)
			{
				SetDlgItemText(CALC_ALL_STATIC_IDS[(values[cnt].OutNo)],GET_STR_FLOAT_VALUE(values[cnt]));
			}
			else
			{
				SetDlgItemText(CALC_ALL_STATIC_IDS[cnt],_T("------"));
			}
		}
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetCalcDataALL"),rc);

}

void CDlgMeasureCtlTab::OnBnClickedButtonSetTimingSingle()
{
	int outNo = 0;
	int onOff = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_SINGLE_PARAM1,outNo) ||
	   !GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_SINGLE_PARAM2,onOff))
	{
		return;
	}
	
	RC rc = LKIF2_SetTimingSingle(outNo,onOff);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetTimingSingle"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonSetZeroSingle()
{
	int outNo = 0;
	int onOff = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_SINGLE_PARAM1,outNo) ||
	   !GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_SINGLE_PARAM2,onOff))
	{
		return;
	}
	RC rc = LKIF2_SetZeroSingle(outNo,onOff);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetZeroSingle"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonResetSingle()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_SINGLE_PARAM1,outNo))
	{
		return;
	}
	RC rc = LKIF2_SetResetSingle(outNo);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetResetSingle"),rc);	
}

void CDlgMeasureCtlTab::OnBnClickedButtonSetTimingMulti()
{
	int outNo = 0;
	int onOff = 0;
	if(!GET_ITEM_FROM_CHECKS(this,SET_MULTI_CHECK_IDS,OUT_NUM_MAX,outNo) ||
	   !GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_MULTI_PARAM,onOff))
	{
		return;
	}
	
	RC rc = LKIF2_SetTimingMulti((LKIF_OUTNO)outNo,onOff);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetTimingMulti"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonSetZeroMulti()
{
	int outNo = 0;
	int onOff =0;
	if(!GET_ITEM_FROM_CHECKS(this,SET_MULTI_CHECK_IDS,OUT_NUM_MAX,outNo) ||
       !GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_MULTI_PARAM,onOff))
	{
		return;
	}

	RC rc = LKIF2_SetZeroMulti((LKIF_OUTNO)outNo,onOff);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetZeroMulti"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonSetResetMulti()
{
	int outNo = 0;
	if(!GET_ITEM_FROM_CHECKS(this,SET_MULTI_CHECK_IDS,OUT_NUM_MAX,outNo))
	{
		return;
	}
	RC rc = LKIF2_SetResetMulti((LKIF_OUTNO)outNo);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetResetMulti"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonSetTimingSync()
{
	int onOff = 0;
	if(!GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_SYNC_PARAM,onOff))
	{
		return;
	}
	RC rc = LKIF2_SetTimingSync(onOff);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetTimingSync"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonSetZeroSync()
{
	int onOff = 0;
	if(!GET_BOOL_FROM_COMBO(this,IDC_COMBO_SET_SYNC_PARAM,onOff))
	{
		return;
	}
	RC rc = LKIF2_SetZeroSync(onOff);	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetZeroSync"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonSetResetSync()
{
	RC rc = LKIF2_SetResetSync();	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetResetSync"),rc);
}


void CDlgMeasureCtlTab::OnBnClickedButtonSetPanelLock()
{
	int onOff = 0;
	if(!GET_BOOL_FROM_COMBO(this,IDC_COMBO_PANEL_LOCK_PARAM,onOff))
	{
		return;
	}
	RC rc = LKIF2_SetPanelLock(onOff);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetPanelLock"),rc);
}


void CDlgMeasureCtlTab::OnBnClickedButtonSetProgramNo()
{
	int program = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_SET_PROGRAM_PARAM,program))
	{
		return;
	}
	RC rc = LKIF2_SetProgramNo(program);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_SetProgramNo"),rc);
}


void CDlgMeasureCtlTab::OnBnClickedButtonGetProgramNo()
{
	int program = 0;
	RC rc = LKIF2_GetProgramNo(&program);
	if(IS_RC_SUCCESS(rc))
	{
		CString str(_T(""));
		GET_TEXT_FROM_COMBO(this,IDC_COMBO_SET_PROGRAM_PARAM,program,str);
		SetDlgItemText(IDC_STATIC_GET_PROGRAM_PARAM,str);
	}	
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetProgramNo"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonStorageStart()
{
	RC rc = LKIF2_DataStorageStart();
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_DataStorageStart"),rc);
}

void CDlgMeasureCtlTab::OnBnClickedButtonStorageStop()
{
	RC rc = LKIF2_DataStorageStop();
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_DataStorageStop"),rc);	
}

void CDlgMeasureCtlTab::OnBnClickedButtonStorageInit()
{
	RC rc = LKIF2_DataStorageInit();
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_DataStorageInit"),rc);	
}

void CDlgMeasureCtlTab::OnBnClickedButtonStorageGetData()
{
	int outNo = 0;
	int nOutBuffer = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_STORAGE_GET_PARAM,outNo)||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_STORAGE_GET_PARAM,nOutBuffer))
	{
		return;
	}


	int nReceived = 0;
	LKIF_FLOATVALUE *pValues = new LKIF_FLOATVALUE[nOutBuffer];
	::ZeroMemory(pValues,sizeof(LKIF_FLOATVALUE)*nOutBuffer);
	RC rc = LKIF2_DataStorageGetData(outNo,nOutBuffer,pValues,&nReceived);
	
	while(IS_RC_SUCCESS(rc))
	{
		CString strReceived(_T(""));
		strReceived.Format(_T("%d"),nReceived);
		SetDlgItemText(IDC_STATIC_STORAGE_GET_PARAM,strReceived);
		
		if(!((CButton*)GetDlgItem(IDC_CHECK_STORAGE_GET_PARAM))->GetCheck())
		{
			break;
		}	

		CFileDialog dlg(FALSE,_T("csv"), _T("DataStorageLog.csv"),
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("CSV (*.csv)|*.csv||"));
		
		if (dlg.DoModal() != IDOK)
		{
			break;
		}
		try
		{
			CStdioFile file(dlg.GetPathName(),CFile::modeCreate|CFile::shareDenyWrite|CFile::modeWrite);
			CString strLine(_T(""));
			file.WriteString(_T("OutNo\n"));
			strLine.Format(_T("%d\n"),outNo);
			file.WriteString(strLine);
			file.WriteString(_T("Data,FloatResult\n"));
			for(int line = 0;line < nReceived;line++)
			{

				if((pValues[line].Value)==0.0)
				{
					strLine.Format(_T("%s,%s\n"),_T("0"),GET_STR_FLOAT_VALUE(pValues[line]));
				}
				else
				{
					strLine.Format(_T("%f,%s\n"),pValues[line].Value,GET_STR_FLOAT_VALUE(pValues[line]));
				}
				file.WriteString(strLine);
			}
		}
		catch(...)
		{
			AfxMessageBox(_T("Failed."));
		}
		break;
	}
	
	free(pValues);

	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_DataStorageGetData"),rc);	
}

void CDlgMeasureCtlTab::OnBnClickedButtonStorageGetStatus()
{
	BOOL bIsStorage = FALSE;
	int nStorageNum = 0;
	RC rc = LKIF2_DataStorageGetStatus(&bIsStorage,&nStorageNum);
	if(IS_RC_SUCCESS(rc))
	{
		CString strStorageNum(_T(""));
		SetDlgItemText(IDC_STATIC_STORAGE_STATUS_PARAM1,(bIsStorage)?_T("Storage in progress"):_T("Stopping in progress"));
		strStorageNum.Format(_T("%d"),nStorageNum);
		SetDlgItemText(IDC_STATIC_STORAGE_STATUS_PARAM2,strStorageNum);
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_DataStorageGetStatus"),rc);	
}

void CDlgMeasureCtlTab::OnBnClickedButtonGetLight()
{
	int headNo = 0;
	int peekNo = 0;
	if(!GET_ITEM_FROM_COMBO(this,IDC_COMBO_GET_LIGHT_PARAM,headNo) ||
	   !GET_VALUE_FROM_EDIT(this,IDC_EDIT_GET_LIGHT_PARAM,peekNo))
	{
		return;
	}

	int measurePosition[PEEK_MAX] = {0,0,0,0}; 
	BYTE waveData[WAVEDATA_MAX];
	ZeroMemory(waveData,sizeof(waveData));
	RC rc = LKIF2_GetLight(headNo,peekNo,measurePosition,waveData);
	while(IS_RC_SUCCESS(rc))
	{
		if(!((CButton*)GetDlgItem(IDC_CHECK_GET_LIGHT_PARAM))->GetCheck())
		{
			break;
		}	
		CFileDialog dlg(FALSE,_T("csv"), _T("LightLog.csv"),
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("CSV (*.csv)|*.csv||"));		
		if (dlg.DoModal() != IDOK)
		{
			break;
		}
		try
		{
			CStdioFile file(dlg.GetPathName(),CFile::modeCreate|CFile::shareDenyWrite|CFile::modeWrite);
			CString strLine(_T(""));
			strLine.Format(_T("HeadNo,%d\n%d,%d\n"),waveData[0],headNo,waveData[1]);
			file.WriteString(strLine);
			strLine.Format(_T("PeekNo,%d\n%d,%d\n"),waveData[2],peekNo,waveData[3]);
			file.WriteString(strLine);
			strLine.Format(_T("MeasurePosition,%d\n%d,%d\n%d,%d\n%d,%d\n%d,%d\n"),
											   waveData[4],
							measurePosition[0],waveData[5],
							measurePosition[1],waveData[6],
							measurePosition[2],waveData[7],
							measurePosition[3],waveData[8]
							);
			file.WriteString(strLine);
			
			for(int line = 9;line < WAVEDATA_MAX;line++)
			{
				strLine.Format(_T(",%d\n"),waveData[line]);
				file.WriteString(strLine);
			}
		}
		catch(...)
		{
			AfxMessageBox(_T("Failed."));
		}
		break;
	}
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_GetLight"),rc);

}

BOOL CDlgMeasureCtlTab::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i = 0;i < MEASURE_TAB_COMBO_NUM;i++)
	{
		((CComboBox*)GetDlgItem(MEASURE_TAB_COMBO_IDS[i]))->SetCurSel(0);
	}
	SetDlgItemText(IDC_EDIT_STORAGE_GET_PARAM,_T("0"));
	SetDlgItemText(IDC_EDIT_GET_LIGHT_PARAM,_T("0"));
	CheckDlgButton(IDC_CHECK_STORAGE_GET_PARAM,BST_CHECKED);
	CheckDlgButton(IDC_CHECK_GET_LIGHT_PARAM,BST_CHECKED);
	return TRUE;
}

void CDlgMeasureCtlTab::OnOK()
{
}

void CDlgMeasureCtlTab::OnCancel()
{
}
