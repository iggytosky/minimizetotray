#include "StdAfx.h"
#include "ChromeTrayIcon.h"

#include <psapi.h>
#include <tlhelp32.h>

static const TCHAR *ChromeWindowClass = _T("Chrome_WindowImpl_0");
static const TCHAR *ChromeWidgetClass = _T("Chrome_WidgetWin_0");

CChromeTrayIcon	g_ChromeTrayIcon;

CChromeTrayIcon::CChromeTrayIcon(void)
{
}

CChromeTrayIcon::~CChromeTrayIcon(void)
{
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BOOL CChromeTrayIcon::CreateTrayIcon(HINSTANCE hInstance)
{
	wstring strChromePath;

	m_hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHROME));

	if(Create(NULL) == NULL)
	{
		DebugLog(_T("Can't create dialog! GLE: %lu"), GetLastError());
		return FALSE;
	}

	DebugLog(_T("Dialog created, HWND: 0x%X"), m_hWnd);

	ShowWindow(SW_HIDE);

	m_TrayIcon.Create(m_hWnd, 1, m_hIcon, _T("Google Chrome"));

	RunMonitoring();

	return TRUE;
}

BOOL CChromeTrayIcon::DestroyTrayIcon()
{
	DestroyWindow();

	StopMonitoring();

	m_TrayIcon.Destroy();

	DestroyIcon(m_hIcon);

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LRESULT CChromeTrayIcon::OnTrayDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	size_t nWindows = m_HiddenWindows.size();

	for(size_t i = 0; i < nWindows; ++i)
	{
		ShowChromeWindow(m_HiddenWindows[i]);
	}

	m_HiddenWindows.clear();

	return 0;
}

LRESULT CChromeTrayIcon::OnTrayContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DebugLog(_T("OnTrayContextMenu"));

	CMenu popupMenu;
	popupMenu.CreatePopupMenu();

	size_t nWindows = m_HiddenWindows.size();

	TCHAR szWindowName[255] = {0};

	for(size_t i = 0; i < nWindows; ++i)
	{
		::GetWindowText(m_HiddenWindows[i], szWindowName, _countof(szWindowName));

		popupMenu.AppendMenu(MF_STRING, 100 + i, szWindowName);
	}

	POINT pt;
	GetCursorPos(&pt);	

	int nMenuItem = ::TrackPopupMenu(popupMenu, TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, m_hWnd, NULL);

	if(nMenuItem >= 100)
	{
		size_t nIndex = nMenuItem - 100;

		if(nIndex < m_HiddenWindows.size())
		{
			ShowChromeWindow(m_HiddenWindows[nIndex]);
			m_HiddenWindows.erase(m_HiddenWindows.begin() + nIndex);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void CChromeTrayIcon::ShowChromeWindow(HWND hWnd)
{
	::ShowWindow(hWnd, SW_SHOW);
	::ShowWindow(hWnd, SW_RESTORE);
}

void CChromeTrayIcon::HideChromeWindow(HWND hWnd)
{
	::ShowWindow(hWnd, SW_HIDE);

	m_HiddenWindows.push_back(hWnd);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szClassName[255] = {0};
	GetClassName(hWnd, szClassName, _countof(szClassName));

	if(_wcsicmp(szClassName, ChromeWindowClass) == 0 ||
	   _wcsicmp(szClassName, ChromeWidgetClass) == 0)
	{
		if(GetWindow(hWnd, GW_CHILD) != NULL)
		{
			if(IsWindowVisible(hWnd) && IsIconic(hWnd))
			{
				((CChromeTrayIcon*)lParam)->HideChromeWindow(hWnd);
			}
		}
	}

	return TRUE;
}

BOOL CChromeTrayIcon::Worker()
{
	EnumWindows(EnumWindowsProc, (LPARAM)this);

	return TRUE;
}