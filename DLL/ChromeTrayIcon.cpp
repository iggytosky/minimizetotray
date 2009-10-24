#include "StdAfx.h"
#include "ChromeTrayIcon.h"

#include "JSMethods.h"

#include <psapi.h>
#include <tlhelp32.h>

static const TCHAR *ChromeWindowClass	= _T("Chrome_WindowImpl_0");
static const TCHAR *ChromeWidgetClass	= _T("Chrome_WidgetWin_0");
static LPCTSTR ChromeWindowClasses[]	= {ChromeWidgetClass, ChromeWindowClass};

static const int ContexMenuItemTextMax	= 48;

CChromeTrayIcon::CChromeTrayIcon(void) : m_hIcon(NULL)
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
	OptionsChanged();

	if(Create(NULL) == NULL)
	{
		return FALSE;
	}

	ShowWindow(SW_HIDE);

	ReCreateTrayIcon();

	m_uTrayRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));;

	RunMonitoring();

	return TRUE;
}

BOOL CChromeTrayIcon::ReCreateTrayIcon()
{
	m_TrayIcon.Destroy();

	HICON hIcon = GetChromeWindowIcon();

	return m_TrayIcon.Create(m_hWnd, 1, hIcon, _T("Google Chrome"));
}

BOOL CChromeTrayIcon::DestroyTrayIcon()
{
	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		if(::IsWindow(m_ChromeWindows[i].hWnd) == FALSE)
		{
			ShowChromeWindow(m_ChromeWindows[i].hWnd);
		}
	}

	if(IsWindow())
	{
		DestroyWindow();
	}

	StopMonitoring();

	m_TrayIcon.Destroy();

	if(m_hIcon != NULL)
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}

	return TRUE;
}

BOOL CChromeTrayIcon::SetTrayIcon(LPCTSTR lpszIconPath)
{	
	BOOL bResult	= FALSE;

	HICON hIcon		= NULL;

	if(lpszIconPath == NULL || lpszIconPath[0] == '\0')
	{
		hIcon = GetChromeWindowIcon();
	}
	else
	{
		hIcon = (HICON)LoadImage(NULL, lpszIconPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

		if(hIcon == NULL)
		{
			DebugLog(_T("LoadImage failed!, path: %s gle: %lu"), lpszIconPath, GetLastError());

			return FALSE;
		}

		if(m_hIcon != NULL)
		{
			DestroyIcon(m_hIcon);
			m_hIcon = hIcon;
		}
	}

	return m_TrayIcon.SetIcon(hIcon);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
LRESULT CChromeTrayIcon::OnTrayMouseCommand(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	TrayAction action = Nothing;

	switch(uMsg)
	{
	case WM_TRAY_LCLICK:
		{
			action = m_options.actLClick;
		}
		break;

	case WM_TRAY_LDBLCLICK:
		{
			action = m_options.actLDblClick;
		}
		break;

	case WM_TRAY_RCLICK:
		{
			action = m_options.actRClick;
		}
		break;

	case WM_TRAY_RDBLCLICK:
		{
			action = m_options.actRDblClick;
		}
		break;
	}

	switch(action)
	{
	case Nothing:
		{

		}
		break;

	case Restore:
		{
			RestoreAllChromeWindows();
		}
		break;

	case ContextMenu:
		{
			ShowContextMenu();
		}
		break;

	case NewTab:
		{
			BOOL bDummy;
			OnNewTab(0, 0, 0, bDummy);
		}
		break;

	case NewWindow:
		{
			BOOL bDummy;
			OnNewWnd(0, 0, 0, bDummy);
		}
		break;
	}

	return 0;
}

LRESULT CChromeTrayIcon::OnOptions(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HWND hChromeWindow = FindVisibleChromeWindow();

	if(hChromeWindow == NULL)
	{
		if(m_ChromeWindows.size() != 0)
		{
			hChromeWindow = FindVisibleChromeWindow();
		}
	}

	if(hChromeWindow != NULL)
	{
		ShowChromeWindow(hChromeWindow);
	}

	CJSMethods::ShowOptions();

	return 0;
}

LRESULT CChromeTrayIcon::OnNewTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HWND hChromeWindow = FindVisibleChromeWindow();

	if(hChromeWindow == NULL)
	{
		if(m_ChromeWindows.size() != 0)
		{
			hChromeWindow = FindVisibleChromeWindow();
		}
	}

	if(hChromeWindow != NULL)
	{
		ShowChromeWindow(hChromeWindow);
	}

	CJSMethods::NewTab();

	return 0;
}

LRESULT CChromeTrayIcon::OnNewWnd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CJSMethods::NewWindow();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void CChromeTrayIcon::ShowChromeWindow(HWND hWnd)
{
	if(::IsWindowVisible(hWnd) == FALSE)
	{
		::ShowWindow(hWnd, SW_SHOW);
	}

	if(::IsIconic(hWnd))
	{
		::ShowWindow(hWnd, SW_RESTORE);
	}

	::SetForegroundWindow(hWnd);
	::SetActiveWindow(hWnd);

	TCHAR szTooltip[255] = {0};

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		hWnd = m_ChromeWindows[i].hWnd;

		if(::IsWindowVisible(hWnd) == FALSE)
		{
			::GetWindowText(hWnd, szTooltip, _countof(szTooltip));
			break;
		}
	}

	if(wcslen(szTooltip) == 0)
	{
		wcscpy_s(szTooltip, _T("Google Chrome"));
	}

	m_TrayIcon.SetTooltip(szTooltip);
}

