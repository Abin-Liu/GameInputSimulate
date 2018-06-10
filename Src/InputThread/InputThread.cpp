// WoWThread.cpp: implementation of the CInputThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "..\..\Includes\InputThread.h"
#include <Mmsystem.h>
#include <stdlib.h>
#include <malloc.h>

#pragma comment(lib, "Winmm.lib")

#define PROC_SEGMENT	100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputThread::CInputThread()
{
	m_hInputWnd = NULL;
	m_hInputDC = NULL;
	m_hThread = NULL;
	m_hTickThread = NULL;
	m_bAbort = TRUE;
	m_hMsgWnd = NULL;
	m_nMsgID = 0;
	m_bAlerting = FALSE;
	m_pszAlertFile = NULL;
	m_dwNextActivate = 0;
	m_bNeedDC = FALSE;
	m_dwActInterval = 5000;
}

CInputThread::~CInputThread()
{
	Stop();
	ReleaseDC();
	if (m_pszAlertFile)
	{
		::free(m_pszAlertFile);
		m_pszAlertFile = NULL;
	}
}

HANDLE CInputThread::_CreateThread(LPTHREAD_START_ROUTINE lpfn, CInputThread* pParam)
{
	DWORD dwThreadID = 0;	
	return ::CreateThread(NULL, 0, lpfn, (LPVOID)pParam, 0, &dwThreadID);
}

BOOL CInputThread::_StopThread(HANDLE &hThread, DWORD dwTimeout)
{
	BOOL bTimeout = FALSE;
	if (hThread)
	{
		bTimeout = (::WaitForSingleObject(hThread, dwTimeout) == WAIT_TIMEOUT);
		if (bTimeout)
			::TerminateThread(hThread, -1); // Terminate if timeout, this is rude, though.
		::CloseHandle(hThread);
		hThread = NULL;
	}

	if (bTimeout)
		::SetLastError(ERROR_TIMEOUT);
	
	return bTimeout;
}

BOOL CInputThread::Start(HWND hInputWnd, DWORD dwActInterval, BOOL bNeedDC)
{
	if (IsRunning())
	{
		::SetLastError(ERROR_TOO_MANY_TCBS);
		return 0;
	}

	if (!::IsWindow(hInputWnd))
	{
		::SetLastError(ERROR_INVALID_HANDLE);		
		return 0;
	}

	m_dwNextActivate = 0;
	m_hInputWnd = hInputWnd;
	m_bNeedDC = bNeedDC;
	m_dwActInterval = dwActInterval;
	m_bAbort = FALSE;
	m_hThread = _CreateThread(_ThreadProc, this);
	if (!m_hThread)
		return FALSE;

	m_hTickThread = _CreateThread(_TickProc, this);
	return TRUE;
}

BOOL CInputThread::Stop(DWORD dwTimeout)
{
	// terminate the thread if it's running
	m_bAbort = TRUE; // Set the stopping flag	
	_StopThread(m_hTickThread, dwTimeout);	
	return _StopThread(m_hThread, dwTimeout);
}

BOOL CInputThread::IsStopping() const
{
	return m_bAbort;
}

BOOL CInputThread::PlayWaveSound(UINT nResourceID, HMODULE hModule, BOOL bAsync, BOOL bLoop)
{
	StopWaveSound();
	return ::PlaySound(MAKEINTRESOURCE(nResourceID), hModule, SND_RESOURCE | SND_NODEFAULT | (bAsync ? SND_ASYNC : 0) | (bLoop ? SND_LOOP : 0));
}

BOOL CInputThread::PlayWaveSound(LPCSTR lpszFilePath, BOOL bAsync, BOOL bLoop)
{
	StopWaveSound();
	return ::PlaySound(lpszFilePath, NULL, SND_FILENAME | SND_NODEFAULT | (bAsync ? SND_ASYNC : 0) | (bLoop ? SND_LOOP : 0));
}

void CInputThread::StopWaveSound()
{
	::PlaySound(NULL, NULL, SND_NODEFAULT);
}

