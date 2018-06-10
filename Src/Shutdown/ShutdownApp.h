// Shutdown.h : main header file for the SHUTDOWN DLL
//

#if !defined(AFX_SHUTDOWN_H__7F628FAE_90E4_4CB7_B2FA_8296490BC2FC__INCLUDED_)
#define AFX_SHUTDOWN_H__7F628FAE_90E4_4CB7_B2FA_8296490BC2FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CShutdownApp
// See Shutdown.cpp for the implementation of this class
//

class CShutdownApp : public CWinApp
{
public:
	CShutdownApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShutdownApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CShutdownApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHUTDOWN_H__7F628FAE_90E4_4CB7_B2FA_8296490BC2FC__INCLUDED_)
