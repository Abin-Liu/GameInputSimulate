#if !defined(AFX_SHUTDOWNCONFIGDLG_H__62E27174_F721_4ECC_9E84_7EFD2F3F7D4F__INCLUDED_)
#define AFX_SHUTDOWNCONFIGDLG_H__62E27174_F721_4ECC_9E84_7EFD2F3F7D4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShutdownConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShutdownConfigDlg dialog

class CShutdownConfigDlg : public CDialog
{
// Construction
public:
	CShutdownConfigDlg(BOOL bEnable, BOOL bShutdown, COleDateTime date, CWnd* pParentWnd, HICON hIcon, LPCTSTR lpszTitle);   // standard constructor
	COleDateTime m_autoDate;

// Dialog Data
	//{{AFX_DATA(CShutdownConfigDlg)
	enum { IDD = IDD_CONFIGDLG };
	CButton	m_wndTurnOff;
	CDateTimeCtrl	m_wndTime;
	CButton	m_wndEnable;
	CDateTimeCtrl	m_wndDate;
	COleDateTime	m_date;
	BOOL	m_bEnable;
	COleDateTime	m_time;
	BOOL	m_bTurnOff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShutdownConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShutdownConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnEnable();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	
	HICON m_hIcon;
	CString m_sTitle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHUTDOWNCONFIGDLG_H__62E27174_F721_4ECC_9E84_7EFD2F3F7D4F__INCLUDED_)
