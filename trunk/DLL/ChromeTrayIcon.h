#pragma once

#include "resource.h"

#include "types.h"

#include "TrayIcon.h"
#include "MonitoringThread.h"

#define TRAY_MENU_COMMAND		10000
#define TRAY_OPTIONS_COMMAND	9999

#define TRAY_NEW_TAB_COMMAND	9998
#define TRAY_NEW_WND_COMMAND	9997

class CChromeTrayIcon : public CDialogImpl<CChromeTrayIcon>, 
						public CMonitoringThread
{
public:
	enum { IDD = IDD_DUMMY };

public:
	CChromeTrayIcon(void);
	~CChromeTrayIcon(void);

	BEGIN_MSG_MAP(CChromeTrayIcon)
		if(uMsg == m_uTrayRestart)
		{
			ReCreateTrayIcon();
		}

		MESSAGE_HANDLER(WM_TRAY_LCLICK, OnTrayMouseCommand)
		MESSAGE_HANDLER(WM_TRAY_LDBLCLICK, OnTrayMouseCommand)
		MESSAGE_HANDLER(WM_TRAY_RCLICK, OnTrayMouseCommand)
		MESSAGE_HANDLER(WM_TRAY_RDBLCLICK, OnTrayMouseCommand)

		COMMAND_ID_HANDLER(TRAY_OPTIONS_COMMAND, OnOptions)
		COMMAND_ID_HANDLER(TRAY_NEW_TAB_COMMAND, OnNewTab)
		COMMAND_ID_HANDLER(TRAY_NEW_WND_COMMAND, OnNewWnd)

		CHAIN_MSG_MAP_MEMBER(m_TrayIcon)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	BOOL CreateTrayIcon(HINSTANCE hInstance);
	BOOL DestroyTrayIcon();
	BOOL SetTrayIcon(LPCTSTR lpszIconPath);

	void HideChromeWindow(HWND hWnd);
	void ShowChromeWindow(HWND hWnd);

	void AddChromeWindow(int nWindowId);
	void RemoveChromeWindow(int nWindowId);
	void ChromeWindowFocusChanged();

	BOOL OptionsChanged();

protected:
	LRESULT OnTrayMouseCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnOptions(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNewTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNewWnd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void RestoreAllChromeWindows();
	void ShowContextMenu();

protected:
	virtual BOOL Worker();

	BOOL ReCreateTrayIcon();

	HICON GetChromeWindowIcon();

	HWND FindVisibleChromeWindow();

protected:
	CMenu					m_TrayMenu;

	vector<ChromeWindow>	m_ChromeWindows;

	HICON					m_hIcon;

	CTrayIcon				m_TrayIcon;
	UINT					m_uTrayRestart;

	ChromeTrayIconOptions	m_options;
	ChromeTrayIconLanguage	m_language;
};

extern CChromeTrayIcon	g_ChromeTrayIcon;