#include "stdafx.h"
#include "ShutdownApp.h"
#include "ShutdownConfigDlg.h"
#include "ShutdownCountdownDlg.h"
#include "OSInfo.h"
#include "..\..\Includes\Shutdown.h"

CShutdown::CShutdown()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Clear();
}

CShutdown::~CShutdown()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

BOOL CShutdown::IsEnabled() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_bEnable;
}

BOOL CShutdown::TimeReached() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_bEnable && COleDateTime::GetCurrentTime() >= COleDateTime(m_time);
}

BOOL CShutdown::NeedPowerOff() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_bEnable && m_bPowerOff;
}

BOOL CShutdown::Schedule()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CShutdownConfigDlg dlg(m_bEnable, m_bPowerOff, m_time, m_pParent, m_hIcon, m_szTitle);
	if (dlg.DoModal() != IDOK)
		return FALSE;

	m_bEnable = dlg.m_bEnable;
	m_bPowerOff = dlg.m_bTurnOff;
	m_time = dlg.m_autoDate;
	return TRUE;
}

void CShutdown::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bEnable = FALSE;
	m_time = 0.0;
	m_bPowerOff = FALSE;
	m_pParent = NULL;
	m_hIcon = NULL;
	::memset(m_szTitle, 0, sizeof(m_szTitle));
}

BOOL CShutdown::PowerOff(int nCountdown) const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CShutdownCountdownDlg dlg(nCountdown, m_pParent, m_hIcon, m_szTitle);
	if (dlg.DoModal() != IDOK)
		return FALSE;

	COSInfo osi;
	return osi.WinShutDown(TRUE, TRUE);
}

void CShutdown::SetWndParent(CWnd *pParent, HICON hIcon, LPCSTR lpszTitle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pParent = pParent;
	m_hIcon = hIcon;
	::memset(m_szTitle, 0, sizeof(m_szTitle));
	if (lpszTitle)
		::strncpy(m_szTitle, lpszTitle, 255);
}
