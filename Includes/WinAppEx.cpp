//////////////////////////////////////////////////////////////////////
// WinAppEx.cpp
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

#include "stdafx.h" // Include if needed.
#include "WinAppEx.h"
#include <tchar.h>
#include <assert.h>

#define __WINAPPEX_UUID_PREFIX	_T("__WINAPPEX_")
#define __WINAPPEX_MAPFILE_DWORD_COUNT	2

struct __WINAPPEX_ENUMWNDDATA
{
	HWND hWnd;
	DWORD dwProcessID;
};

//////////////////////////////////////////////////////////////////////
// Replacement of MFC AfxGetApp()
//////////////////////////////////////////////////////////////////////
#ifdef __AFXWIN_H__
CWinAppEx* AfxGetAppEx()
{
	return (CWinAppEx*)AfxGetApp();
}
#endif

// Callback procedure for ::EnumWindows, finds the window whose creator's
// process ID matches given ID.
BOOL CALLBACK __WINAPPEX_EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	__WINAPPEX_ENUMWNDDATA* pData = (__WINAPPEX_ENUMWNDDATA*)lParam;
	if (pData == NULL || pData->hWnd != NULL)
		return FALSE;
	
	DWORD dwProcID = 0;
	::GetWindowThreadProcessId(hWnd, &dwProcID);
	if (dwProcID == pData->dwProcessID)
	{
		pData->hWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////
CWinAppEx::CWinAppEx()
{
	m_pszAppDir = NULL;
	m_hMutex = NULL;
	m_hFileMapping = NULL;
	m_pSharedData = NULL;
	m_dwPrevProcID = 0;
	m_pszHelpFile = NULL;
	
	// Copies application directory
	TCHAR sz[MAX_PATH + 1] = _T("");
	::GetModuleFileName(NULL, sz, MAX_PATH);
	LPTSTR p = _tcsrchr(sz, _T('\\'));
	if (p)
		*p = _T('\0');
	m_pszAppDir = _tcsdup(sz);	
}

CWinAppEx::~CWinAppEx()
{
	if (m_pszAppDir)
		::free(m_pszAppDir); // Why not "delete [] m_pszAppDir"? Well, used "_tcsdup", which called "malloc"...

	if (m_pszHelpFile)
		::free(m_pszHelpFile); // See above

	// Decrement instance count right before destruction
	if (m_pSharedData && m_hFileMapping && m_hMutex)
	{
		::WaitForSingleObject(m_hMutex, INFINITE);
		m_pSharedData[0] -= 1;
		m_pSharedData[1] = m_dwPrevProcID;
		::ReleaseMutex(m_hMutex);
	}
	
	if (m_hMutex)
		::CloseHandle(m_hMutex);

	// un-map memory file
	if (m_pSharedData != NULL)
		::UnmapViewOfFile(m_pSharedData);

	// close memory file handle
	if (m_hFileMapping != NULL)
		::CloseHandle(m_hFileMapping);
}

BOOL CWinAppEx::IsInstanceUnique() const
{
	return GetInstanceCount() < 2L; // An instance is unique if the instance count is smaller than 2
}

BOOL CWinAppEx::RestorePrevInstanceMainWnd()
{
	HWND hWnd = GetPrevInstanceMainWnd();
	if (!::IsWindow(hWnd))
		return FALSE;

	HWND hChildWnd = ::GetLastActivePopup(hWnd);

	if (::IsIconic(hWnd))
		::ShowWindow(hWnd, SW_RESTORE);

	if (hChildWnd != NULL)
	{
		if (::IsIconic(hChildWnd))
			::ShowWindow(hChildWnd, SW_RESTORE);
	}

	::SetForegroundWindow(hChildWnd);
	return TRUE;
}

int CWinAppEx::GetAppDir(LPTSTR lpszBuffer, int nBufferSize) const
{
	if (lpszBuffer == NULL || nBufferSize < 1)
		return 0;

	lpszBuffer[0] = 0;
	if (m_pszAppDir)
		_tcsncpy(lpszBuffer, m_pszAppDir, nBufferSize);
	return _tcslen(lpszBuffer);
}

#ifdef __AFXWIN_H__
CString CWinAppEx::GetAppDir() const
{
	return m_pszAppDir ? m_pszAppDir : _T("");
}
#endif

BOOL CWinAppEx::SetInstanceUniqueID(LPCTSTR lpszUniqueID)
{
	if (lpszUniqueID == NULL || m_hMutex || m_hFileMapping || m_pSharedData)
		return FALSE; // You may not call this function twice, and lpszUniqueID must not be NULL
	
	m_hMutex = ::CreateMutex(NULL, FALSE, lpszUniqueID);
	if (m_hMutex == NULL)
		return FALSE;
	
	BOOL bUnique = ::GetLastError() != ERROR_ALREADY_EXISTS;

	// We cannot create a mapping file using a name that was already taken by a mutex, so simply 
	// prefix the name and reuse it.
	LPTSTR psz = new TCHAR[_tcslen(lpszUniqueID) + _tcslen(__WINAPPEX_UUID_PREFIX) + 1];
	_tcscpy(psz, __WINAPPEX_UUID_PREFIX);
	_tcscat(psz, lpszUniqueID);
	m_hFileMapping = ::CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, __WINAPPEX_MAPFILE_DWORD_COUNT * sizeof(DWORD), psz);
	delete [] psz;

	if (m_hFileMapping == NULL)
		return FALSE;

	// Data allocation scheme:
	// Bytes 0-3: Instance count.
	// Bytes 4-7: Previous instance process ID.
	m_pSharedData = (LPDWORD)::MapViewOfFile(m_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, __WINAPPEX_MAPFILE_DWORD_COUNT * sizeof(DWORD));	
	if (m_pSharedData == NULL)
	{
		::CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
		return FALSE;
	}

	::WaitForSingleObject(m_hMutex, INFINITE);
	if (bUnique)
	{	
		// This is the first instance, so initialize the shared data block
		m_dwPrevProcID = 0;
		m_pSharedData[0] = 1;		
	}
	else
	{
		// All subsequent instances need to increment instance count. 
		m_dwPrevProcID = m_pSharedData[1]; // Backup data for later restore
		m_pSharedData[0] += 1; // Increment instance count
	}

	// Every instance must register itself into the shared memory block! 
	m_pSharedData[1] = ::GetCurrentProcessId(); // Register using current process ID
	::ReleaseMutex(m_hMutex);

	return TRUE;
}

