#include "StdAfx.h"
#include "LKIF2Tools.h"

static const int LKIF_OUT_NUMS[OUT_NUM_MAX]={LKIF_OUTNO_01,LKIF_OUTNO_02,LKIF_OUTNO_03,LKIF_OUTNO_04,
											 LKIF_OUTNO_05,LKIF_OUTNO_06,LKIF_OUTNO_07,LKIF_OUTNO_08,
											 LKIF_OUTNO_09,LKIF_OUTNO_10,LKIF_OUTNO_11,LKIF_OUTNO_12}; 
static const int FILTER_PARAM_MAX = 10;
static const TCHAR* FILTER_AVE_STR[] ={ _T("LKIF_FILTERPARA_AVE_1"), _T("LKIF_FILTERPARA_AVE_4"), 
										_T("LKIF_FILTERPARA_AVE_16"),_T("LKIF_FILTERPARA_AVE_64"), 
										_T("LKIF_FILTERPARA_AVE_256"), _T("LKIF_FILTERPARA_AVE_1024"),
										_T("LKIF_FILTERPARA_AVE_4096"),_T("LKIF_FILTERPARA_AVE_16384"),
										_T("LKIF_FILTERPARA_AVE_65536"),_T("LKIF_FILTERPARA_AVE_262144")};
static const TCHAR* FILTER_COFF_STR[] ={_T("LKIF_FILTERPARA_COFF_3000"), _T("LKIF_FILTERPARA_COFF_1000"),
                                        _T("LKIF_FILTERPARA_COFF_300"), _T("LKIF_FILTERPARA_COFF_100"), 
                                        _T("LKIF_FILTERPARA_COFF_30"), _T("LKIF_FILTERPARA_COFF_10"), 
                                        _T("LKIF_FILTERPARA_COFF_3"), _T("LKIF_FILTERPARA_COFF_1"), 
										_T("LKIF_FILTERPARA_COFF_0_3"), _T("LKIF_FILTERPARA_COFF_0_1")};
static const int DISPLAYUNIT_MAX = 7;
static const TCHAR* UNIT_DISP_STR[] = {_T("LKIF_DISPLAYUNIT_0000_01MM"), _T("LKIF_DISPLAYUNIT_000_001MM"), 
                                       _T("LKIF_DISPLAYUNIT_00_0001MM"), _T("LKIF_DISPLAYUNIT_0_00001MM"),
                                       _T("LKIF_DISPLAYUNIT_00000_1UM"), _T("LKIF_DISPLAYUNIT_0000_01UM"), 
									   _T("LKIF_DISPLAYUNIT_000_001UM")};
static const TCHAR* UNIT_SPEED_STR[] = {_T("LKIF_DISPLAYUNIT_00000_1M_S"), _T("LKIF_DISPLAYUNIT_0000_01M_S"), 
                                         _T("LKIF_DISPLAYUNIT_000_001M_S"), _T("LKIF_DISPLAYUNIT_00000_1MM_S"),
                                         _T("LKIF_DISPLAYUNIT_0000_01MM_S"),_T("LKIF_DISPLAYUNIT_000_001MM_S"),
									     _T("LKIF_DISPLAYUNIT_00_0001MM_S")};
static const TCHAR* UNIT_ACCEL_STR[] = {_T("LKIF_DISPLAYUNIT_00000_1KM_S2"), _T("LKIF_DISPLAYUNIT_0000_01KM_S2"), 
                                         _T("LKIF_DISPLAYUNIT_000_001KM_S2"), _T("LKIF_DISPLAYUNIT_00000_1M_S2"),
                                         _T("LKIF_DISPLAYUNIT_0000_01M_S2"), _T("LKIF_DISPLAYUNIT_000_001M_S2"), 
										 _T("LKIF_DISPLAYUNIT_00_0001M_S2")};
CLKIF2Tools::CLKIF2Tools(void)
{
}

CLKIF2Tools::~CLKIF2Tools(void)
{
}

BOOL CLKIF2Tools::IsIntegerText(const TCHAR *pcszText)
{
	BOOL			bReturnCode;
	int				iCharCount;
	BOOL			bIsFoundNumText;

	bReturnCode = TRUE;
	iCharCount = 0;
	bIsFoundNumText = FALSE;
	while(*pcszText == _T(' ')) {
		pcszText ++;
	}
	if (*pcszText == '-') {
		pcszText ++;
	}
	if (_tcslen(pcszText) <= 10) {
		while(*(pcszText+iCharCount) != 0x00) {
			switch(*(pcszText+iCharCount)) {
			case	'0' :
			case	'1' :
			case	'2' :
			case	'3' :
			case	'4' :
			case	'5' :
			case	'6' :
			case	'7' :
			case	'8' :
			case	'9' :
				bIsFoundNumText = TRUE;
				break;
			case	'-' :
			case	'+' :
				if (iCharCount == 0) {
				} else {
					bReturnCode = FALSE;
				}
				break;
			default :
				bReturnCode = FALSE;
				break;
			}
			iCharCount ++;
			if (bReturnCode == FALSE) {
				break;
			}
		}
		if (bIsFoundNumText == FALSE) {
			bReturnCode = FALSE;
		} else if ((_ttoi64(pcszText)&0xffffffff00000000) != 0) {
			bReturnCode = FALSE;
		}
	} else {
		bReturnCode = FALSE;
	}
	return(bReturnCode);
}

