#pragma once

#include "resource.h"

#include "TrayIcon.h"
#include "MonitoringThread.h"

class CChromeTrayIcon : public CDialogImpl<CChromeTrayIcon>, public CMonitoringThread
{
public:
	enum { IDD = IDD_DUMMY };

public:
	CChromeTrayIcon(void);
	~CChromeTrayIcon(void);

	BEGIN_MSG_MAP(CChromeTrayIcon)
		MESSAGE_HANDLER(WM_TRAY_DBLCLICK, OnTrayDblClick)
		MESSAGE_HANDLER(WM_TRAY_CONTEXT_MENU, OnTrayContextMenu)

		CHAIN_MSG_MAP_MEMBER(m_TrayIcon)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	BOOL CreateTrayIcon(HINSTANCE hInstance);
	BOOL DestroyTrayIcon();

	void HideChromeWindow(HWND hWnd);
	void ShowChromeWindow(HWND hWnd);

protected:
	LRESULT OnTrayDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTrayContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	virtual BOOL Worker();

protected:
	vector<HWND>		m_HiddenWindows;

	HICON				m_hIcon;
	CTrayIcon			m_TrayIcon;
};

extern CChromeTrayIcon	g_ChromeTrayIcon;