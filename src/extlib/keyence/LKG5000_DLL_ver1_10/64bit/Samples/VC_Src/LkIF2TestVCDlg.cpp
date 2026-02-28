#include "stdafx.h"
#include "LkIF2TestVC.h"
#include "LkIF2TestVCDlg.h"
#include "LKIF2Tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

// ŽÀ‘•
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()




CLkIF2TestVCDlg::CLkIF2TestVCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLkIF2TestVCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLkIF2TestVCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CLkIF2TestVCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CLkIF2TestVCDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()



BOOL CLkIF2TestVCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	{
		m_tabGet1.Create(CDlgGetTab1::IDD,this);
		m_tabGet2.Create(CDlgGetTab2::IDD,this);
		m_tabSet1.Create(CDlgSetTab1::IDD,this);
		m_tabSet2.Create(CDlgSetTab2::IDD,this);
		m_tabMeasureCtl.Create(CDlgMeasureCtlTab::IDD,this);
		m_tabMode.Create(CDlgModeTab::IDD,this);
		m_tabOther.Create(CDlgOtherTab::IDD,this);
		CRect rect;
		rect.SetRect(15,36,915,817);
		m_tabGet1.MoveWindow(&rect,FALSE);
		m_tabGet2.MoveWindow(&rect,FALSE);
		m_tabSet1.MoveWindow(&rect,FALSE);
		m_tabSet2.MoveWindow(&rect,FALSE);
		m_tabMeasureCtl.MoveWindow(&rect,FALSE);
		m_tabMode.MoveWindow(&rect,FALSE);
		m_tabOther.MoveWindow(&rect,FALSE);

		m_TabCtrl.InsertItem(0,_T("Measurement control commands"));
		m_TabCtrl.InsertItem(1,_T("Setting change commands 1"));
		m_TabCtrl.InsertItem(2,_T("Setting change commands 2"));
		m_TabCtrl.InsertItem(3,_T("Setting confirmation commands 1"));
		m_TabCtrl.InsertItem(4,_T("Setting confirmation commands 2"));
		m_TabCtrl.InsertItem(5,_T("Mode change commands"));
		m_TabCtrl.InsertItem(6,_T("Other commands"));

		OnTcnSelchangeTab(0,0);
		CheckDlgButton(IDC_CHECK_MSG_SHOW,BST_CHECKED);
		
	}
	return TRUE;
}

void CLkIF2TestVCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CLkIF2TestVCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CLkIF2TestVCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLkIF2TestVCDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int sel = m_TabCtrl.GetCurSel();
	
	m_tabMeasureCtl.ShowWindow((sel==0)? SW_SHOW :SW_HIDE);
	m_tabSet1.ShowWindow((sel==1)? SW_SHOW :SW_HIDE);
	m_tabSet2.ShowWindow((sel==2)? SW_SHOW :SW_HIDE);
	m_tabGet1.ShowWindow((sel==3)? SW_SHOW :SW_HIDE);
	m_tabGet2.ShowWindow((sel==4)? SW_SHOW :SW_HIDE);
	m_tabMode.ShowWindow((sel==5)? SW_SHOW :SW_HIDE);
	m_tabOther.ShowWindow((sel==6)? SW_SHOW :SW_HIDE);
	if(pResult)*pResult = 0;
}

void CLkIF2TestVCDlg::DisplayCommandResult(CString strCommand,RC rc)
{
	CString strDisplay = _T("");
	if(IS_RC_SUCCESS(rc))
	{		
		strDisplay.Format(_T("%s is succeeded. RC = 0x%X"),strCommand,rc);
		((CStatic*)GetDlgItem(IDC_STATIC_RESULT))->SetWindowText(strDisplay);	
	}
	else
	{

		strDisplay.Format(_T("Failed in %s. RC = 0x%X"),strCommand,rc);
		UpdateData();
		if((((CButton*)GetDlgItem(IDC_CHECK_MSG_SHOW))->GetState() & 0x01) == 0x01)
		{
			AfxMessageBox(strDisplay);
		}
		((CStatic*)GetDlgItem(IDC_STATIC_RESULT))->SetWindowText(strDisplay);	
	}
}



CString CLkIF2TestVCDlg::GetTextFromCombo(int nId,int sel)
{
	CString result(_T(""));
	CWnd * pWnd = GetWindowHasControl(nId);
	if(pWnd == NULL)
	{
		return result;
	}

	GET_TEXT_FROM_COMBO(pWnd,nId,sel,result);
	return result;
}

CWnd* CLkIF2TestVCDlg::GetWindowHasControl(int nId)
{
	switch(nId)
	{
	case IDC_COMBO_SET_PANEL_PARAM1:
	case IDC_COMBO_SET_PANEL_PARAM2:
	case IDC_COMBO_SET_ABLE_PARAM2:
	case IDC_COMBO_SET_MEASURE_PARAM2:
	case IDC_COMBO_SET_BASIC_PARAM2:
	case IDC_COMBO_SET_REFLECTION_PARAM2:
	case IDC_COMBO_SET_MASK_PARAM2:
	case IDC_COMBO_SET_MEDIAN_PARAM2:
	case IDC_COMBO_SET_LASER_GROUP_PARAM2:
	case IDC_COMBO_SET_RANGE_PARAM2:
	case IDC_COMBO_SET_MUTUAL_PARAM2:
	case IDC_COMBO_SET_METHOD_PARAM2:
	case IDC_COMBO_SET_TARGET_PARAM2:
	case IDC_COMBO_SET_ADDSUB_PARAM2:
	case IDC_COMBO_SET_ADDSUB_PARAM3:
		return &m_tabSet1;
		break;
	case IDC_COMBO_SET_FILTER_PARAM2:
	case IDC_COMBO_SET_FILTER_PARAM3:
	case IDC_COMBO_SET_TRIGGER_PARAM2:
	case IDC_COMBO_SET_CALCMODE_PARAM2:
	case IDC_COMBO_SET_UNIT_PARAM2:
	case IDC_COMBO_SET_MEASURETYPE_PARAM2:
	case IDC_COMBO_SET_SYNC_PARAM2:
	case IDC_COMBO_SET_STORAGE_TARGET_PARAM2:
	case IDC_COMBO_SET_SAMPLING_PARAM:
	case IDC_COMBO_SET_MUTUAL_PREV_PARAM:
	case IDC_COMBO_SET_TOLERANCE_FORMAT_PARAM:
	case IDC_COMBO_SET_STROBE_PARAM:
	case IDC_COMBO_SET_STORAGE_PARAM:
	case IDC_COMBO_SET_ANALOG_CH_PARAM2:
	case IDC_COMBO_SET_ALARM_FORM_PARAM:
		return &m_tabSet2;
		break;
	default:
		break;

	}
	return NULL;
}
