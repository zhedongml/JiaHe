#pragma once
#include "afxcmn.h"
#include "dlggettab1.h"
#include "dlggettab2.h"
#include "dlgmodetab.h"
#include "dlgothertab.h"
#include "dlgsettab1.h"
#include "dlgsettab2.h"
#include "DlgMeasureCtlTab.h"
#include "afxwin.h"
#include "LKIF2.h"


class CLkIF2TestVCDlg : public CDialog
{
public:
	CLkIF2TestVCDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_LKIF2TESTVC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV


// ŽÀ‘•
protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
private:
	CTabCtrl m_TabCtrl;
	
	CDlgGetTab1 m_tabGet1;
	CDlgGetTab2 m_tabGet2;
	CDlgMeasureCtlTab m_tabMeasureCtl;
	CDlgModeTab m_tabMode;
	CDlgOtherTab m_tabOther;
	CDlgSetTab1 m_tabSet1;
	CDlgSetTab2 m_tabSet2;
public:
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	void DisplayCommandResult(CString strCommand,RC rc);
	CString GetTextFromCombo(int nId,int sel);
private:
	CWnd* GetWindowHasControl(int nId);
};
