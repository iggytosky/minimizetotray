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
	::ZeroMemory(&m_nid, sizeof(m_nid));

	m_nid.cbSize			= sizeof(m_nid); 
	m_nid.uCallbackMessage	= ::RegisterWindowMessage(TEXT("TaskbarNotifyMsg"));

	m_nid.hWnd				= hWnd;
	m_nid.uID				= iID;
	m_nid.uFlags			= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_nid.hIcon				= hIcon;

	size_t nTipLen	= (_countof(m_nid.szTip) > wcslen(lpszTooltip)) ? wcslen(lpszTooltip) : _countof(m_nid.szTip);
	wcsncpy_s(m_nid.szTip, lpszTooltip, nTipLen);

	return Show();
}

BOOL CTrayIcon::Destroy()
{
	BOOL bResult = FALSE;

	if(IsVisible())
	{
		Hide(); 
	}
	
	m_nid.hWnd	= NULL;
	m_nid.hIcon = NULL;

	return TRUE;
}

BOOL CTrayIcon::SetIcon(HICON hIcon)
{
	BOOL bResult	= FALSE;

	m_nid.uFlags	= NIF_ICON; 
	m_nid.hIcon		= hIcon;
	bResult			= ::Shell_NotifyIcon(NIM_MODIFY, &m_nid); 

	return bResult;
}

BOOL CTrayIcon::SetTooltip(LPCTSTR lpszTooltip)
{
	BOOL bResult	= FALSE;

	m_nid.uFlags	= NIF_TIP; 

	size_t nTipLen	= (_countof(m_nid.szTip) > wcslen(lpszTooltip)) ? wcslen(lpszTooltip) : _countof(m_nid.szTip);
	wcsncpy_s(m_nid.szTip, lpszTooltip, nTipLen);

	bResult			= ::Shell_NotifyIcon(NIM_MODIFY, &m_nid); 

	return bResult;
}

BOOL CTrayIcon::Hide()
{
	if(IsVisible() == FALSE)
	{
		return FALSE;
	}

	NOTIFYICONDATA nid	= m_nid;
	nid.uFlags			= NIF_MESSAGE;

	if(Shell_NotifyIcon(NIM_DELETE, &m_nid))
	{
		m_bVisible = FALSE;
		return TRUE;
	}

	return FALSE;
}

BOOL CTrayIcon::Show()
{
	if(IsVisible())
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