void CLKIF2Tools::ShowErrorMsg()
{
	AfxMessageBox(_T("illegal input"));
}

BOOL CLKIF2Tools::IsRcSuccess(RC result)
{
	return (result == RC_OK);
}

CString CLKIF2Tools::GetStringFromFloatValue(LKIF_FLOATVALUE_OUT value)
{
	CString result;
	switch(value.FloatResult)
	{
		case LKIF_FLOATRESULT_VALID:
				result.Format(_T("%f"),value.Value);	
			break;
		case LKIF_FLOATRESULT_RANGEOVER_P:
			result.Append(_T("+FFFFFFF"));
			break;
		case LKIF_FLOATRESULT_RANGEOVER_N:
			result.Append(_T("-FFFFFFF"));
			break;
		case LKIF_FLOATRESULT_WAITING:
			result.Append(_T("--------"));
			break;
		case LKIF_FLOATRESULT_ALARM:
			result.Append(_T("alarm"));
			break;
		default:
			result.Append(_T("Invalid"));
	}

	return result;
}

CString CLKIF2Tools::GetStringFromFloatValue(LKIF_FLOATVALUE value)
{
	CString result;
	switch(value.FloatResult)
	{
		case LKIF_FLOATRESULT_VALID:
			result.Append(_T("LKIF_FLOATRESULT_VALID"));
			break;
		case LKIF_FLOATRESULT_RANGEOVER_P:
			result.Append(_T("LKIF_FLOATRESULT_RANGEOVER_P"));
			break;
		case LKIF_FLOATRESULT_RANGEOVER_N:
			result.Append(_T("LKIF_FLOATRESULT_RANGEOVER_N"));
			break;
		case LKIF_FLOATRESULT_WAITING:
			result.Append(_T("LKIF_FLOATRESULT_WAITING"));
			break;
		case LKIF_FLOATRESULT_ALARM:
			result.Append(_T("LKIF_FLOATRESULT_ALARM"));
			break;
		default:
			result.Append(_T("LKIF_FLOATRESULT_INVALID"));
	}

	return result;
}

BOOL CLKIF2Tools::GetItemFromCombo(CWnd * wnd,int nID,int &item)
{
	if(wnd == NULL)
	{
		return FALSE;
	}

	int nSel = ((CComboBox*)wnd->GetDlgItem(nID))->GetCurSel();
	if(nSel == CB_ERR)
	{
		TCHAR temp[11];
		int cnt = wnd->GetDlgItemText(nID,temp,11);
		if( cnt < 1  || 10 <cnt)
		{
			SHOW_ERROR_MSG;
			return FALSE;
		}
		if(IS_INTEGER_TEXT(temp) == FALSE)
		{
			SHOW_ERROR_MSG;
			return FALSE;
		}
		nSel = _ttoi(temp);
	}
	item = nSel;
	return TRUE;
}

BOOL CLKIF2Tools::GetTextFromCombo(CWnd * wnd,int nID,int nSel,CString &item)
{

	if(nSel == CB_ERR || wnd == NULL)
	{
		return FALSE;
	}
	((CComboBox*)wnd->GetDlgItem(nID))->GetLBText(nSel,item);
	return TRUE;
}

BOOL CLKIF2Tools::GetBoolFromCombo(CWnd * wnd,int nID,BOOL &nBool)
{
	if(wnd == NULL)
	{
		return FALSE;
	}
	int nSel = ((CComboBox*)wnd->GetDlgItem(nID))->GetCurSel();
	if(nSel == CB_ERR)
	{
		CString temp(_T(""));
		int cnt = wnd->GetDlgItemText(nID,temp);
		
		if( cnt < 1  || 10 <cnt)
		{
			SHOW_ERROR_MSG;
			return FALSE;
		}
		if(IS_INTEGER_TEXT(temp) == FALSE)
		{
			SHOW_ERROR_MSG;
			return FALSE;
		}
		nSel = _ttoi(temp);
		nBool = nSel;

	}
	else
	{	
		ASSERT((nSel==0) || (nSel==1));
		nBool = (nSel==0)?1:0;
	}
	return TRUE;
}

