// ShutdownCountdownDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShutdownApp.h"
#include "ShutdownCountdownDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShutdownCountdownDlg dialog


CShutdownCountdownDlg::CShutdownCountdownDlg(int nCountDown, CWnd* pParentWnd, HICON hIcon, LPCTSTR lpszTitle)
: CDialog(CShutdownCountdownDlg::IDD, pParentWnd ? pParentWnd : GetDesktopWindow())
{
	//{{AFX_DATA_INIT(CShutdownCountdownDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nCountDown = nCountDown >= 0 ? nCountDown : 30;
	m_hIcon = hIcon ? hIcon : AfxGetApp()->LoadIcon(IDI_MAIN);	
	m_sTitle = lpszTitle;
	if (m_sTitle.IsEmpty())	
		m_sTitle = _T("计划关机");
}


void CShutdownCountdownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShutdownCountdownDlg)
	DDX_Control(pDX, IDC_COUNTDOWN, m_wndCountDown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShutdownCountdownDlg, CDialog)
	//{{AFX_MSG_MAP(CShutdownCountdownDlg)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShutdownCountdownDlg message handlers

BOOL CShutdownCountdownDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	SetWindowText(m_sTitle);

	m_font.CreatePointFont(150, _T("Arial Black"));
	m_wndCountDown.SetFont(&m_font);
	UpdateCountdown();
	SetTimer(ID_TMR_COUNTDOWN, 1000, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShutdownCountdownDlg::UpdateCountdown()
{
	CString str;
	str.Format(_T("%d"), m_nCountDown);
	m_wndCountDown.SetWindowText(str);
}

void CShutdownCountdownDlg::OnDestroy() 
{
	KillTimer(ID_TMR_COUNTDOWN);
	CDialog::OnDestroy();
	m_font.DeleteObject();	
}

void CShutdownCountdownDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == ID_TMR_COUNTDOWN)
	{
		m_nCountDown--;
		if (m_nCountDown >= 0)
			UpdateCountdown();
		else
			EndDialog(IDOK);
	}

	CDialog::OnTimer(nIDEvent);
}
