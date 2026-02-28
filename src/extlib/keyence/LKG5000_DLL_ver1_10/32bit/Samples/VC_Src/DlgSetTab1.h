#pragma once



class CDlgSetTab1 : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetTab1)

public:
	CDlgSetTab1(CWnd* pParent = NULL);
	virtual ~CDlgSetTab1();

	enum { IDD = IDD_DLG_SET_TAB1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSetPanel();
	afx_msg void OnBnClickedButtonSetTolerance();
	afx_msg void OnBnClickedButtonSetAble();
	afx_msg void OnBnClickedButtonSetAbleMinmax();
	afx_msg void OnBnClickedButtonSetMeasure();
	afx_msg void OnBnClickedButtonSetBasic();
	afx_msg void OnBnClickedButtonSetNumAlarm();
	afx_msg void OnBnClickedButtonSetRecovery();
	afx_msg void OnBnClickedButtonSetLevel();
	afx_msg void OnBnClickedButtonAbleStart();
	afx_msg void OnBnClickedButtonAbleStop();
	afx_msg void OnBnClickedButtonAbleCancel();
	afx_msg void OnBnClickedButtonSetReflection();
	afx_msg void OnBnClickedButtonSetMask();
	afx_msg void OnBnClickedButtonSetMedian();
	afx_msg void OnBnClickedButtonSetLaserGroup();
	afx_msg void OnBnClickedButtonSetRange();
	afx_msg void OnBnClickedButtonSetMutual();
	afx_msg void OnBnClickedButtonSetMethod();
	afx_msg void OnBnClickedButtonSetTarget();
	afx_msg void OnBnClickedButtonSetAddsub();
	afx_msg void OnBnClickedButtonSetOperation();
	afx_msg void OnBnClickedButtonSetScaling();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
