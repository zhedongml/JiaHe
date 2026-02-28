#pragma once



class CDlgGetTab2 : public CDialog
{
	DECLARE_DYNAMIC(CDlgGetTab2)

public:
	CDlgGetTab2(CWnd* pParent = NULL);
	virtual ~CDlgGetTab2();

	enum { IDD = IDD_DLG_GET_TAB2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGetNumCh();
	afx_msg void OnBnClickedButtonGetFilter();
	afx_msg void OnBnClickedButtonGetTrigger();
	afx_msg void OnBnClickedButtonGetOffset();
	afx_msg void OnBnClickedButtonGetCalcmode();
	afx_msg void OnBnClickedButtonGetAnalogScaling();
	afx_msg void OnBnClickedButtonGetUnit();
	afx_msg void OnBnClickedButtonGetMeasuretype();
	afx_msg void OnBnClickedButtonGetStorageTarget();
	afx_msg void OnBnClickedButtonGetSampling();
	afx_msg void OnBnClickedButtonGetMutualPrev();
	afx_msg void OnBnClickedButtonGetToleranceFormat();
	afx_msg void OnBnClickedButtonGetStrobe();
	afx_msg void OnBnClickedButtonGetStorage();
	afx_msg void OnBnClickedButtonGetAnalogCh();
	afx_msg void OnBnClickedButtonGetAlarmForm();
	afx_msg void OnBnClickedButtonGetHead();
	afx_msg void OnBnClickedButtonGetOut();
	afx_msg void OnBnClickedButtonGetSync();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