DWORD CInputThread::_ThreadProc(LPVOID pParam)
{
	CInputThread* pThread = (CInputThread*)pParam;

	if (!pThread->IsValidInputWnd())
		return -1;

	if (!pThread->UpdateDC())
		return 1;

	PlayWaveSound(IDR_START, g_hModule, FALSE);
	pThread->OnStart();
	pThread->PostMessage(MSG_START);

	// Call the thread proc
	DWORD dwExitCode = pThread->ThreadProc();
		
	PlayWaveSound(IDR_STOP, g_hModule, FALSE);
	pThread->OnStop();
	pThread->PostMessage(MSG_STOP);
	pThread->ReleaseDC();

	pThread->m_bAbort = TRUE;	
	return dwExitCode;
}

DWORD CInputThread::_TickProc(LPVOID pParam)
{
	CInputThread* pThread = (CInputThread*)pParam;
	DWORD dwTick = 0;
	while (!pThread->m_bAbort)
	{
		if (!pThread->IsValidInputWnd())
		{
			pThread->m_bAbort = TRUE;
			return 0;
		}
		
		if (pThread->IsInputWndForeground())
		{
			pThread->m_dwNextActivate = 0;
			pThread->PostMessage(MSG_TICK, (LPARAM)dwTick);		
			dwTick++;
		}
		else
		{
			if (pThread->m_dwNextActivate == 0)
			{
				pThread->m_dwNextActivate = ::GetTickCount() + pThread->m_dwActInterval;
			}
			else if (::GetTickCount() > pThread->m_dwNextActivate)
			{
				pThread->m_dwNextActivate = 0;
				pThread->ActivateInputWnd();
			}
		}	

		pThread->ConditionalSleep(PROC_SEGMENT * 2);
	}
	return 0;
}


BOOL CInputThread::IsRunning() const
{
	DWORD dwCode = 0;
	::GetExitCodeThread(m_hThread, &dwCode);
	return dwCode == STILL_ACTIVE;
}

void CInputThread::ConditionalSleep(DWORD dwMillseconds) const
{
	// Only sleep as long as m_iAbort is zero. It may slow your application down
	// if dwMinimumMilliseconds is very small, so beware. It is recommended that
	// value of dwMinimumMilliseconds to be at least 50.
	if (m_bAbort || dwMillseconds == 0)
		return;

	if (dwMillseconds <= PROC_SEGMENT)
	{
		::Sleep(dwMillseconds);
	}
	else
	{
		DWORD dwRemain = dwMillseconds % PROC_SEGMENT;
		DWORD dwSteps = dwMillseconds / PROC_SEGMENT;

		for (DWORD i = 0; !m_bAbort && i < dwSteps; i++)
			::Sleep(PROC_SEGMENT);

		if (!m_bAbort && dwRemain)
			::Sleep(dwRemain);
	}
}

void CInputThread::SetMessageWnd(HWND hWnd, UINT nMsgID)
{
	m_hMsgWnd = hWnd;
	m_nMsgID = nMsgID;
}

void CInputThread::PostMessage(UINT nType, LPARAM lParam) const
{
	if (m_hMsgWnd && m_nMsgID)
		::PostMessage(m_hMsgWnd, m_nMsgID, (WPARAM)nType, lParam);
}

HWND CInputThread::GetInputWnd() const
{
	return m_hInputWnd;
}

BOOL CInputThread::IsValidInputWnd() const
{
	return ::IsWindow(m_hInputWnd);
}

BOOL CInputThread::IsInputWndForeground() const
{
	return ::GetForegroundWindow() == m_hInputWnd;
}

BOOL CInputThread::ActivateInputWnd() const
{
	if (::IsIconic(m_hInputWnd))
		::ShowWindow(m_hInputWnd, SW_RESTORE);

	return ::SetForegroundWindow(m_hInputWnd);	
}

BOOL CInputThread::CloseInputWnd() const
{
	return ::PostMessage(m_hInputWnd, WM_CLOSE, 0, 0);
}

void CInputThread::ReleaseAllKeys()
{
	BYTE aStats[256] = { 0 };
	if (!::GetKeyboardState(aStats))
		return;

	for (WORD i = 0; i < 256; i++)
	{
		if (aStats[i] & 0x80)
			::keybd_event((BYTE)i, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, ::GetMessageExtraInfo());
	}
}

