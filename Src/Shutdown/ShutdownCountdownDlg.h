#if !defined(AFX_SHUTDOWNCOUNTDOWNDLG_H__537AEDBC_8B7F_45A4_A5C4_6FFB42B6B68A__INCLUDED_)
#define AFX_SHUTDOWNCOUNTDOWNDLG_H__537AEDBC_8B7F_45A4_A5C4_6FFB42B6B68A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShutdownCountdownDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShutdownCountdownDlg dialog

class CShutdownCountdownDlg : public CDialog
{
// Construction
public:
	CShutdownCountdownDlg(int nCountdown, CWnd* pParentWnd, HICON hIcon, LPCTSTR lpszTitle);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShutdownCountdownDlg)
	enum { IDD = IDD_SHUTDOWNCOUNTDOWNDLG };
	CStatic	m_wndCountDown;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShutdownCountdownDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShutdownCountdownDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	
	void UpdateCountdown();

	int m_nCountDown;
	CFont m_font;
	HICON m_hIcon;
	CString m_sTitle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHUTDOWNCOUNTDOWNDLG_H__537AEDBC_8B7F_45A4_A5C4_6FFB42B6B68A__INCLUDED_)
