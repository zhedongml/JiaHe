#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "DlgOtherTab.h"
#include "LkIF2TestVCDlg.h"
#include "LKIF2Tools.h"
#include "Winsock2.h"


IMPLEMENT_DYNAMIC(CDlgOtherTab, CDialog)

CDlgOtherTab::CDlgOtherTab(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOtherTab::IDD, pParent)
{

}

CDlgOtherTab::~CDlgOtherTab()
{
}

void CDlgOtherTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgOtherTab, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPENUSB, &CDlgOtherTab::OnBnClickedButtonOpenUSB)
	ON_BN_CLICKED(IDC_BUTTON_OPENETHER, &CDlgOtherTab::OnBnClickedButtonOpenETHER)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgOtherTab::OnBnClickedButtonClose)
END_MESSAGE_MAP()



void CDlgOtherTab::OnBnClickedButtonOpenUSB()
{
	RC rc = LKIF2_OpenDeviceUsb();
   ((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_OpenDeviceUsb"),rc);
}

void CDlgOtherTab::OnBnClickedButtonOpenETHER()
{
	CString strAddr[4];		
	GetDlgItemText(IDC_EDIT_IP1,strAddr[0]);
	GetDlgItemText(IDC_EDIT_IP2,strAddr[1]);
	GetDlgItemText(IDC_EDIT_IP3,strAddr[2]);
	GetDlgItemText(IDC_EDIT_IP4,strAddr[3]);
	for(int i=0;i<4;++i)
	{
		if(IS_INTEGER_TEXT(strAddr[i]) == FALSE)
		{
			SHOW_ERROR_MSG;
			return;
		}
	}

	char strIp[20];
	_sprintf_p(strIp,20,"%d.%d.%d.%d\0",_ttoi(strAddr[0]),_ttoi(strAddr[1]),_ttoi(strAddr[2]),_ttoi(strAddr[3]));
	LKIF_OPENPARAM_ETHERNET paramEther;
	paramEther.IPAddress.S_un.S_addr = inet_addr(strIp);
	if(paramEther.IPAddress.S_un.S_addr == INADDR_NONE)
	{
		SHOW_ERROR_MSG;
		return;
	}
	RC rc = LKIF2_OpenDeviceETHER(&paramEther);
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_OpenDeviceETHER"),rc);
}

void CDlgOtherTab::OnBnClickedButtonClose()
{
	RC rc = LKIF2_CloseDevice();
	((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_CloseDevice"),rc);
}


BOOL CDlgOtherTab::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_EDIT_IP1,_T("192"));
	SetDlgItemText(IDC_EDIT_IP2,_T("168"));
	SetDlgItemText(IDC_EDIT_IP3,_T("10"));
	SetDlgItemText(IDC_EDIT_IP4,_T("10"));
	return TRUE; 
}

void CDlgOtherTab::OnOK()
{

}

void CDlgOtherTab::OnCancel()
{

}