void CChromeTrayIcon::HideChromeWindow(HWND hWnd)
{
	BOOL	bFound			= FALSE;

	TCHAR	szTooltip[255]	= {0};

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		if(hWnd == m_ChromeWindows[i].hWnd)
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound == FALSE)
	{
		ChromeWindow newWnd = {-1, hWnd};
		m_ChromeWindows.push_back(newWnd);
	}

	::GetWindowText(hWnd, szTooltip, _countof(szTooltip));
	m_TrayIcon.SetTooltip(szTooltip);

	::ShowWindow(hWnd, SW_HIDE);
}

void CChromeTrayIcon::AddChromeWindow(int nWindowId)
{
	HWND hChromeWindow = FindVisibleChromeWindow();

	if(hChromeWindow == NULL)
	{
		return;
	}

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		if(m_ChromeWindows[i].hWnd == hChromeWindow)
		{
			m_ChromeWindows.erase(m_ChromeWindows.begin() + i);

			if(i != 0)
			{
				--i;
			}
		}
	}

	ChromeWindow chromeWnd = {nWindowId, hChromeWindow};
	m_ChromeWindows.push_back(chromeWnd);
}

void CChromeTrayIcon::RemoveChromeWindow(int nWindowId)
{
	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		if(m_ChromeWindows[i].nId == nWindowId)
		{
			m_ChromeWindows.erase(m_ChromeWindows.begin() + i);
			break;
		}
	}
}

void CChromeTrayIcon::ChromeWindowFocusChanged()
{
}

