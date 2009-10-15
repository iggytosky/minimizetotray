#include "StdAfx.h"
#include "TrayIcon.h"

CTrayIcon::CTrayIcon(void)
{
	Init();
}

CTrayIcon::~CTrayIcon(void)
{
	Destroy();
}

void CTrayIcon::Init()
{
	::ZeroMemory(&m_nid, sizeof(m_nid));

	m_nid.cbSize			= sizeof(m_nid); 
	m_nid.uCallbackMessage	= ::RegisterWindowMessage(TEXT("TrayNotifyMsg"));
	m_nTaskbarRestartMsg	= ::RegisterWindowMessage(TEXT("TrayCreated"));
}

BOOL CTrayIcon::AddIcon()
{
	return Shell_NotifyIcon(NIM_ADD, &m_nid);;
}

BOOL CTrayIcon::Create(HWND hWnd, UINT iID, HICON hIcon, LPCTSTR lpszTooltip)
{
	m_nid.hWnd		= hWnd;
	m_nid.uID		= iID;
	m_nid.uFlags	= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_nid.hIcon		= hIcon;

	size_t nTipLen	= (_countof(m_nid.szTip) > wcslen(lpszTooltip)) ? wcslen(lpszTooltip) : _countof(m_nid.szTip);
	wcsncpy_s(m_nid.szTip, lpszTooltip, nTipLen);

	return AddIcon();
}

BOOL CTrayIcon::Destroy()
{
	BOOL bResult = FALSE;

	bResult = ::Shell_NotifyIcon(NIM_DELETE, &m_nid); 
	
	if(m_nid.hIcon != NULL) 
	{
		::DestroyIcon(m_nid.hIcon);
	}

	m_nid.hIcon = NULL;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LRESULT CTrayIcon::OnContextMenu(LPARAM /*uMsg*/, BOOL& bHandled)
{
	::SendMessage(m_nid.hWnd, WM_TRAY_CONTEXT_MENU, 0, 0);

	return 0;
}

LRESULT CTrayIcon::OnDblClick(LPARAM /*uMsg*/, BOOL& bHandled)
{
	::SendMessage(m_nid.hWnd, WM_TRAY_DBLCLICK, 0, 0);

	return 0;
}

LRESULT CTrayIcon::OnClick(LPARAM /*uMsg*/, BOOL& bHandled)
{
	return 0;
}