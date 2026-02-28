#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "DlgModeTab.h"
#include "LkIF2TestVCDlg.h"


IMPLEMENT_DYNAMIC(CDlgModeTab, CDialog)

CDlgModeTab::CDlgModeTab(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModeTab::IDD, pParent)
{

}

CDlgModeTab::~CDlgModeTab()
{
}

void CDlgModeTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgModeTab, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_START_MEASURE, &CDlgModeTab::OnBnClickedButtonStartMeasure)
	ON_BN_CLICKED(IDC_BUTTON_STOP_MEASURE, &CDlgModeTab::OnBnClickedButtonStopMeasure)
END_MESSAGE_MAP()



void CDlgModeTab::OnBnClickedButtonStartMeasure()
{
	RC rc = LKIF2_StartMeasure();
   ((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_StartMeasure"),rc);
}

void CDlgModeTab::OnBnClickedButtonStopMeasure()
{
   RC rc = LKIF2_StopMeasure();
   ((CLkIF2TestVCDlg*)GetParent())->DisplayCommandResult(_T("LKIF2_StopMeasure"),rc);
}

void CDlgModeTab::OnOK()
{
}

void CDlgModeTab::OnCancel()
{
}
