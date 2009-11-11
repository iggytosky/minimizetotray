#include "StdAfx.h"
#include "TrayIcon.h"

CTrayIcon::CTrayIcon(void) : m_bVisible(FALSE)
{
}

CTrayIcon::~CTrayIcon(void)
{
	Destroy();
}

BOOL CTrayIcon::Create(HWND hWnd, UINT iID, HICON hIcon, LPCTSTR lpszTooltip)
{
	::ZeroMemory(&m_nid, sizeof(NOTIFYICONDATA));

	m_nid.cbSize			= sizeof(NOTIFYICONDATA); 
	m_nid.uCallbackMessage	= ::RegisterWindowMessage(TEXT("TaskbarNotifyMsg"));

	m_nid.hWnd				= hWnd;
	m_nid.uID				= iID;
	m_nid.uFlags			= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_nid.hIcon				= hIcon;

	wcsncpy_s(m_nid.szTip, lpszTooltip, MaxTextLengthToCopy(wcslen(lpszTooltip), _countof(m_nid.szTip) - 1));

	return ShowIcon();
}

BOOL CTrayIcon::Destroy()
{
	BOOL bResult = FALSE;

	if(IsIconVisible())
	{
		HideIcon(); 
	}
	
	m_nid.hWnd	= NULL;
	m_nid.hIcon = NULL;

	return TRUE;
}

BOOL CTrayIcon::SetIcon(HICON hIcon)
{
	BOOL bResult	= FALSE;

	m_nid.hIcon		= hIcon;
	m_nid.uFlags	= NIF_ICON;
	bResult			= ::Shell_NotifyIcon(NIM_MODIFY, &m_nid); 

	return bResult;
}

BOOL CTrayIcon::SetTooltip(LPCTSTR lpszTooltip)
{
	BOOL bResult	= FALSE;

	m_nid.uFlags	= NIF_TIP;

	wcsncpy_s(m_nid.szTip, lpszTooltip, MaxTextLengthToCopy(wcslen(lpszTooltip), _countof(m_nid.szTip) - 1));

	bResult			= ::Shell_NotifyIcon(NIM_MODIFY, &m_nid); 

	return bResult;
}

BOOL CTrayIcon::HideIcon()
{
	if(IsIconVisible() == FALSE)
	{
		return FALSE;
	}

	if(Shell_NotifyIcon(NIM_DELETE, &m_nid))
	{
		m_bVisible = FALSE;
		return TRUE;
	}

	return FALSE;
}

BOOL CTrayIcon::ShowIcon()
{
	if(IsIconVisible())
	{
		return TRUE;
	}

	m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	if(Shell_NotifyIcon(NIM_ADD, &m_nid))
	{
		m_bVisible = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL CTrayIcon::ShowBaloon(LPCTSTR lpszTitle, LPCTSTR lpszText)
{	
	wcsncpy_s(m_nid.szInfoTitle, lpszTitle, MaxTextLengthToCopy(wcslen(lpszTitle), _countof(m_nid.szInfoTitle) - 1));
	wcsncpy_s(m_nid.szInfo, lpszText, MaxTextLengthToCopy(wcslen(lpszText), _countof(m_nid.szInfo) - 1));

	m_nid.uFlags			= NIF_INFO;
	m_nid.uTimeout			= 10 * 1000;
	m_nid.dwInfoFlags		= NIIF_INFO;

	BOOL bResult = Shell_NotifyIcon(NIM_MODIFY, &m_nid);

	m_nid.szInfo[0] = _T('\0');

	return bResult;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LRESULT CTrayIcon::OnMouseCommand(LPARAM uMsg, BOOL& bHandled)
{
	UINT uMouseMessage = 0;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			uMouseMessage = WM_TRAY_LCLICK;
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			uMouseMessage = WM_TRAY_LDBLCLICK;
		}
		break;

	case WM_RBUTTONDOWN:
		{
			uMouseMessage = WM_TRAY_RCLICK;
		}
		break;

	case WM_RBUTTONDBLCLK:
		{
			uMouseMessage = WM_TRAY_RDBLCLICK;
		}
		break;
	}

	::SendMessage(m_nid.hWnd, uMouseMessage, 0, 0);

	return 0;
}