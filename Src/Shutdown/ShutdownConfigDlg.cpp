// ShutdownConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShutdownApp.h"
#include "ShutdownConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShutdownConfigDlg dialog


CShutdownConfigDlg::CShutdownConfigDlg(BOOL bEnable, BOOL bShutdown, COleDateTime date, CWnd* pParentWnd, HICON hIcon, LPCTSTR lpszTitle)
: CDialog(CShutdownConfigDlg::IDD, pParentWnd ? pParentWnd : GetDesktopWindow())
{
	//{{AFX_DATA_INIT(CShutdownConfigDlg)
	m_date = date;
	m_bEnable = bEnable;
	m_bTurnOff = bShutdown;
	m_time = date;
	//}}AFX_DATA_INIT	
	m_autoDate = date;
	if (!bEnable)
	{
		COleDateTime def = COleDateTime::GetCurrentTime() + COleDateTimeSpan(0, 8, 0, 0);
		m_date = def;
		m_time = def;
		m_autoDate = def;
	}

	m_hIcon = hIcon ? hIcon : AfxGetApp()->LoadIcon(IDI_MAIN);
	m_sTitle = lpszTitle;
	if (m_sTitle.IsEmpty())	
		m_sTitle = _T("自动关机设置");
}

void CShutdownConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShutdownConfigDlg)
	DDX_Control(pDX, IDC_TURNOFF, m_wndTurnOff);
	DDX_Control(pDX, IDC_TIME, m_wndTime);
	DDX_Control(pDX, IDC_ENABLE, m_wndEnable);
	DDX_Control(pDX, IDC_DATE, m_wndDate);
	DDX_DateTimeCtrl(pDX, IDC_DATE, m_date);
	DDX_Check(pDX, IDC_ENABLE, m_bEnable);
	DDX_DateTimeCtrl(pDX, IDC_TIME, m_time);
	DDX_Check(pDX, IDC_TURNOFF, m_bTurnOff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShutdownConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CShutdownConfigDlg)
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShutdownConfigDlg message handlers

BOOL CShutdownConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	SetWindowText(m_sTitle);

	m_wndDate.EnableWindow(m_bEnable);
	m_wndTime.EnableWindow(m_bEnable);
	m_wndTurnOff.EnableWindow(m_bEnable);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShutdownConfigDlg::OnEnable() 
{
	// TODO: Add your control notification handler code here
	m_bEnable = m_wndEnable.GetCheck();
	m_wndDate.EnableWindow(m_bEnable);
	m_wndTime.EnableWindow(m_bEnable);
	m_wndTurnOff.EnableWindow(m_bEnable);
}

void CShutdownConfigDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);	
	COleDateTime temp;
	temp.SetDateTime(m_date.GetYear(), m_date.GetMonth(), m_date.GetDay(), m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());
	if (m_bEnable && COleDateTime::GetCurrentTime() >= temp)
	{
		MessageBox("设定时间不得早于当前系统时间。", "自动关机", MB_ICONEXCLAMATION);
		m_wndDate.SetFocus();
		return;
	}
	
	m_autoDate = temp;
	EndDialog(IDOK);
}
