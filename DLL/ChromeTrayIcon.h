#pragma once

#include "resource.h"

#include "types.h"

#include "TrayIcon.h"
#include "TrayPopup.h"
//#include "MonitoringThread.h"
#include "MinimizeMonitor.h"

#define TRAY_MENU_COMMAND		10000
#define TRAY_OPTIONS_COMMAND	9999

#define TRAY_NEW_TAB_COMMAND	9998
#define TRAY_NEW_WND_COMMAND	9997
#define TRAY_FAVORITES_COMMAND	9000

class CChromeTrayIcon : public CDialogImpl<CChromeTrayIcon>,
						private CMinimizeMonitorCallback
						/*, 
						public CMonitoringThread*/
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

		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)

		COMMAND_ID_HANDLER(TRAY_OPTIONS_COMMAND, OnOptions)
		COMMAND_ID_HANDLER(TRAY_NEW_TAB_COMMAND, OnNewTab)
		COMMAND_ID_HANDLER(TRAY_NEW_WND_COMMAND, OnNewWnd)
		COMMAND_RANGE_HANDLER(TRAY_FAVORITES_COMMAND, TRAY_FAVORITES_COMMAND + 50, OnFavorites)

		CHAIN_MSG_MAP_MEMBER(m_TrayIcon)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	BOOL CreateTrayIcon(HINSTANCE hInstance);
	BOOL DestroyTrayIcon();
	
	BOOL SetTrayIcon(LPCTSTR lpszIconPath);
	BOOL PopupNotify(LPCTSTR lpszTitle, LPCTSTR lpszText);

	void HideChromeWindow(HWND hWnd);
	void ShowChromeWindow(HWND hWnd);

	//void AddChromeWindow(int nWindowId);
	//void RemoveChromeWindow(int nWindowId);
	//void ChromeWindowFocusChanged();

	BOOL OptionsChanged();

protected:
	LRESULT OnTrayMouseCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnOptions(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNewTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNewWnd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFavorites(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void RestoreAllChromeWindows();
	void ShowContextMenu();

private:
	//virtual BOOL Worker();
	BOOL StartMonitoring();
	BOOL StopMonitoring();

	virtual BOOL OnMouseButtonUp(const HWND &targetWindow, const DWORD &mouseButton, const POINT &coord);

	BOOL ReCreateTrayIcon();

	HICON GetChromeWindowIcon();

	HWND FindVisibleChromeWindow();

	BOOL RegisterHotKeys();
	void UnregisterHotKeys();

protected:
	ATOM					m_HotKeyId;
	BOOL					m_bChromeIsHidded;

	CMenu					m_TrayMenu;

	vector<ChromeWindow>	m_ChromeWindows;

	HICON					m_hIcon;

	CTrayPopup				m_TrayPopup;
	CTrayIcon				m_TrayIcon;
	UINT					m_uTrayRestart;

	map<DWORD, wstring>		m_Favorites;

	ChromeTrayIconOptions	m_options;
	ChromeTrayIconLanguage	m_language;

	CRITICAL_SECTION		m_csWindowsList;

	CMinimizeMonitor	m_mouseMonitor;
};

extern CChromeTrayIcon	g_ChromeTrayIcon;