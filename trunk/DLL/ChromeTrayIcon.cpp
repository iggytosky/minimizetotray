#include "StdAfx.h"
#include "ChromeTrayIcon.h"

#include "JSMethods.h"

#include "utils.h"

static const TCHAR *ChromeWindowClass	= _T("Chrome_WindowImpl_0");
static const TCHAR *ChromeWidgetClass	= _T("Chrome_WidgetWin_0");

static LPCTSTR ChromeWindowClasses[]	= {ChromeWidgetClass, ChromeWindowClass};

static const int ContexMenuItemTextMax	= 48;

CChromeTrayIcon::CChromeTrayIcon(void) : 
		m_hIcon(NULL), m_HotKeyId(0),
		m_bChromeIsHidded(FALSE)
{
	InitializeCriticalSection(&m_csWindowsList);
}

CChromeTrayIcon::~CChromeTrayIcon(void)
{
	DeleteCriticalSection(&m_csWindowsList);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BOOL CChromeTrayIcon::CreateTrayIcon(HINSTANCE hInstance)
{
	if(Create(NULL) == NULL)
	{
		DebugLog(_T("Failed to create the tray icon!\n GLE: %lu"), GetLastError());

		return FALSE;
	}

	ShowWindow(SW_HIDE);

	OptionsChanged();

	ReCreateTrayIcon();

	m_uTrayRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));;

	m_TrayPopup.Create();

	HICON hIcon = GetChromeWindowIcon();

	m_TrayPopup.SetIcon(hIcon);

	//RunMonitoring();
	StartMonitoring();

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
	UnregisterHotKeys();

	EnterCriticalSection(&m_csWindowsList);

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		if(::IsWindow(m_ChromeWindows[i].hWnd) == FALSE)
		{
			ShowChromeWindow(m_ChromeWindows[i].hWnd);
		}
	}

	m_ChromeWindows.clear();

	LeaveCriticalSection(&m_csWindowsList);

	if(IsWindow())
	{
		DestroyWindow();
	}

	StopMonitoring();

	m_TrayIcon.Destroy();

	m_TrayPopup.DestroyWindow();

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

	if(m_hIcon != NULL)
	{
		DestroyIcon(m_hIcon);
	}

	if(lpszIconPath == NULL || lpszIconPath[0] == '\0')
	{
		m_hIcon = GetChromeWindowIcon();
	}
	else
	{
		m_hIcon = (HICON)LoadImage(NULL, lpszIconPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

		if(m_hIcon == NULL)
		{
			m_hIcon = GetChromeWindowIcon();
		}
	}

	return m_TrayIcon.SetIcon(m_hIcon);
}

BOOL CChromeTrayIcon::PopupNotify(LPCTSTR lpszTitle, LPCTSTR lpszText)
{
	m_TrayPopup.PopupWindow(lpszTitle, lpszText);

	return TRUE;
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

LRESULT CChromeTrayIcon::OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_bChromeIsHidded)
	{
		m_bChromeIsHidded = FALSE;

		m_TrayIcon.ShowIcon();
	}
	else
	{
		m_bChromeIsHidded = TRUE;

		HWND hWnd;

		EnterCriticalSection(&m_csWindowsList);

		for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
		{
			hWnd = m_ChromeWindows[i].hWnd;

			if(::IsWindowVisible(hWnd))
			{
				HideChromeWindow(hWnd);
			}
		}

		LeaveCriticalSection(&m_csWindowsList);

		if(m_options.bBossHideTrayIcon)
		{
			m_TrayIcon.HideIcon();
		}
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

LRESULT CChromeTrayIcon::OnFavorites(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	map<DWORD, wstring>::iterator it = m_Favorites.find(wID);

	if(it == m_Favorites.end())
	{
		return 0;
	}

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

	CJSMethods::OpenUrl((*it).second);

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

	EnterCriticalSection(&m_csWindowsList);

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		hWnd = m_ChromeWindows[i].hWnd;

		if(::IsWindowVisible(hWnd) == FALSE)
		{
			::GetWindowText(hWnd, szTooltip, _countof(szTooltip));
			break;
		}
	}

	LeaveCriticalSection(&m_csWindowsList);

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

	EnterCriticalSection(&m_csWindowsList);

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

	LeaveCriticalSection(&m_csWindowsList);

	::GetWindowText(hWnd, szTooltip, _countof(szTooltip));
	m_TrayIcon.SetTooltip(szTooltip);

	::ShowWindow(hWnd, SW_HIDE);
}
/*
void CChromeTrayIcon::AddChromeWindow(int nWindowId)
{
	DebugLog(_T("AddChromeWindow id=%d"), nWindowId);

	HWND hChromeWindow = FindVisibleChromeWindow();

	DebugLog(_T("AddChromeWindow hChromeWindow == %X"), hChromeWindow);

	if(hChromeWindow == NULL)
	{
		return;
	}

	EnterCriticalSection(&m_csWindowsList);

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		if(m_ChromeWindows[i].hWnd == hChromeWindow)
		{
			if(m_ChromeWindows[i].nId == nWindowId)
			{
				LeaveCriticalSection(&m_csWindowsList);

				DebugLog(_T("AddChromeWindow window already exist"), hChromeWindow);

				return;
			}

			m_ChromeWindows.erase(m_ChromeWindows.begin() + i);

			if(i != 0)
			{
				--i;
			}
		}
	}

	ChromeWindow chromeWnd = {nWindowId, hChromeWindow};
	m_ChromeWindows.push_back(chromeWnd);

	LeaveCriticalSection(&m_csWindowsList);
}

void CChromeTrayIcon::RemoveChromeWindow(int nWindowId)
{
	DebugLog(_T("RemoveChromeWindow id=%d m_ChromeWindows.size=%d"), nWindowId, m_ChromeWindows.size());

	EnterCriticalSection(&m_csWindowsList);

	for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
	{
		if(m_ChromeWindows[i].nId == nWindowId)
		{
			m_ChromeWindows.erase(m_ChromeWindows.begin() + i);
			break;
		}
	}

	DebugLog(_T("RemoveChromeWindow new size=%d"),  m_ChromeWindows.size());

	LeaveCriticalSection(&m_csWindowsList);
}

void CChromeTrayIcon::ChromeWindowFocusChanged()
{
}
*/