LONG CWinAppEx::GetInstanceCount() const
{
	// Simply read byte 0-3 from the shared memory block.
	LONG lCount = 0;
	if (m_pSharedData && m_hFileMapping && m_hMutex)
	{
		::WaitForSingleObject(m_hMutex, INFINITE);
		lCount = (LONG)m_pSharedData[0];
		::ReleaseMutex(m_hMutex);
	}

	// Returns instance count (1-n).
	// Returns 0 means "SetInstanceUniqueID" has not been called, or has but failed.
	return lCount;
}

DWORD CWinAppEx::GetPrevInstanceProcessID() const
{
	return m_dwPrevProcID;
}

HWND CWinAppEx::GetPrevInstanceMainWnd() const
{
	// Enumerate all windows and find the one, if any, whose creator's
	// process ID matches the record.
	if (m_dwPrevProcID == 0)
		return NULL; // I am the first instance, there's no previous one recorded.

	__WINAPPEX_ENUMWNDDATA data;
	data.hWnd = NULL;
	data.dwProcessID = m_dwPrevProcID;
	::EnumWindows(__WINAPPEX_EnumWindowsProc, (LPARAM)&data);
	return data.hWnd;
}

#ifdef __AFXWIN_H__

DOUBLE CWinAppEx::GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, DOUBLE fDefault)
{
	CString str = GetProfileString(lpszSection, lpszEntry);
	str.TrimLeft();
	str.TrimRight();
	return str.IsEmpty() ? fDefault : _tcstod(str, NULL);
}

