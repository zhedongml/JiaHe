#pragma once



class CDlgMeasureCtlTab : public CDialog
{
	DECLARE_DYNAMIC(CDlgMeasureCtlTab)

public:
	CDlgMeasureCtlTab(CWnd* pParent = NULL);
	virtual ~CDlgMeasureCtlTab();

	enum { IDD = IDD_DLG_MEASURE_CTL_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetResetMulti();
	afx_msg void OnBnClickedButtonGetCalcSingle();
	afx_msg void OnBnClickedButtonGetCalcMulti();
	afx_msg void OnBnClickedButtonGetCalcAll();
	afx_msg void OnBnClickedButtonSetTimingSingle();
	afx_msg void OnBnClickedButtonSetZeroSingle();
	afx_msg void OnBnClickedButtonResetSingle();
	afx_msg void OnBnClickedButtonSetTimingMulti();
	afx_msg void OnBnClickedButtonSetZeroMulti();
	afx_msg void OnBnClickedButtonSetTimingSync();
	afx_msg void OnBnClickedButtonSetZeroSync();
	afx_msg void OnBnClickedButtonSetResetSync();
	afx_msg void OnBnClickedButtonSetPanelLock();
	afx_msg void OnBnClickedButtonSetProgramNo();
	afx_msg void OnBnClickedButtonGetProgramNo();
	afx_msg void OnBnClickedButtonStorageStart();
	afx_msg void OnBnClickedButtonStorageStop();
	afx_msg void OnBnClickedButtonStorageInit();
	afx_msg void OnBnClickedButtonStorageGetData();
	afx_msg void OnBnClickedButtonStorageGetStatus();
	afx_msg void OnBnClickedButtonGetLight();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