void CInputThread::RemoveKeyRedundancy(WORD wVKCode, WORD& wModifier)
{
	if (wVKCode == VK_SHIFT)
		wModifier &= ~MOD_SHIFT;

	if (wVKCode == VK_CONTROL)
		wModifier &= ~MOD_CONTROL;

	if (wVKCode == VK_MENU)
		wModifier &= ~MOD_ALT;
}

void CInputThread::KeyDown(WORD wVKCode, WORD wModifier)
{
	RemoveKeyRedundancy(wVKCode, wModifier);

	if (wModifier & MOD_ALT)
		::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY, ::GetMessageExtraInfo());

	if (wModifier & MOD_CONTROL)
		::keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY, ::GetMessageExtraInfo());

	if (wModifier & MOD_SHIFT)
		::keybd_event(VK_SHIFT, 0, KEYEVENTF_EXTENDEDKEY, ::GetMessageExtraInfo());

	if (wVKCode)
		::keybd_event((BYTE)wVKCode, 0, KEYEVENTF_EXTENDEDKEY, ::GetMessageExtraInfo());
}

void CInputThread::KeyDown(DWORD dwKeys)
{
	KeyDown(LOWORD(dwKeys), HIWORD(dwKeys));
}

void CInputThread::KeyUp(WORD wVKCode, WORD wModifier)
{
	RemoveKeyRedundancy(wVKCode, wModifier);

	if (wModifier & MOD_ALT)
		::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, ::GetMessageExtraInfo());

	if (wModifier & MOD_CONTROL)
		::keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, ::GetMessageExtraInfo());

	if (wModifier & MOD_SHIFT)
		::keybd_event(VK_SHIFT, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, ::GetMessageExtraInfo());

	if (wVKCode)
		::keybd_event((BYTE)wVKCode, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, ::GetMessageExtraInfo());
}

void CInputThread::KeyUp(DWORD dwKeys)
{
	KeyUp(LOWORD(dwKeys), HIWORD(dwKeys));
}

void CInputThread::KeyStroke(WORD wVKCode, WORD wModifier)
{
	KeyDown(wVKCode, wModifier);
	KeyUp(wVKCode, wModifier);
}

void CInputThread::KeyStroke(DWORD dwKeys)
{
	KeyStroke(LOWORD(dwKeys), HIWORD(dwKeys));
}

void CInputThread::MouseMove(int x, int y) const
{
	if (!IsValidInputWnd())
		return;

	POINT point;
	point.x = x;
	point.y = y;	
	::ClientToScreen(m_hInputWnd, &point);
	::SetCursorPos(point.x, point.y);
}

void CInputThread::MouseDown(WORD wButtons)
{
	if (wButtons & MOUSE_LEFT)
		::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, ::GetMessageExtraInfo());

	if (wButtons & MOUSE_RIGHT)
		::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, ::GetMessageExtraInfo());

	if (wButtons & MOUSE_MIDDLE)
		::mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, ::GetMessageExtraInfo());
}

void CInputThread::MouseUp(WORD wButtons)
{
	if (wButtons & MOUSE_LEFT)
		::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, ::GetMessageExtraInfo());

	if (wButtons & MOUSE_RIGHT)
		::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, ::GetMessageExtraInfo());

	if (wButtons & MOUSE_MIDDLE)
		::mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, ::GetMessageExtraInfo());
}

void CInputThread::MouseClick(WORD wButtons)
{
	MouseDown(wButtons);
	MouseUp(wButtons);
}

void CInputThread::MouseDblClick(WORD wButtons)
{
	MouseClick(wButtons);
	MouseClick(wButtons);
}

void CInputThread::MouseWheel(BOOL bScrollUp)
{
	::mouse_event(MOUSEEVENTF_WHEEL, 0, 0, bScrollUp ? 120 : -120, ::GetMessageExtraInfo());
}

BOOL CInputThread::UpdateDC()
{
	ReleaseDC();
	if (!m_bNeedDC)
		return TRUE;

	GetClientRect(&m_clientRect);
	if (m_hInputWnd && ::IsWindow(m_hInputWnd))
		m_hInputDC = ::GetDCEx(m_hInputWnd, NULL, 0);
	return m_hInputDC != NULL;
}

void CInputThread::ReleaseDC()
{
	if (m_hInputDC)
	{
		::ReleaseDC(m_hInputWnd, m_hInputDC);
		m_hInputDC = NULL;
		::memset(&m_clientRect, 0, sizeof(RECT));
	}
}