BOOL CWinAppEx::WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, DOUBLE fValue)
{
	CString str;
	str.Format(_T("%f"), fValue);
	return WriteProfileString(lpszSection, lpszEntry, str);
}

int __CWinAppExParseCommaSeparatedValues(const CString& str, LPLONG pBuffer, int nMaxCount)
{
	int nStart = 0, nEnd = 0, nCount = 0;
	while (nCount < nMaxCount && nEnd < str.GetLength())
	{
		// determine the paragraph ("xxx,xxx,xxx;")
		nEnd = str.Find(_T(','), nStart);
		if( nEnd == -1 )
		{
			// reached the end of string
			nEnd = str.GetLength();
		}

		CString s = str.Mid(nStart, nEnd - nStart);
		s.TrimLeft();
		s.TrimRight();
		pBuffer[nCount] = _tcstol(s, NULL, 10);
		nCount++;
		nStart = nEnd + 1;
	}
	return nCount;
}

POINT CWinAppEx::GetProfilePoint(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG lDefaultX, LONG lDefaultY)
{	
	POINT pt;
	pt.x = lDefaultX;
	pt.y = lDefaultY;

	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.IsEmpty())
		return pt;

	LONG ls[2];
	if (__CWinAppExParseCommaSeparatedValues(str, ls, 2) != 2)
		return pt;

	pt.x = ls[0];
	pt.y = ls[1];	
	return pt;
}

BOOL CWinAppEx::WriteProfilePoint(LPCTSTR lpszSection, LPCTSTR lpszEntry, const POINT* lpPoint)
{
	assert(lpPoint);
	CString str;
	str.Format(_T("%d,%d"), lpPoint->x, lpPoint->y);
	return WriteProfileString(lpszSection, lpszEntry, str);
}
	
RECT CWinAppEx::GetProfileRect(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG lDefaultL, LONG lDefaultT, LONG lDefaultR, LONG lDefaultB)
{
	RECT rect;
	rect.left = lDefaultL;
	rect.top = lDefaultT;
	rect.right = lDefaultR;
	rect.bottom = lDefaultB;

	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.IsEmpty())
		return rect;

	LONG ls[4];
	if (__CWinAppExParseCommaSeparatedValues(str, ls, 4) != 4)
		return rect;

	rect.left = ls[0];
	rect.top = ls[1];
	rect.right = ls[2];
	rect.bottom = ls[3];
	return rect;
}

BOOL CWinAppEx::WriteProfileRect(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCRECT lpRect)
{
	assert(lpRect);
	CString str;
	str.Format(_T("%d,%d,%d,%d"), lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	return WriteProfileString(lpszSection, lpszEntry, str);
}

#endif

void CWinAppEx::WinHelp(DWORD dwData, UINT nCmd) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_pszHelpFile)
	{
		CWinApp::WinHelp(dwData, nCmd);
		return;
	}

	LPTSTR p = _tcsrchr(m_pszHelpFile, _T('.'))	;
	if (p && _tcsicmp(p, _T(".chm")) == 0)
		::ShellExecute(NULL, _T("open"), _T("hh.exe"), m_pszHelpFile, NULL, SW_SHOW);
	else
		::ShellExecute(NULL, _T("open"), m_pszHelpFile, NULL, NULL, SW_SHOW);
}

void CWinAppEx::SetHelpFile(LPCTSTR lpszFilePath)
{
	if (m_pszHelpFile)
	{
		::free(m_pszHelpFile);
		m_pszHelpFile = NULL;
	}

	if (lpszFilePath && *lpszFilePath)
		m_pszHelpFile = _tcsdup(lpszFilePath);
}

LPCTSTR CWinAppEx::GetHelpFile() const
{
	return m_pszHelpFile;
}