BOOL CChromeTrayIcon::OptionsChanged()
{
	if(CJSMethods::GetOptions(m_options) == false)
	{
		return FALSE;
	}

	if(m_options.bHideTray == FALSE)
	{
		if(m_TrayIcon.IsIconVisible() == FALSE)
		{
			m_TrayIcon.ShowIcon();
		}
	}

	UnregisterHotKeys();

	if(m_options.bEnableBossKey)
	{
		RegisterHotKeys();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BOOL CChromeTrayIcon::StartMonitoring()
{
	m_mouseMonitor.SetCallBack(this);
	m_mouseMonitor.Start();

	return TRUE;
}

BOOL CChromeTrayIcon::StopMonitoring()
{
	m_mouseMonitor.Stop();

	return TRUE;
}

BOOL CChromeTrayIcon::OnMouseButtonUp(const HWND &targetWindow, const DWORD &mouseButton, const POINT &coord)
{
	TCHAR windowClass[255] = {0};

	if(GetClassName(targetWindow, windowClass, _countof(windowClass)) == 0)
	{
		return FALSE;
	}

	BOOL isChromeWindow = FALSE;

	for(size_t i = 0; i < _countof(ChromeWindowClasses); ++i)
	{
		if(_wcsicmp(ChromeWindowClasses[i], windowClass) == 0)
		{
			isChromeWindow = TRUE;

			break;
		}
	}

	if(isChromeWindow == FALSE)
	{
		DebugLog(_T("This is not Chrome window. Window class: %s\n"), windowClass);

		return FALSE;
	}

	switch(mouseButton)
	{
	case VK_LBUTTON:
		{
			if(m_options.bMinimizeOnLeftButton)
			{
				HideChromeWindow(targetWindow);
			}
		}
		break;

	case VK_RBUTTON:
		{
			if(m_options.bMinimizeOnRightButton)
			{
				HideChromeWindow(targetWindow);
			}
		}
		break;
	}
	/*
	HWND hChromeWnd = WindowFromPoint(coord);

	if(hChromeWnd == NULL)
	{
		return FALSE;
	}

	TCHAR windowClass[255] = {0};

	if(GetClassName(hChromeWnd, windowClass, _countof(windowClass)) == 0)
	{
		return FALSE;
	}

	BOOL isChromeWindow = FALSE;

	for(size_t i = 0; i < _countof(ChromeWindowClasses); ++i)
	{
		if(_wcsicmp(ChromeWindowClasses[i], windowClass) == 0)
		{
			isChromeWindow = TRUE;

			break;
		}
	}

	if(isChromeWindow == FALSE)
	{
		DebugLog(_T("This is not Chrome window. Window class: %s\n"), windowClass);

		return FALSE;
	}

	LRESULT hitTestResult = ::SendMessage(hChromeWnd, WM_NCHITTEST, 0, MAKELPARAM(coord.x, coord.y));

	DebugLog(_T("hitTestResult: %lu\n"), hitTestResult);

	if(mouseButton == VK_LBUTTON)
	{
		//if(hitTestResult == HTNOWHERE)
		{
			if(::IsWindowVisible(hChromeWnd) && ::IsIconic(hChromeWnd))
			{
				HideChromeWindow(hChromeWnd);
				//ATLTRACE(_T("Chrome is minimized!\n"));
			}
		}
	}
	else if(mouseButton == VK_RBUTTON)
	{
		if(hitTestResult == HTMINBUTTON)
		{
			//ATLTRACE(_T("Chrome is minimized!\n"));
			//::SendMessage(hActiveWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
	}
	*/

	return TRUE;
}

/*
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

	if(m_options.bHideTray && m_bChromeIsHidded == FALSE)
	{
		size_t nHiddenWindows = 0;

		EnterCriticalSection(&m_csWindowsList);

		for(size_t i = 0; i < m_ChromeWindows.size(); ++i)
		{
			if(::IsWindowVisible(m_ChromeWindows[i].hWnd) == FALSE)
			{
				++nHiddenWindows;
			}
		}

		LeaveCriticalSection(&m_csWindowsList);

		if(nHiddenWindows == 0 && m_TrayIcon.IsIconVisible())
		{
			DebugLog(_T("m_ChromeWindows.size=%d"), m_ChromeWindows.size());

			m_TrayIcon.HideIcon();
		}
		else if(nHiddenWindows != 0 && m_TrayIcon.IsIconVisible() == FALSE)
		{
			m_TrayIcon.ShowIcon();
		}
	}

	return TRUE;
}
*/
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void CChromeTrayIcon::RestoreAllChromeWindows()
{
	HWND hWnd;

	size_t nRestoredWindows = 0;

	EnterCriticalSection(&m_csWindowsList);

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

	LeaveCriticalSection(&m_csWindowsList);

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
	
	if(m_options.bShowOptions)
	{
		m_TrayMenu.AppendMenu(MF_STRING, TRAY_OPTIONS_COMMAND, m_language.strOptions.c_str());
	}

	if(m_options.bShowOptions && (m_options.bShowNewWindow || m_options.bShowNewTab))
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

	if(m_options.bShowFavorites && m_options.strFavorites.empty() == FALSE)
	{
		if(m_options.bShowOptions || m_options.bShowNewWindow || m_options.bShowNewTab)
		{
			m_TrayMenu.AppendMenu(MF_SEPARATOR, TRAY_OPTIONS_COMMAND, _T(""));
		}

		m_Favorites.clear();

		vector<wstring> favorites;
		vector<wstring> favoriteItem;

		CMenu favoritesMenu;
		favoritesMenu.CreatePopupMenu();

		DWORD dwCmdId;

		if(SplitString(m_options.strFavorites, favorites, _T("\n")))
		{
			for(size_t i = 0; i < favorites.size(); ++i)
			{
				if(SplitString(favorites[i], favoriteItem, _T(" http")))
				{
					if(favoriteItem.size() == 2)
					{
						dwCmdId = TRAY_FAVORITES_COMMAND + m_Favorites.size();

						favoritesMenu.AppendMenu(MF_STRING, dwCmdId, favoriteItem[0].c_str());
						m_Favorites[dwCmdId] = favoriteItem[1].c_str();
					}
				}
			}
		}

		m_TrayMenu.AppendMenu(MF_POPUP | MF_STRING, (UINT_PTR)favoritesMenu.m_hMenu, m_language.strFavorites.c_str());
	}

	BOOL	bNeedToAddSeparator	= TRUE;
	vector<ChromeTab> tabs;
	HWND	hWnd				= NULL;
	HWND	hChildWindow		= NULL;

	EnterCriticalSection(&m_csWindowsList);

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

	LeaveCriticalSection(&m_csWindowsList);

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
		hIcon = (HICON)CopyImage(hIcon, IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);

		return hIcon;
	}
	else if (hIcon = (HICON)GetClassLong(hChromeWnd, GCL_HICON))
	{
		hIcon = (HICON)CopyImage(hIcon, IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);

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

	return NULL;
}

BOOL CChromeTrayIcon::RegisterHotKeys()
{
	BOOL bResult;

	if(m_HotKeyId == 0)
	{
		m_HotKeyId = GlobalAddAtom(_T("chromeTrayHotKey"));
	}
	else
	{
		UnregisterHotKey(m_hWnd, m_HotKeyId);
	}

	bResult = RegisterHotKey(m_hWnd, m_HotKeyId, m_options.wBossModifier, m_options.wBossKey);

	return bResult;
}

void CChromeTrayIcon::UnregisterHotKeys()
{
	if(m_HotKeyId != 0)
	{
		UnregisterHotKey(m_hWnd, (int)m_HotKeyId);
		GlobalDeleteAtom(m_HotKeyId);
		m_HotKeyId = 0;
	}
}