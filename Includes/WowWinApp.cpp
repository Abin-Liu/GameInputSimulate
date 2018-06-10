// WowWinApp.cpp: implementation of the CWowWinApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WowWinApp.h"
#include <shellapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HWND AfxGetWowWindow()
{
	return ::FindWindow(_T("GxWindowClass"), NULL);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWowWinApp::CWowWinApp()
{
	TCHAR szInstallPath[MAX_PATH + 1];
	
	DWORD dwRead = (MAX_PATH + 1) * sizeof(TCHAR);
	HKEY hKey = NULL;
	LONG lRes = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft"), 0, KEY_QUERY_VALUE, &hKey);
	if (lRes != ERROR_SUCCESS || hKey == NULL)
		return;

	DWORD dwType = REG_SZ;
	lRes = ::RegQueryValueEx(hKey, _T("InstallPath"), NULL, &dwType, (LPBYTE)szInstallPath, &dwRead);
	::RegCloseKey(hKey);

	if (lRes == ERROR_SUCCESS)
	{
		m_sWowPath = szInstallPath;
		m_sWowPath.TrimRight(_T('\\'));	
	}	
}

CWowWinApp::~CWowWinApp()
{
}

BOOL CWowWinApp::InitInstance()
{
	if (m_sWowPath.IsEmpty())
	{
		AfxMessageBox(_T("无法读取魔兽世界游戏安装目录，请检查系统注册表。"), MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (_tcsicmp(m_lpCmdLine, _T("-installaddon")) == 0)
	{
		InstallAddOn();
		return FALSE;
	}

	if (_tcsicmp(m_lpCmdLine, _T("-uninstalladdon")) == 0)
	{
		UninstallAddOn();
		return FALSE;
	}	

	SetHelpFile(GetAppDir() + _T("\\") + m_pszAppName + _T(".chm"));
	return TRUE;
}

BOOL CWowWinApp::InstallAddOn() const
{
	if (m_sWowPath.IsEmpty())
		return FALSE;	

	CString sAddonPath = GetAddonPath();

	if (::GetFileAttributes(sAddonPath) != -1)
		UninstallAddOn();
	
	return XCopy(GetAppDir() + _T("\\UI"), sAddonPath);
}

BOOL CWowWinApp::UninstallAddOn() const
{
	if (m_sWowPath.IsEmpty())
		return FALSE;	

	CString sAddonPath = GetAddonPath();

	if (::GetFileAttributes(sAddonPath) == -1)
		return FALSE;	
	
	return DelTree(sAddonPath);
}

CString CWowWinApp::GetAddonPath() const
{
	if (m_sWowPath.IsEmpty())
		return _T("");

	return m_sWowPath + _T("\\Interface\\AddOns\\") + GetAddonName();
}

void CWowWinApp::SetAddonName(LPCTSTR lpszName)
{
	m_sAddonName = lpszName;
}

CString CWowWinApp::GetAddonName() const
{
	return m_sAddonName.IsEmpty() ? m_pszAppName : (LPCTSTR)m_sAddonName;
}

BOOL CWowWinApp::XCopy(LPCTSTR lpszSrc, LPCTSTR lpszDest)
{	
	TCHAR szSource[MAX_PATH + 2] = _T("");
	TCHAR szDestination[MAX_PATH + 2] = _T("");
	::_tcsncpy(szSource, lpszSrc, MAX_PATH);
	::_tcsncpy(szDestination, lpszDest, MAX_PATH);

	SHFILEOPSTRUCT fs;
	::memset(&fs, 0, sizeof(SHFILEOPSTRUCT));

	fs.pFrom = szSource;
	fs.pTo = szDestination;
	fs.wFunc = FO_COPY;	
	fs.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;

	return ::SHFileOperation(&fs) == 0;
}

BOOL CWowWinApp::DelTree(LPCTSTR lpszPath)
{
	TCHAR szSource[MAX_PATH + 2] = _T("");
	::_tcsncpy(szSource, lpszPath, MAX_PATH);

	SHFILEOPSTRUCT fs;
	::memset(&fs, 0, sizeof(SHFILEOPSTRUCT));

	fs.pFrom = szSource;
	fs.wFunc = FO_DELETE;
	fs.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
	
	return ::SHFileOperation(&fs) == 0;
}