BOOL CLKIF2Tools::GetItemFromCheckBoxes(CWnd * wnd, const int * pIDs, int nNumID , int & item)
{
	int result = 0;
	if( nNumID < 1 || OUT_NUM_MAX < nNumID ||(wnd == NULL))
	{
		return FALSE;
	}
	for(int cnt = 0;cnt < nNumID;pIDs++,cnt++)
	{
		if(((CButton*)wnd->GetDlgItem(*pIDs))->GetCheck())
		{
			result |= LKIF_OUT_NUMS[cnt];
		}
	}
	item = result;
	return TRUE;
}

int CLKIF2Tools::CountCheckFromCheckBoxes(CWnd* wnd, const int * pIDs, int nNumID)
{
	int result = 0;
	if(nNumID < 1 || OUT_NUM_MAX < nNumID|| (wnd == NULL))
	{
		return result;
	}
	for(int cnt = 0;cnt < nNumID;pIDs++,cnt++)
	{
		if(((CButton*)wnd->GetDlgItem(*pIDs))->GetCheck())
		{
			++result;
		}
	}
	return result;
}

LKIF_OUTNO CLKIF2Tools::LkifOutNo(int index)
{
	if(index < 0 || OUT_NUM_MAX <= index)
	{
		return (LKIF_OUTNO)0;
	}
	return (LKIF_OUTNO)LKIF_OUT_NUMS[index];
}

BOOL CLKIF2Tools::GetValueFromEdit(CWnd* wnd, int nID , int & value)
{
	if(wnd == NULL)
	{
		return FALSE;
	}

	CString strValue(_T(""));
	wnd->GetDlgItemText(nID,strValue);
	if(IS_INTEGER_TEXT(strValue) == FALSE)
	{
		SHOW_ERROR_MSG;
		return FALSE;
	}
	value = _ttoi(strValue);
	return TRUE;
}

BOOL CLKIF2Tools::ResetComboForFilterParam(CWnd * wnd,int nID,LKIF_FILTERMODE mode)
{
	if(wnd == NULL)
	{
		return FALSE;
	}
	CComboBox* pCombo = (CComboBox*)wnd->GetDlgItem(nID);
	if(pCombo == NULL)
	{
		return FALSE;
	}
	int sel = pCombo->GetCurSel();
	pCombo->ResetContent();
	if(mode ==LKIF_FILTERMODE_MOVING_AVERAGE)
	{
		for(int i = 0;i<FILTER_PARAM_MAX;i++)
		{			
			pCombo->InsertString(-1,FILTER_AVE_STR[i]);
		}
	}
	else
	{
		for(int i = 0;i<FILTER_PARAM_MAX;i++)
		{			
			pCombo->InsertString(-1,FILTER_COFF_STR[i]);
		}
	}
	pCombo->SetCurSel((sel == CB_ERR)?0:sel);
	return TRUE;
}
BOOL CLKIF2Tools::ResetComboForUnit(CWnd * wnd,int nID,LKIF_MEASURETYPE type)
{
	if(wnd == NULL)
	{
		return FALSE;
	}
	CComboBox* pCombo = (CComboBox*)wnd->GetDlgItem(nID);
	if(pCombo == NULL)
	{
		return FALSE;
	}
	int sel = pCombo->GetCurSel();
	pCombo->ResetContent();
	if(type == LKIF_MEASURETYPE_DISPLACEMENT)
	{
		for(int i = 0;i<DISPLAYUNIT_MAX;i++)
		{			
			pCombo->InsertString(-1,UNIT_DISP_STR[i]);
		}
	}
	else if(type == LKIF_MEASURETYPE_SPEED)
	{
		for(int i = 0;i<DISPLAYUNIT_MAX;i++)
		{			
			pCombo->InsertString(-1,UNIT_SPEED_STR[i]);
		}
	}
	else
	{
		for(int i = 0;i<DISPLAYUNIT_MAX;i++)
		{			
			pCombo->InsertString(-1,UNIT_ACCEL_STR[i]);
		}
	}

	pCombo->SetCurSel((sel == CB_ERR)?0:sel);
	return TRUE;

}

CString CLKIF2Tools::GetFilterParamString(LKIF_FILTERMODE mode,LKIF_FILTERPARA param)
{
	const TCHAR* pStr;
	if(mode ==LKIF_FILTERMODE_MOVING_AVERAGE)
	{
		pStr = FILTER_AVE_STR[param];
	}
	else
	{
		pStr = FILTER_COFF_STR[param];
	}
	
	CString result(pStr);
	return result;
}

CString CLKIF2Tools::GetUnitString(LKIF_DISPLAYUNIT unit,LKIF_MEASURETYPE type)
{
	const TCHAR* pStr;
	switch(type)
	{
	case LKIF_MEASURETYPE_SPEED:
		pStr = UNIT_SPEED_STR[unit];
		break;
	case LKIF_MEASURETYPE_ACCELERATION:
		pStr = UNIT_ACCEL_STR[unit];
		break;
	default:
		pStr = UNIT_DISP_STR[unit];
		break;
	}

	CString result(pStr);
	return result;
}