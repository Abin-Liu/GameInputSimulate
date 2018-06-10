// OSInfo.cpp: implementation of the COSInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OSInfo.h"

#ifndef ULONG_PTR
#define ULONG_PTR	ULONG
#endif
#include <TLHELP32.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define OS_PROCESS_NUM		1024
#define OS_PROCESS_NAME_LEN	1024
#define EXPLORER_NAME		_T("explorer.exe")

const DWORD SHUTDOWN_FLAGS = 0x00040000 | 0x00000001 | 0x80000000;

struct PROCESSIDNAME
{
	DWORD dwProcessID;
	TCHAR szName[MAX_PATH];
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COSInfo::COSInfo()
{
	::memset(&m_ovi, NULL, sizeof(OSVERSIONINFO));
	m_ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&m_ovi);
}

BOOL COSInfo::IsWin3x() const
{
	return m_ovi.dwPlatformId == VER_PLATFORM_WIN32s;
}

BOOL COSInfo::IsWinNT() const
{
	return (m_ovi.dwPlatformId == VER_PLATFORM_WIN32_NT && m_ovi.dwMajorVersion < 5);
}

BOOL COSInfo::IsWin2000() const
{
	return (m_ovi.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& m_ovi.dwMajorVersion == 5
		&& m_ovi.dwMinorVersion == 0
		&& m_ovi.dwBuildNumber == 2195);
}

BOOL COSInfo::IsWinXP() const
{
	return (m_ovi.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& m_ovi.dwMajorVersion == 5
		&& m_ovi.dwMinorVersion == 1);
}

BOOL COSInfo::IsWin95() const
{
	return (m_ovi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS
		&& m_ovi.dwMajorVersion == 4
		&& m_ovi.dwMinorVersion == 0);
}

BOOL COSInfo::IsWin98() const
{
	return (m_ovi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS
		&& m_ovi.dwMajorVersion == 4
		&& m_ovi.dwMinorVersion == 10);
}

BOOL COSInfo::IsWinME() const
{
	return (m_ovi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS
		&& m_ovi.dwMajorVersion == 4
		&& m_ovi.dwMinorVersion == 90);
}

BOOL COSInfo::IsWinCE() const
{
	return m_ovi.dwPlatformId == 3;
}

BOOL COSInfo::IsWin32NTPlateform() const
{
	return m_ovi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
}

CString COSInfo::GetComputerName()
{
	CString str;
	LPTSTR lpBuffer = str.GetBuffer(MAX_COMPUTERNAME_LENGTH + 2);
	DWORD dwLen = MAX_COMPUTERNAME_LENGTH + 1;

	::GetComputerName(lpBuffer, &dwLen);
	str.ReleaseBuffer();
	return str;
}

CString COSInfo::GetUserName()
{
	CString str;
	LPTSTR lpBuffer = str.GetBuffer(256);
	DWORD dwLen = 255;

	::GetUserName(lpBuffer, &dwLen);
	str.ReleaseBuffer();
	return str;
}

CString COSInfo::GetMainIdentity()
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = 255;
	BOOL bSuccess = TRUE;
	TCHAR sz[256] = _T("");
	HKEY hKey;
	CString str;
								 
	if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER, _T("Identities"), 0, KEY_QUERY_VALUE, &hKey))
	{
		return str;
	}


	if (ERROR_SUCCESS == ::RegQueryValueEx(hKey, _T("Default User ID"), NULL,
		&dwType, (LPBYTE)sz, &dwSize))
	{
		str = sz;
	}

	::RegCloseKey(hKey);
	return str;	
}

BOOL COSInfo::AdjustPrivilege(LPCTSTR lpszPrivilege, BOOL bEnable)
{
	HANDLE hToken; 
	// Get a token for this process. 
	if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))	
	{
		return FALSE;
	}

	TOKEN_PRIVILEGES tp;
	LUID luid;

	if ( !::LookupPrivilegeValue( 
        NULL,            // lookup privilege on local system
        lpszPrivilege,   // privilege to lookup 
        &luid ) )
	{   // receives LUID of privilege
	    return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnable)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	::AdjustTokenPrivileges(hToken, 
					FALSE, 
					&tp, 
					sizeof(TOKEN_PRIVILEGES), 
					(PTOKEN_PRIVILEGES) NULL, 
					(PDWORD) NULL); 
 
	// Call GetLastError to determine whether the function succeeded.
	return ::GetLastError() == ERROR_SUCCESS;
}

BOOL COSInfo::WinShutDown(BOOL bForce, BOOL bPowerOff) const
{
	if (!IsWin32NTPlateform())
	{
		if (!AdjustPrivilege(SE_SHUTDOWN_NAME, TRUE))
			return FALSE;
	}
	
	UINT nFlag = bPowerOff ? EWX_POWEROFF : EWX_SHUTDOWN;
	if (bForce)
		nFlag |= EWX_FORCE;

	return KillProcess(GetProcessID(EXPLORER_NAME)) && ::ExitWindowsEx(nFlag, SHUTDOWN_FLAGS);
}

BOOL COSInfo::WinLogOff(BOOL bForce) const
{
	UINT nFlag = EWX_LOGOFF;
	if (bForce)
		nFlag |= EWX_FORCE;
	return KillProcess(GetProcessID(EXPLORER_NAME)) && ::ExitWindowsEx(nFlag, SHUTDOWN_FLAGS);
}

BOOL COSInfo::WinReboot(BOOL bForce) const
{
	if (!IsWin32NTPlateform())
	{
		if (!AdjustPrivilege(SE_SHUTDOWN_NAME, TRUE))
			return FALSE;
	}
	
	UINT nFlag = EWX_REBOOT;
	if (bForce)
		nFlag |= EWX_FORCE;
	return KillProcess(GetProcessID(EXPLORER_NAME)) && ::ExitWindowsEx(nFlag, SHUTDOWN_FLAGS);
}

