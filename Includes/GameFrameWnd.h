#ifndef __GAMEMAINFRAME_H__
#define __GAMEMAINFRAME_H__

#include "InputThread.h"
#include "TrayFrameWnd.h"
#include "Shutdown.h"

#define GAME_WND_CLASS_WOW			_T("GxWindowClass")			// World of Warcraft
#define GAME_WND_CLASS_DIABLO3		_T("D3 Main Window Class")	// Diablo III

class CGameFrameWnd : public CTrayFrameWnd
{	
public:
	
	CGameFrameWnd();
	virtual ~CGameFrameWnd();
	
	virtual BOOL RegisterPauseKey();
	virtual BOOL UnregisterPauseKey();

	void SetTitleIcon(LPCTSTR lpszTitle, UINT nIconResID);
	CString GetTitle() const;
	HICON GetIcon() const;

	virtual void ScheduleShutdown();
	BOOL IsShutdownScheduled() const;

	virtual void SetThread(CInputThread* pThread);
	CInputThread* GetThread();
	const CInputThread* GetThread() const;

protected:
	
	DECLARE_DYNCREATE(CGameFrameWnd)
	// Generated message map functions
	//{{AFX_MSG(CGameFrameWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	LRESULT PreProcessThreadMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	
	virtual HWND OnRequestGameWnd() const { return NULL; }	
	virtual void OnStart() {}
	virtual void OnStop() {}
	virtual BOOL PreStart() { return TRUE; }
	virtual BOOL PreStop() { return TRUE; }
	virtual void OnTick(DWORD dwTickCount) {}
	virtual void OnThreadMessage(WPARAM wParam, LPARAM lParam) {}
	virtual void OnScheduleShutdown();
	virtual void OnUpdateScheduleShutdown(CCmdUI* pCmdUI);

private:
	
	CInputThread* m_pThread;
	HICON m_hIcon;
	CString m_sTitle;
	CShutdown m_shutdown;
};

#endif // __GAMEMAINFRAME_H__
