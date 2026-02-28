#pragma once



class CDlgOtherTab : public CDialog
{
	DECLARE_DYNAMIC(CDlgOtherTab)

public:
	CDlgOtherTab(CWnd* pParent = NULL);
	virtual ~CDlgOtherTab();

	enum { IDD = IDD_DLG_OTHER_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenUSB();
	afx_msg void OnBnClickedButtonOpenETHER();
	afx_msg void OnBnClickedButtonClose();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
