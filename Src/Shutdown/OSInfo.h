// OSInfo.h: interface for the COSInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	__OSINFO_H__
#define __OSINFO_H__

// Reason for defining the following 2 structs is to get rid of useless info contained
// in tool-helper 32 structures, also no need to include "TLHELP32.H" in this
// header file.

// Process information
typedef struct tagProcessInfo
{
	DWORD dwProcessID;
	DWORD dwParentProcessID;
	TCHAR szExeFile[MAX_PATH];
} PROCESSINFO, *LPPROCESSINFO;

// Process module information
typedef struct tagModuleInfo
{
	DWORD dwProcessID;
	DWORD dwBaseAddr;
	DWORD dwBaseSize;
	HMODULE hModule;
	TCHAR szModule[MAX_PATH];
	TCHAR szExeFile[MAX_PATH];
} MODULEINFO, *LPMODULEINFO;

// Call-back functions to process each process and module.
typedef BOOL (CALLBACK *EnumProcessProc)(const LPPROCESSINFO lpPI32, LPARAM lParam);
typedef BOOL (CALLBACK *EnumProcessModuleProc)(const LPMODULEINFO lpMI32, LPARAM lParam);

class COSInfo
{
public:		
	
	COSInfo();

	BOOL IsWin3x() const;
	BOOL IsWin95() const;
	BOOL IsWin98() const;
	BOOL IsWinME() const;
	BOOL IsWinNT() const;
	BOOL IsWin2000() const;
	BOOL IsWinXP() const;
	BOOL IsWinCE() const;
	BOOL IsWin32NTPlateform() const;

	static CString GetMainIdentity();
	static CString GetUserName();
	static CString GetComputerName();

	BOOL WinLogOff(BOOL bForce = FALSE) const;
	BOOL WinReboot(BOOL bForce = FALSE) const;
	BOOL WinShutDown(BOOL bForce = FALSE, BOOL bPowerOff = TRUE) const;
	
	static BOOL AdjustPrivilege(LPCTSTR lpszPrivilege, BOOL bEnable);

	static int EnumProcess(EnumProcessProc lpFn, LPARAM lParam);
	static int EnumProcessModule(DWORD dwProcessID, EnumProcessModuleProc lpFn, LPARAM lParam);
	static DWORD GetProcessID(LPCTSTR lpProcessName);
	static DWORD GetProcessID(LPCTSTR lpClassName, LPCTSTR lpWndName);
	static DWORD GetProcessID(HWND hWnd);
	static BOOL GetProcessName(DWORD dwProcessID, LPTSTR lpNameBuffer, DWORD dwMaxBufSize);
	static BOOL KillProcess(DWORD dwProcessID, UINT nExitCode = 0);
	static BOOL CloseWnd(HWND hWnd, DWORD dwTimeOut);

protected:
	static BOOL CALLBACK _ProcIDComp(const LPPROCESSINFO lpPI, LPARAM lParam);
	static BOOL CALLBACK _ProcNameComp(const LPPROCESSINFO lpPI, LPARAM lParam);
	OSVERSIONINFO m_ovi;
};

#endif
