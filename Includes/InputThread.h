//////////////////////////////////////////////////////////////////////
// InputThread.h: interface for the CInputThread class.
//
// An abstract class for in-game hacking input-simulating thread.
//
// Abin (abinn32@yahoo.com)
// 02/12/2012
//////////////////////////////////////////////////////////////////////

#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include <windows.h>

class __declspec(dllexport) CInputThread
{

public:	

	// Mouse button flags
	enum {	MOUSE_LEFT = 0x01,		// Left mouse button
			MOUSE_RIGHT = 0x02,		// Right mouse button
			MOUSE_MIDDLE = 0x04,	// Middle mouse button
	};

	// Derived classes should use new identifiers no smaller than MSG_APP + 1
	enum {	MSG_NONE = 0,		// N/A
			MSG_START,			// Thread started
			MSG_STOP,			// Thread stopped
			MSG_TICK,			// Thread ticking
			MSG_ALERT,			// Alert started or stopped (lParam 1/0)
			MSG_APP = 255,		// Application defined messages must start after this
	};

	// Common pixel color definition
	enum {	PIXEL_INVALID = -1,					// N/A
			PIXEL_RED = RGB(255, 0, 0),			// Red
			PIXEL_GREEN = RGB(0, 255, 0),		// Green
			PIXEL_BLUE = RGB(0, 0, 255),		// Blue
			PIXEL_YELLOW = RGB(255, 255, 0),	// Yellow	
			
			// Purple and cyan are reserved as alert signal colors
			PIXEL_ALERT_START = RGB(255, 0, 255),	// Purple, alert start
			PIXEL_ALERT_STOP = RGB(0, 255, 255),	// Cyan, alert stop
	};

	// Pixel positions
	enum {	
			POS_INVALID = -1,	// N/A
			POS_TOPLEFT = 0,	// Top left
			POS_TOP,			// Top
			POS_TOPRIGHT,		// Top right
			POS_RIGHT,			// Right
			POS_BOTTOMRIGHT,	// Bottom right
			POS_BOTTOM,			// Bottom
			POS_BOTTOMLEFT,		// Bottom left
			POS_LEFT,			// Left
			POS_CENTER,			// Center
	};

	// Alert status
	enum {
			ALERT_NONE = 0,		// No alert status
			ALERT_START,		// Alert just started
			ALERT_STOP,			// Alert just stopped
			ALERT_CONTINUE,		// Alert continuing
	};

	CInputThread();
	virtual ~CInputThread();

	// Start & stop
	virtual BOOL Start(HWND hInputWnd, DWORD dwActInterval = 5000, BOOL bNeedDC = FALSE); // Start a thread
	virtual BOOL Stop(DWORD dwTimeout = INFINITE); // Stop the running thread

	// Thread status
	BOOL IsRunning() const; // Is the thread is alive?
	BOOL IsStopping() const; // The thread being stopped? Application's ThreadProc function shall keep tracking this!	

	// Conditional sleep, only sleep when IsStopping() returns zero
	virtual void ConditionalSleep(DWORD dwMillseconds) const;
	virtual void SetMessageWnd(HWND hWnd, UINT nMsgID);
	virtual void PostMessage(UINT nType, LPARAM lParam = 0) const;	

	HWND GetInputWnd() const;
	BOOL IsValidInputWnd() const;
	BOOL IsInputWndForeground() const;
	BOOL ActivateInputWnd() const;
	BOOL CloseInputWnd() const;

	// Monitoring input window client DC
	BOOL UpdateDC();
	void ReleaseDC();
	BOOL GetClientRect(LPRECT lpRect) const;
	COLORREF GetPixel(int x, int y) const; // Return CLR_INVALID if invalid
	COLORREF GetPixel(POINT pt, int xOffset = 0, int yOffset = 0) const;
	COLORREF GetPixel(int nPosition) const;
	BOOL WaitForPixel(int nPosition, COLORREF color, DWORD dwTimeout);

	// Play sounds
	static BOOL PlayWaveSound(UINT nResourceID, HMODULE hModule, BOOL bAsync, BOOL bLoop = FALSE);
	static BOOL PlayWaveSound(LPCSTR lpszFilePath, BOOL bAsync, BOOL bLoop = FALSE);
	static void StopWaveSound();

	// Alert
	void Alert(BOOL bPlay);
	BOOL IsAlerting() const;
	void SetAlertSoundFile(LPCSTR lpszSoundFile);
	int UpdateAlertStatus(int nPosition, LPCOLORREF lpColor = NULL);

	// User imput simulation
	static void ReleaseAllKeys();
	static void RemoveKeyRedundancy(WORD wVKCode, WORD& wModifier);
	static void KeyDown(WORD wVKCode, WORD wModifier);
	static void KeyDown(DWORD dwKeys);
	static void KeyUp(WORD wVKCode, WORD wModifier);
	static void KeyUp(DWORD dwKeys);
	static void KeyStroke(WORD wVKCode, WORD wModifier);
	static void KeyStroke(DWORD dwKeys);
	static void MouseDown(WORD wButtons);
	static void MouseUp(WORD wButtons);
	static void MouseClick(WORD wButtons);
	static void MouseDblClick(WORD wButtons);
	static void MouseWheel(BOOL bScrollUp);
	void MouseMove(int x, int y) const; // Move cursor to a position against the client area of game window

protected:
	
	/////////////////////////////////////////////////////////////////////////////
	// The thread process function, must be overridden! This is actually the core
	// of any derived classes. A thread is meaningless without implementing this
	// method. 
	/////////////////////////////////////////////////////////////////////////////
	virtual DWORD ThreadProc() = 0;
	virtual void OnStart() {}
	virtual void OnStop() {}

private:

	static HANDLE _CreateThread(LPTHREAD_START_ROUTINE lpfn, CInputThread* pParam);
	static BOOL _StopThread(HANDLE& hThread, DWORD dwTimeout);	
	static DWORD WINAPI _ThreadProc(LPVOID pParam);
	static DWORD WINAPI _TickProc(LPVOID pParam);

	volatile BOOL m_bAbort; // Abort signal
	HWND m_hInputWnd;
	HDC m_hInputDC;
	RECT m_clientRect;
	HANDLE m_hThread; // The thread handle
	HANDLE m_hTickThread;
	HWND m_hMsgWnd;
	UINT m_nMsgID;
	BOOL m_bAlerting;
	LPSTR m_pszAlertFile;
	DWORD m_dwNextActivate;
	BOOL m_bNeedDC;
	DWORD m_dwActInterval;
};

#endif // __INPUTTHREAD_H__