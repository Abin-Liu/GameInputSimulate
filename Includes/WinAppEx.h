//////////////////////////////////////////////////////////////////////
// WinAppEx.h
//
// CWinAppEx derives from MFC CWinApp class(if using MFC), it can be used for:
//
// 1, Limiting an application in single instance.
// 2, Easily obtaining application directory.
// 3, Finding and restoring main window of previous instance. 
//
// Abin (abinn32@yahoo.com)
// 3rd May, 2006
//////////////////////////////////////////////////////////////////////

#ifndef __WINAPPEX_H__
#define __WINAPPEX_H__

#include <Windows.h>

class CWinAppEx
#ifdef __AFXWIN_H__
: public CWinApp 
#endif
{
public:	

	////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////
	CWinAppEx();
	virtual ~CWinAppEx();

	////////////////////////////////////////////////////////////////////////////
	// Convenient help access
	////////////////////////////////////////////////////////////////////////////
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	LPCTSTR GetHelpFile() const;
	virtual void SetHelpFile(LPCTSTR lpszFilePath);

	////////////////////////////////////////////////////////////////////////////
	// Methods for Instance Limit & Others
	////////////////////////////////////////////////////////////////////////////
	virtual BOOL SetInstanceUniqueID(LPCTSTR lpszUniqueID); // Assign an unique string to the instance
	LONG GetInstanceCount() const; // Retrieve instance count
	BOOL IsInstanceUnique() const;		// TRUE if the instance is unique
	DWORD GetPrevInstanceProcessID() const;
	HWND GetPrevInstanceMainWnd() const;
	BOOL RestorePrevInstanceMainWnd(); // Restore the main window of previous instance.

	////////////////////////////////////////////////////////////////////////////
	// Application directory Access
	////////////////////////////////////////////////////////////////////////////
	int GetAppDir(LPTSTR lpszBuffer, int nBufferSize) const;
#ifdef __AFXWIN_H__
	CString GetAppDir() const;
#endif

	////////////////////////////////////////////////////////////////////////////
	// Additional profile access
	////////////////////////////////////////////////////////////////////////////
#ifdef __AFXWIN_H__
	DOUBLE GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, DOUBLE fDefault = 0.0);
	BOOL WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, DOUBLE fValue);
	POINT GetProfilePoint(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG lDefaultX = 0, LONG lDefaultY = 0);
	BOOL WriteProfilePoint(LPCTSTR lpszSection, LPCTSTR lpszEntry, const POINT* lpPoint);	
	RECT GetProfileRect(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG lDefaultL = 0, LONG lDefaultT = 0, LONG lDefaultR = 0, LONG lDefaultB = 0);
	BOOL WriteProfileRect(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCRECT lpRect);
#endif
	
protected:

	LPTSTR m_pszAppDir; // Application directory
	HANDLE m_hMutex; // Mutex
	HANDLE m_hFileMapping; // File mapping handle
	LPDWORD m_pSharedData; // File mapping data pointer
	DWORD m_dwPrevProcID; // A local copy of the previous instance process ID
	LPTSTR m_pszHelpFile; // Help file name
};

#ifdef __AFXWIN_H__
CWinAppEx* AfxGetAppEx(); // The global function to replace MFC AfxGetApp()
#endif

#endif // __WINAPPEX_H__