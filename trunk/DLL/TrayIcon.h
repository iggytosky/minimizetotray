#pragma once

#include <ShellAPI.h>

#define WM_TRAY_FIRST				WM_USER + 3

#define WM_TRAY_CONTEXT_MENU		WM_TRAY_FIRST + 1
#define WM_TRAY_DBLCLICK			WM_TRAY_FIRST + 2

#define TASKBAR_MESSAGE_HANDLER(ti, msg, func) \
	if(uMsg==ti.m_nid.uCallbackMessage && wParam==ti.m_nid.uID && lParam==msg) \
	{ \
		bHandled = TRUE; \
		lResult = func(lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

class CTrayIcon
{
public:
	CTrayIcon(void);
	~CTrayIcon(void);

	BEGIN_MSG_MAP(CTrayIcon)
		TASKBAR_MESSAGE_HANDLER((*this), WM_RBUTTONDOWN, OnContextMenu)
		TASKBAR_MESSAGE_HANDLER((*this), WM_LBUTTONDBLCLK, OnDblClick)
		TASKBAR_MESSAGE_HANDLER((*this), WM_LBUTTONDOWN, OnClick)
	END_MSG_MAP()

public:
	BOOL Create(HWND hWnd, UINT iID, HICON hIcon, LPCTSTR lpszTooltip);
	BOOL Destroy();

protected:
	LRESULT OnContextMenu(LPARAM /*uMsg*/, BOOL& bHandled);
	LRESULT OnDblClick(LPARAM /*uMsg*/, BOOL& bHandled);
	LRESULT OnClick(LPARAM /*uMsg*/, BOOL& bHandled);

protected:
	void Init();

	BOOL AddIcon();

protected:
	NOTIFYICONDATA	m_nid;
	UINT			m_nTaskbarRestartMsg;

	friend class CChromeTrayIcon;
};
