#pragma once



class CDlgSetTab2 : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetTab2)

public:
	CDlgSetTab2(CWnd* pParent = NULL);
	virtual ~CDlgSetTab2();

	enum { IDD = IDD_DLG_SET_TAB2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetFilter();
	afx_msg void OnBnClickedButtonSetTrigger();
	afx_msg void OnBnClickedButtonSetOffset();
	afx_msg void OnBnClickedButtonSetCalcmode();
	afx_msg void OnBnClickedButtonSetAnalogScaling();
	afx_msg void OnBnClickedButtonSetUnit();
	afx_msg void OnBnClickedButtonSetSync();
	afx_msg void OnBnClickedButtonSetMeasuretype();
	afx_msg void OnBnClickedButtonSetStorageTarget();
	afx_msg void OnBnClickedButtonSetSampling();
	afx_msg void OnBnClickedButtonSetMutualPrev();
	afx_msg void OnBnClickedButtonSetToleranceFormat();
	afx_msg void OnBnClickedButtonSetStrobe();
	afx_msg void OnBnClickedButtonSetStorage();
	afx_msg void OnBnClickedButtonSetAnalogCh();
	afx_msg void OnBnClickedButtonSetAlarmForm();
	afx_msg void OnBnClickedButtonSetHead();
	afx_msg void OnBnClickedButtonSetOut();
	afx_msg void OnBnClickedButtonSetNumCh();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboSetFilterParam2();
	afx_msg void OnCbnSelchangeComboSetMeasuretypeParam2();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
