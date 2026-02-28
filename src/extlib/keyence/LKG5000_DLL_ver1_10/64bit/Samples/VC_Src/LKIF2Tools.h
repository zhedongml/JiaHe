#pragma once
#include "stdafx.h"
#include "LKIF2.h"

class CLKIF2Tools
{
private:
	CLKIF2Tools(void);
	~CLKIF2Tools(void);
	

public:
	static BOOL IsIntegerText(const TCHAR *pcszText);
	static void ShowErrorMsg();
	static BOOL IsRcSuccess(RC result);
	static CString GetStringFromFloatValue(LKIF_FLOATVALUE_OUT value);
	static CString GetStringFromFloatValue(LKIF_FLOATVALUE value);
	static BOOL GetItemFromCombo(CWnd * wnd,int nID,int &item);
	static BOOL GetItemFromCheckBoxes(CWnd * wnd, const int * pIDs, int nNumID , int & item);
	static int CountCheckFromCheckBoxes(CWnd* wnd, const int * pIDs, int nNumID);
	static BOOL GetBoolFromCombo(CWnd * wnd,int nID,BOOL &nBool);
	static BOOL GetTextFromCombo(CWnd * wnd,int nID,int nSel,CString &strText);
	static BOOL GetValueFromEdit(CWnd* wnd, int nID , int & value);
	static LKIF_OUTNO LkifOutNo(int index);
	static BOOL ResetComboForFilterParam(CWnd * wnd,int nID,LKIF_FILTERMODE mode);
	static BOOL ResetComboForUnit(CWnd * wnd,int nID,LKIF_MEASURETYPE type);
	static CString GetFilterParamString(LKIF_FILTERMODE mode,LKIF_FILTERPARA param);
	static CString GetUnitString(LKIF_DISPLAYUNIT unit,LKIF_MEASURETYPE type);

};

static const int OUT_NUM_MAX = 12;
static const int STORAGE_MAX = 65535;
static const int WAVEDATA_MAX  = 2048;
static const int PEEK_MAX  = 4;

#define	IS_INTEGER_TEXT(pszText)	CLKIF2Tools::IsIntegerText(pszText)
#define SHOW_ERROR_MSG CLKIF2Tools::ShowErrorMsg()
#define IS_RC_SUCCESS(rc) CLKIF2Tools::IsRcSuccess(rc)
#define GET_STR_FLOAT_VALUE(value) CLKIF2Tools::GetStringFromFloatValue(value)
#define GET_ITEM_FROM_COMBO(wnd,id,item) CLKIF2Tools::GetItemFromCombo(wnd,id,item)
#define GET_BOOL_FROM_COMBO(wnd,id,item) CLKIF2Tools::GetBoolFromCombo(wnd,id,item)
#define GET_ITEM_FROM_CHECKS(wnd,ids,numId,item) CLKIF2Tools::GetItemFromCheckBoxes(wnd,ids,numId,item)
#define COUNT_CHECK_FROM_CHECK(wnd,ids,numId) CLKIF2Tools::CountCheckFromCheckBoxes(wnd,ids,numId)
#define GET_TEXT_FROM_COMBO(wnd,id,sel,txt) CLKIF2Tools::GetTextFromCombo(wnd,id,sel,txt)
#define GET_VALUE_FROM_EDIT(wnd,id,value) CLKIF2Tools::GetValueFromEdit(wnd,id,value)
#define GET_LKIF_OUT_NUM(index) CLKIF2Tools::LkifOutNo(index)
#define RESET_COMBO_FILTER_PARAM(wnd,id,mode) CLKIF2Tools::ResetComboForFilterParam(wnd,id,mode)
#define RESET_COMBO_UNIT(wnd,id,type) CLKIF2Tools::ResetComboForUnit(wnd,id,type)
#define GET_FILTER_PARAM_STR(mode,param) CLKIF2Tools::GetFilterParamString(mode,param)
#define GET_UNIT_STR(unit,type) CLKIF2Tools::GetUnitString(unit,type) 