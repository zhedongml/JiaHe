#pragma once



class CDlgGetTab1 : public CDialog
{
	DECLARE_DYNAMIC(CDlgGetTab1)

public:
	CDlgGetTab1(CWnd* pParent = NULL);
	virtual ~CDlgGetTab1();

	enum { IDD = IDD_DLG_GET_TAB1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGetPanel();
	afx_msg void OnBnClickedButtonGetTolerance();
	afx_msg void OnBnClickedButtonGetAble();
	afx_msg void OnBnClickedButtonGetAbleMinmax();
	afx_msg void OnBnClickedButtonGetMeasure();
	afx_msg void OnBnClickedButtonGetBasic();
	afx_msg void OnBnClickedButtonGetNumAlarm();
	afx_msg void OnBnClickedButtonGetRecovery();
	afx_msg void OnBnClickedButtonGetLevel();
	afx_msg void OnBnClickedButtonGetReflection();
	afx_msg void OnBnClickedButtonGetMask();
	afx_msg void OnBnClickedButtonGetMedian();
	afx_msg void OnBnClickedButtonGetLaserGroup();
	afx_msg void OnBnClickedButtonGetRange();
	afx_msg void OnBnClickedButtonGetMutual();
	afx_msg void OnBnClickedButtonGetMethod();
	afx_msg void OnBnClickedButtonGetTarget();
	afx_msg void OnBnClickedButtonGetAddsub();
	afx_msg void OnBnClickedButtonGetOperation();
	afx_msg void OnBnClickedButtonGetScaling();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
