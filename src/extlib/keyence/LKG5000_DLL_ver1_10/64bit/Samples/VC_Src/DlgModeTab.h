#pragma once



class CDlgModeTab : public CDialog
{
	DECLARE_DYNAMIC(CDlgModeTab)

public:
	CDlgModeTab(CWnd* pParent = NULL);
	virtual ~CDlgModeTab();

	enum { IDD = IDD_DLG_MODE_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStartMeasure();
	afx_msg void OnBnClickedButtonStopMeasure();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