BOOL CInputThread::GetClientRect(LPRECT lpRect) const
{
	if (!lpRect)
		return FALSE;

	::memset(lpRect, 0, sizeof(RECT));

	if (!m_hInputWnd)
		return FALSE;

	if (!::GetClientRect(m_hInputWnd, lpRect))
		return FALSE;

	return (lpRect->right > lpRect->left) && (lpRect->bottom > lpRect->top);
}

COLORREF CInputThread::GetPixel(POINT pt, int xOffset, int yOffset) const
{
	return GetPixel(pt.x + xOffset, pt.y + yOffset);
}

COLORREF CInputThread::GetPixel(int x, int y) const
{
	COLORREF clr = CLR_INVALID;
	if (m_hInputDC)
	{
		clr = ::GetPixel(m_hInputDC, x, y);
		if (clr != CLR_INVALID)
		{
			BYTE r = GetRValue(clr);
			BYTE g = GetGValue(clr);
			BYTE b = GetBValue(clr);
			clr = RGB(r, g, b);
		}		
	}
	return clr;
}

COLORREF CInputThread::GetPixel(int nPosition) const
{
	int x = -1, y = -1;
	switch (nPosition)
	{
	case POS_TOPLEFT:
		x = 0;
		y = 0;
		break;
		
	case POS_TOP:
		x = m_clientRect.right / 2;
		y = 0;
		break;

	case POS_TOPRIGHT:
		x = m_clientRect.right - 1;
		y = 0;
		break;

	case POS_RIGHT:
		x = m_clientRect.right - 1;
		y = m_clientRect.bottom / 2;
		break;

	case POS_BOTTOMRIGHT:
		x = m_clientRect.right - 1;
		y = m_clientRect.bottom - 1;
		break;

	case POS_BOTTOM:
		x = m_clientRect.right / 2;
		y = m_clientRect.bottom - 1;
		break;

	case POS_BOTTOMLEFT:
		x = 0;
		y = m_clientRect.bottom - 1;
		break;

	case POS_LEFT:
		x = 0;
		y = m_clientRect.bottom / 2;
		break;

	case POS_CENTER:
		x = m_clientRect.right / 2;
		y = m_clientRect.bottom / 2;
		break;

	default:
		break;
	}

	return GetPixel(x, y);
}

BOOL CInputThread::WaitForPixel(int nPosition, COLORREF color, DWORD dwTimeout)
{
	DWORD dwStart = ::GetTickCount();
	while (TRUE)
	{
		if (GetPixel(nPosition) == color)
			return TRUE;

		if (::GetTickCount() - dwStart > dwTimeout)
			break;

		ConditionalSleep(100);
	}

	return FALSE;
}

BOOL CInputThread::IsAlerting() const
{
	return m_bAlerting;
}

void CInputThread::Alert(BOOL bPlay)
{
	StopWaveSound();
	if (bPlay)
	{
		if (m_pszAlertFile)
			PlayWaveSound(m_pszAlertFile, TRUE, TRUE);
		else
			PlayWaveSound(IDR_ALERT, g_hModule, TRUE, TRUE);
	}
	m_bAlerting = bPlay;
}

int CInputThread::UpdateAlertStatus(int nPosition, LPCOLORREF lpColor)
{
	COLORREF color = GetPixel(nPosition);
	if (lpColor)
		*lpColor = color;
	
	if (IsAlerting())
	{
		// Stop on purple
		if (color == PIXEL_ALERT_STOP)
		{			
			Alert(FALSE);
			PostMessage(MSG_ALERT, 0);
			return ALERT_STOP;
		}

		return ALERT_CONTINUE;
	}
	else
	{
		// Start on red
		if (color == PIXEL_ALERT_START)
		{
			PostMessage(MSG_ALERT, 1);
			Alert(TRUE);
			return ALERT_START;
		}

		return ALERT_NONE;
	}
}

void CInputThread::SetAlertSoundFile(LPCSTR lpszSoundFile)
{
	if (m_pszAlertFile)
	{
		::free(m_pszAlertFile);
		m_pszAlertFile = NULL;
	}

	if (lpszSoundFile && *lpszSoundFile)
		lpszSoundFile = ::_strdup(lpszSoundFile);
}
