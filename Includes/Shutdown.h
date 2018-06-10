/////////////////////////////////////////////////////////
// Shutdown.h
//
// A simple resource DLL that exports a couple of CDialog
// derived windows for scheduling/performing automatic game
// closing and system power-off.
//
// Abin (abinn32@yahoo.com)
// 2012-11-19
/////////////////////////////////////////////////////////

class __declspec(dllexport) CShutdown
{
public:

	CShutdown();
	~CShutdown();

	BOOL IsEnabled() const;
	BOOL TimeReached() const;
	BOOL NeedPowerOff() const;

	void SetWndParent(CWnd* pParent, HICON hIcon, LPCSTR lpszTitle);
	void Clear();
	BOOL Schedule();	
	BOOL PowerOff(int nCountdown = 30) const;

private:

	BOOL m_bEnable;
	DATE m_time;
	BOOL m_bPowerOff;
	CWnd* m_pParent;
	HICON m_hIcon;
	char m_szTitle[256];
};
