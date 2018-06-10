// GameFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "GameFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_TRAYNOTIFY	(WM_APP + 1357)
#define WM_GAMETHREAD	(WM_APP + 1358)

/////////////////////////////////////////////////////////////////////////////
// CGameFrameWnd

IMPLEMENT_DYNCREATE(CGameFrameWnd, CTrayFrameWnd)

CGameFrameWnd::CGameFrameWnd()
{
	m_hIcon = NULL;
	m_pThread = NULL;
	::memset(&m_shutdown, 0, sizeof(m_shutdown));
}

CGameFrameWnd::~CGameFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CGameFrameWnd, CTrayFrameWnd)
	//{{AFX_MSG_MAP(CGameFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_TRAY_MESSAGE(WM_TRAYNOTIFY)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_GAMETHREAD, PreProcessThreadMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameFrameWnd message handlers

void CGameFrameWnd::SetTitleIcon(LPCTSTR lpszTitle, UINT nIconResID)
{
	m_sTitle = lpszTitle;
	m_hIcon = AfxGetApp()->LoadIcon(nIconResID);
	m_shutdown.SetWndParent(NULL, m_hIcon, m_sTitle);
}

HICON CGameFrameWnd::GetIcon() const
{
	return m_hIcon;
}

CString CGameFrameWnd::GetTitle() const
{
	return m_sTitle;
}

void CGameFrameWnd::SetThread(CInputThread *pThread)
{
	m_pThread = pThread;
}

CInputThread* CGameFrameWnd::GetThread()
{
	return m_pThread;
}

const CInputThread* CGameFrameWnd::GetThread() const
{
	return m_pThread;
}

int CGameFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTrayFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetWindowText(m_sTitle);
	if (m_pThread)
	{
		m_pThread->SetMessageWnd(m_hWnd, WM_GAMETHREAD);
		if (!RegisterPauseKey())
			return -1;
	}
	
	// TODO: Add your specialized creation code here
	CreateTray(WM_TRAYNOTIFY, m_hIcon, m_sTitle, _T("点击这里进行设置"), m_sTitle, NIIF_INFO);
	return 0;

}

void CGameFrameWnd::OnDestroy() 
{
	if (m_pThread)
	{
		m_pThread->Stop();
		UnregisterPauseKey();
	}

	CTrayFrameWnd::OnDestroy();	
}

BOOL CGameFrameWnd::RegisterPauseKey()
{
	while (!::RegisterHotKey(m_hWnd, 0, 0, VK_PAUSE))
	{
		if (MessageBox(_T("无法注册按键\"Pause\"，此按键可能已被其它程序注册，请关闭所有可能注册了这个按键的程序后重试。"), NULL, MB_ICONEXCLAMATION | MB_RETRYCANCEL) == IDCANCEL)
			return FALSE;
	}
	return TRUE;
}

BOOL CGameFrameWnd::UnregisterPauseKey()
{
	return ::UnregisterHotKey(m_hWnd, 0);
}

LRESULT CGameFrameWnd::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0 && m_pThread)
	{
		HWND hWnd = OnRequestGameWnd();
		if (hWnd && ::GetForegroundWindow() == hWnd)
		{
			if (m_pThread->IsRunning())
			{
				if (PreStop())
					m_pThread->Stop();
			}
			else
			{
				if (PreStart())
					m_pThread->Start(hWnd);
			}
		}
		else
		{
			MessageBox(_T("请先将游戏窗口置于前台再按\"Pause\"键。"));
		}
	}
	return 0;
}

LRESULT CGameFrameWnd::PreProcessThreadMsg(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case CInputThread::MSG_START:
		if (m_shutdown.TimeReached())
			m_shutdown.Clear();

		OnStart();
		break;

	case CInputThread::MSG_STOP:
		OnStop();
		break;

	case CInputThread::MSG_TICK:
		if (m_shutdown.TimeReached())
		{
			m_pThread->Stop();
			m_pThread->CloseInputWnd();

			if (m_shutdown.NeedPowerOff())
				m_shutdown.PowerOff();
		}
		else if (!m_pThread->IsInputWndForeground())
		{
			m_pThread->ActivateInputWnd();
		}

		OnTick(lParam);
		break;

	default:
		OnThreadMessage(wParam, lParam);
		break;
	}
	
	return 0;
}

void CGameFrameWnd::ScheduleShutdown()
{
	if (m_pThread)
		m_pThread->Stop();
	m_shutdown.Schedule();
}

BOOL CGameFrameWnd::IsShutdownScheduled() const
{
	return m_shutdown.IsEnabled();
}

void CGameFrameWnd::OnScheduleShutdown()
{
	ScheduleShutdown();
}

void CGameFrameWnd::OnUpdateScheduleShutdown(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsShutdownScheduled());
}