BOOL CChromeTrayIcon::OptionsChanged()
{
	if(CJSMethods::GetOptions(m_options) == false)
	{
		return FALSE;
	}

	if(m_options.bHideTray == FALSE)
	{
		if(m_TrayIcon.IsVisible() == FALSE)
		{
			m_TrayIcon.Show();
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

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

	if(m_options.bHideTray)
	{
		size_t nHiddenWindows = 0;

		for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
		{
			if(::IsWindowVisible(m_ChromeWindows[i].hWnd) == FALSE)
			{
				++nHiddenWindows;
			}
		}

		if(nHiddenWindows == 0 && m_TrayIcon.IsVisible())
		{
			m_TrayIcon.Hide();
		}
		else if(nHiddenWindows != 0 && m_TrayIcon.IsVisible() == FALSE)
		{
			m_TrayIcon.Show();
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void CChromeTrayIcon::RestoreAllChromeWindows()
{
	HWND hWnd;

	size_t nRestoredWindows = 0;

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		hWnd = m_ChromeWindows[i].hWnd;

		if(::IsWindow(hWnd) == FALSE)
		{
			m_ChromeWindows.erase(m_ChromeWindows.begin() + i);

			if(i != 0)
			{
				--i;
			}

			continue;
		}

		if(::IsWindowVisible(hWnd) == FALSE)
		{
			ShowChromeWindow(hWnd);
			++nRestoredWindows;
		}
	}

	if(nRestoredWindows == 0 && m_ChromeWindows.size() != 0)
	{
		HWND hWnd = FindVisibleChromeWindow();

		if(hWnd != NULL)
		{
			ShowChromeWindow(hWnd);
		}
	}
}

void CChromeTrayIcon::ShowContextMenu()
{
	CJSMethods::GetLanguage(m_language);

	TCHAR szWindowName[255] = {0};

	if(m_TrayMenu.m_hMenu != NULL)
	{
		m_TrayMenu.DestroyMenu();
	}

	m_TrayMenu.CreatePopupMenu();
	m_TrayMenu.AppendMenu(MF_STRING, TRAY_OPTIONS_COMMAND, m_language.strOptions.c_str());

	if(m_options.bShowNewWindow || m_options.bShowNewTab)
	{
		m_TrayMenu.AppendMenu(MF_SEPARATOR, TRAY_OPTIONS_COMMAND, _T(""));
	}

	if(m_options.bShowNewWindow)
	{
		m_TrayMenu.AppendMenu(MF_STRING, TRAY_NEW_WND_COMMAND, m_language.strNewWindow.c_str());
	}

	if(m_options.bShowNewTab)
	{
		m_TrayMenu.AppendMenu(MF_STRING, TRAY_NEW_TAB_COMMAND, m_language.strNewTab.c_str());
	}

	BOOL	bNeedToAddSeparator	= TRUE;
	vector<ChromeTab> tabs;
	HWND	hWnd				= NULL;
	HWND	hChildWindow		= NULL;

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		hWnd = m_ChromeWindows[i].hWnd;

		if(::IsWindow(hWnd) == FALSE)
		{
			m_ChromeWindows.erase(m_ChromeWindows.begin() + i);

			if(i != 0)
			{
				--i;
			}
			continue;
		}

		if(::IsWindowVisible(hWnd) == FALSE)
		{
			hChildWindow = NULL;

			for(size_t j = 0; j < _countof(ChromeWindowClasses); ++j)
			{
				hChildWindow = FindWindowEx(hWnd, NULL, ChromeWindowClasses[j], NULL);

				if(hChildWindow != NULL)
				{
					::GetWindowText(hChildWindow, szWindowName, _countof(szWindowName));
					break;
				}
			}

			if(hChildWindow == NULL)
			{
				::GetWindowText(hWnd, szWindowName, _countof(szWindowName));
			}

			if(wcslen(szWindowName) > ContexMenuItemTextMax)
			{
				szWindowName[ContexMenuItemTextMax] = '\0';
				wcscat_s(szWindowName, _T("..."));
			}

			tabs.clear();
			//CJSMethods::GetWindowTabs(m_ChromeWindows[i].nId, tabs);
			
			if(tabs.size() == 0)
			{
				if(bNeedToAddSeparator)
				{
					m_TrayMenu.AppendMenu(MF_SEPARATOR, TRAY_OPTIONS_COMMAND, _T(""));
					bNeedToAddSeparator = FALSE;
				}

				m_TrayMenu.AppendMenu(MF_STRING, TRAY_MENU_COMMAND + 100 * i, szWindowName);
			}
			else
			{
				ChromeTab tab;

				CMenu subMenu;
				subMenu.CreatePopupMenu();

				for(size_t k = 0; k < tabs.size(); ++k)
				{
					tab = tabs[k];

					DebugLog(_T("Tab title: %s"), tab.strTitle.c_str());

					if(tab.strTitle.size() > ContexMenuItemTextMax)
					{
						tab.strTitle = tab.strTitle.substr(ContexMenuItemTextMax);
						tab.strTitle += _T("...");
					}

					subMenu.AppendMenu(MF_STRING, TRAY_MENU_COMMAND + 100 * i + tab.nId, tab.strTitle.c_str());
				}

				m_TrayMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT_PTR)subMenu.m_hMenu, szWindowName);
			}
		}
	}

	if(m_TrayMenu.GetMenuItemCount() == 0)
	{
		return;
	}

	POINT pt;
	GetCursorPos(&pt);	

	SetForegroundWindow(m_hWnd);

	int nMenuItem = ::TrackPopupMenu(m_TrayMenu, TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, m_hWnd, NULL);

	PostMessage(WM_NULL, 0, 0);

	if(nMenuItem >= TRAY_MENU_COMMAND)
	{
		int nIndex = nMenuItem - TRAY_MENU_COMMAND;

		int nWindow = (int)(nIndex / 100);

		//int nTab = (nIndex % 100);

		//CJSMethods::SelectWindowTab(nTab);

		ShowChromeWindow(m_ChromeWindows[nWindow].hWnd);
	}
	else
	{
		::SendMessage(m_hWnd, WM_COMMAND, nMenuItem, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

HICON CChromeTrayIcon::GetChromeWindowIcon()
{
	HICON hIcon = NULL;

	HWND hChromeWnd = FindWindow(ChromeWindowClass, NULL);

	if(hChromeWnd == NULL)
	{
		hChromeWnd = FindWindow(ChromeWidgetClass, NULL);
	}

	if(hChromeWnd == NULL)
	{
		return hIcon;
	}

	if(hIcon = (HICON)SendMessage(hChromeWnd, WM_GETICON, ICON_SMALL, 0))
	{
		return hIcon;
	}
	else if (hIcon = (HICON)GetClassLong(hChromeWnd, GCL_HICON))
	{
		return hIcon;
	}

	return hIcon;
}

HWND CChromeTrayIcon::FindVisibleChromeWindow()
{
	HWND hChromeWindow = NULL;

	for(size_t i = 0; i < _countof(ChromeWindowClasses); ++i)
	{
		hChromeWindow = FindWindowEx(NULL, NULL, ChromeWindowClasses[i], NULL);

		while(hChromeWindow != NULL && FindWindowEx(hChromeWindow, NULL, ChromeWindowClasses[i], NULL) == NULL)
		{
			hChromeWindow = FindWindowEx(NULL, hChromeWindow, ChromeWindowClasses[i], NULL);
		}

		if(hChromeWindow != NULL)
		{
			return hChromeWindow;
		}
	}

	/*
	HWND hChildWindow = NULL;

	for(size_t j = 0; j < _countof(ChromeWindowClasses); ++j)
	{
		hChildWindow = FindWindowEx(hWnd, NULL, ChromeWindowClasses[j], NULL);

		if(hChildWindow != NULL)
		{
			return hWnd;
		}
	}
	*/

	return NULL;
}