DWORD COSInfo::GetProcessID(LPCTSTR lpProcessName)
{
	if (lpProcessName == NULL || lpProcessName[0] == _T('\0'))
		return 0;

	PROCESSIDNAME idname = { 0 };
	_tcsncpy(idname.szName, lpProcessName, MAX_PATH - 1);
	EnumProcess(_ProcNameComp, (LPARAM)&idname);
	return idname.dwProcessID;
}

BOOL COSInfo::GetProcessName(DWORD dwProcessID, LPTSTR lpNameBuffer, DWORD dwMaxBufSize)
{
	if (lpNameBuffer == NULL || dwMaxBufSize == 0)
		return FALSE;

	ASSERT(lpNameBuffer != NULL && !::IsBadWritePtr(lpNameBuffer, dwMaxBufSize));
	lpNameBuffer[0] = _T('\0');
	PROCESSIDNAME idname = { 0 };
	idname.dwProcessID = dwProcessID;
	EnumProcess(_ProcIDComp, (LPARAM)&idname);
	_tcsncpy(lpNameBuffer, idname.szName, dwMaxBufSize);
    return idname.szName[0] != _T('\0');
}

BOOL COSInfo::KillProcess(DWORD dwProcessID, UINT nExitCode)
{
	if (dwProcessID == 0) // validate dwProcessId
		return FALSE;

    // get a handle to the process with TERMINATE rights
	HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID); 
	if (hProcess == NULL) // validate process handle
		return FALSE;

	BOOL bResult = ::TerminateProcess(hProcess, nExitCode); // terminate the process
	::CloseHandle(hProcess); // close the handle

	return bResult; // we're done!
}

DWORD COSInfo::GetProcessID(HWND hWnd)
{
	DWORD pid = 0;
	::GetWindowThreadProcessId(hWnd, &pid);
	return pid;
}

BOOL COSInfo::CloseWnd(HWND hWnd, DWORD dwTimeOut)
{
	return ::SendMessageTimeout(hWnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, dwTimeOut, NULL);
}

int COSInfo::EnumProcessModule(DWORD dwProcessID, EnumProcessModuleProc lpFn, LPARAM lParam)
{
	if (lpFn == NULL || dwProcessID == 0)
		return 0;

	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
	if (hModuleSnap == (HANDLE)-1) 
		return 0; 

    MODULEENTRY32 me32 = {0};
	me32.dwSize = sizeof(MODULEENTRY32);
	BOOL bContinue = ::Module32First(hModuleSnap, &me32);
	int nCount = 0;

	if (bContinue)
	{	
		do 
		{
			MODULEINFO mi = { 0 };
			mi.dwBaseAddr = (DWORD)me32.modBaseAddr;
			mi.dwBaseSize = me32.modBaseSize;
			mi.dwProcessID = me32.th32ProcessID;
			mi.hModule = me32.hModule;
			_tcsncpy(mi.szExeFile, me32.szExePath, MAX_PATH - 1);
			_tcsncpy(mi.szModule, me32.szModule, MAX_PATH - 1);

			if (lpFn(&mi, lParam))
				nCount++;
			else
				bContinue = FALSE;
				
		} while (bContinue && ::Module32Next(hModuleSnap, &me32)); 
	}
	
    ::CloseHandle (hModuleSnap); 
	return nCount;
}

int COSInfo::EnumProcess(EnumProcessProc lpFn, LPARAM lParam)
{
	if (lpFn == NULL)
		return 0;

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == (HANDLE)-1) 
		return 0;

	PROCESSENTRY32 pe32 = {0};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bContinue = ::Process32First(hProcessSnap, &pe32);
	int nCount = 0;

	if (bContinue)
	{
		do
		{
			PROCESSINFO pi = { 0 };
			pi.dwProcessID = pe32.th32ProcessID;
			pi.dwParentProcessID = pe32.th32ParentProcessID;
			_tcsncpy(pi.szExeFile, pe32.szExeFile, MAX_PATH - 1);

			if (lpFn(&pi, lParam))
				nCount++;
			else
				bContinue = FALSE;
		} while(bContinue && ::Process32Next(hProcessSnap, &pe32));
	}

	::CloseHandle(hProcessSnap);
	return nCount;
}

BOOL COSInfo::_ProcNameComp(const LPPROCESSINFO lpPI, LPARAM lParam)
{
	PROCESSIDNAME* pIdName = (PROCESSIDNAME*)lParam;
	if (pIdName == NULL)
		return FALSE;

	pIdName->dwProcessID = 0;
	if (_tcsicmp(lpPI->szExeFile, pIdName->szName) == 0)
	{
		pIdName->dwProcessID = lpPI->dwProcessID;
		return FALSE;
	}

	return TRUE;
}

BOOL COSInfo::_ProcIDComp(const LPPROCESSINFO lpPI, LPARAM lParam)
{
	PROCESSIDNAME* pIdName = (PROCESSIDNAME*)lParam;
	if (pIdName == NULL)
		return FALSE;

	pIdName->szName[0] = _T('\0');
	if (lpPI->dwProcessID == pIdName->dwProcessID)
	{
		_tcsncpy(pIdName->szName, lpPI->szExeFile, MAX_PATH - 1);
		return FALSE;
	}

	return TRUE;
}

DWORD COSInfo::GetProcessID(LPCTSTR lpClassName, LPCTSTR lpWndName)
{
	return GetProcessID(::FindWindow(lpClassName, lpWndName));
}
