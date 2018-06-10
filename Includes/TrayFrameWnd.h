////////////////////////////////////////////////////////////////////
// TrayFrameWnd.h
//
// CTrayFrameWnd, a CFrameWnd derived class implements system tray
// window combined with Chris Maunder's CSystemTray class.
//
// Abin (abinn32@yahoo.com)
//
// 2012-11-20
////////////////////////////////////////////////////////////////////

#ifndef __TRAYFRAMEWND_H__
#define __TRAYFRAMEWND_H__

#include "SystemTray.h"

#define ON_TRAY_MESSAGE(x)	ON_MESSAGE(x, OnTrayNotify)

class CTrayFrameWnd : public CFrameWnd
{
public:	

	CTrayFrameWnd();
	virtual ~CTrayFrameWnd();

	virtual BOOL CreateTray(UINT nMessageID, HICON hIcon, LPCTSTR lpszTooltip, LPCTSTR lpszBalloonText = NULL, LPCTSTR lpszBalloonTitle = NULL, DWORD dwBalloonIcon = NIIF_INFO, UINT nBalloonTimeout = 10);
	virtual BOOL CreateTray(UINT nMessageID, UINT nIconResID, LPCTSTR lpszTooltip, LPCTSTR lpszBalloonText = NULL, LPCTSTR lpszBalloonTitle = NULL, DWORD dwBalloonIcon = NIIF_INFO, UINT nBalloonTimeout = 10);
	virtual BOOL SetTrayIcon(UINT nIconResID);
	virtual BOOL SetTrayIcon(HICON hIcon);
	virtual BOOL SetTrayTooltipText(LPCTSTR lpszText);
	virtual BOOL ShowTrayBalloon(LPCTSTR lpszText, LPCTSTR lpszTitle, DWORD dwIcon = NIIF_INFO, UINT nBalloonTimeout = 10);

	virtual BOOL PopupMenu(CMenu* pMenu, int nSubMenuIndex = 0, int nDefaultItemPos = -1);
	virtual BOOL PopupMenu(UINT nMenuResID, int nSubMenuIndex = 0, int nDefaultItemPos = -1);
	virtual int MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nType = MB_ICONEXCLAMATION);

	virtual int DoModal(CDialog* pDialog);
	virtual CDialog* RestorePrevDialog();
	CDialog* GetPrevDialog();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayFrameWnd)
	protected:
	//}}AFX_VIRTUAL

protected:	
	
	DECLARE_DYNCREATE(CTrayFrameWnd)
	// Generated message map functions
	//{{AFX_MSG(CTrayFrameWnd)
	//}}AFX_MSG
	LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:	

	virtual void OnConfig() {}
	virtual void OnTrayLClick() {}
	virtual void OnTrayRClick();
	virtual void OnTrayDblClick();
	virtual void OnTrayBalloonClick();
	virtual void OnAppHelp();

	void CmdRouteMenu(CMenu* pMenu);

	CSystemTray m_tray;	

private:
	
	CDialog* m_pDialog;	
};

#endif // __TRAYFRAMEWND_H__
