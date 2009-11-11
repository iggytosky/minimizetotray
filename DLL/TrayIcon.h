#pragma once

#include <ShellAPI.h>

#define WM_TRAY_FIRST		WM_USER + 3

#define WM_TRAY_RCLICK		WM_TRAY_FIRST + 1
#define WM_TRAY_LCLICK		WM_TRAY_FIRST + 2
#define WM_TRAY_RDBLCLICK	WM_TRAY_FIRST + 3
#define WM_TRAY_LDBLCLICK	WM_TRAY_FIRST + 4

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
		TASKBAR_MESSAGE_HANDLER((*this), WM_RBUTTONDOWN, OnMouseCommand)
		TASKBAR_MESSAGE_HANDLER((*this), WM_RBUTTONDBLCLK, OnMouseCommand)
		TASKBAR_MESSAGE_HANDLER((*this), WM_LBUTTONDBLCLK, OnMouseCommand)
		TASKBAR_MESSAGE_HANDLER((*this), WM_LBUTTONDOWN, OnMouseCommand)
	END_MSG_MAP()

public:
	BOOL Create(HWND hWnd, UINT iID, HICON hIcon, LPCTSTR lpszTooltip);
	BOOL Destroy();
	BOOL SetIcon(HICON hIcon);
	BOOL SetTooltip(LPCTSTR lpszTooltip);

	BOOL HideIcon();
	BOOL ShowIcon();

	BOOL IsIconVisible()
	{
		return m_bVisible;
	}

	BOOL ShowBaloon(LPCTSTR lpszTitle, LPCTSTR lpszText);

protected:
	LRESULT OnMouseCommand(LPARAM /*uMsg*/, BOOL& bHandled);

protected:
	size_t MaxTextLengthToCopy(size_t nTextLength, size_t nMaxLength)
	{
		return (nMaxLength > nTextLength) ? nTextLength : nMaxLength;
	}

protected:
	BOOL			m_bVisible;

	NOTIFYICONDATA	m_nid;

	friend class CChromeTrayIcon;
};
