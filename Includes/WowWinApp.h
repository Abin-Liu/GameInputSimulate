//////////////////////////////////////////////////////////////////////
// WowWinApp.h
//
// CWinAppEx derived class specifically used for World of Warcraft game
// hacking programs.
//
// Abin
// 2013-5-04
//////////////////////////////////////////////////////////////////////

#ifndef __WOWWINAPP_H__
#define __WOWWINAPP_H__

#include "WinAppEx.h"

class CWowWinApp : public CWinAppEx  
{
public:		

	CWowWinApp();
	virtual ~CWowWinApp();

	virtual BOOL InitInstance(); // Overridable

	CString GetAddonPath() const;	
	void SetAddonName(LPCTSTR lpszName);
	CString GetAddonName() const;

	virtual BOOL InstallAddOn() const;
	virtual BOOL UninstallAddOn() const;

	static BOOL XCopy(LPCTSTR lpszSrc, LPCTSTR lpszDest);
	static BOOL DelTree(LPCTSTR lpszPath);

private:
	
	CString m_sAddonName;
	CString m_sWowPath;
};

HWND AfxGetWowWindow();

#endif // __WOWWINAPP_H__
