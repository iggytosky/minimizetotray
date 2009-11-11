#pragma once

typedef CWinTraits<WS_POPUPWINDOW, WS_EX_TOOLWINDOW> CPopupDialogTraits;

template <class T>
class CPopupDialog : public CWindowImpl<CPopupDialog<T>, CWindow, CPopupDialogTraits>
{
public:
	DECLARE_WND_CLASS(_T("PopupDialog Window Class"))

	CPopupDialog()
	{
		m_hParentWnd	= NULL;
		m_hIcon			= NULL;
	}

	virtual ~CPopupDialog()
	{
		if(m_hWnd != NULL && IsWindow())
		{
			DestroyWindow();
		}
	}

	BEGIN_MSG_MAP(CPopupDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PostQuitMessage(0);

		return 0;
	}

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam != NULL)
		{
			CDCHandle dc((HDC)wParam);
			DrawClientArea(dc);
		}

		return 1;
	}


public:
	void HideWindow()
	{
		if(!IsWindowVisible())
		{
			return;
		}

		::ShowWindow(m_hWnd, SW_HIDE);
	}

	void ShowWindow(HWND hWndParent)
	{
		if(IsWindowVisible())
		{
			return;
		}

		if(IsFullScreenAppRunning())
		{
			return;
		}

		PlaceWindow();
		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}

	void SetTitle(LPCTSTR lpszTitle)
	{
		if(m_strTitle != lpszTitle)
		{
			m_strTitle = lpszTitle;

			if(IsWindow())
			{
				Invalidate();
			}
		}
	}

	void SetText(LPCTSTR lpszText)
	{
		if(m_strText != lpszText)
		{
			m_strText = lpszText;

			if(IsWindow())
			{
				Invalidate();
			}
		}
	}

	void SetIcon(HICON hIcon)
	{
		m_hIcon = hIcon;

		if(m_hIcon != NULL)
		{
			::GetIconInfo(m_hIcon, &m_icoInfo);
		}
	}

	virtual BOOL Create()
	{
		CWindowImpl<CPopupDialog, CWindow, CPopupDialogTraits>::Create(NULL, CWindow::rcDefault, NULL);
		
		ModifyStyle(WS_BORDER, 0, 0);

		_Init();

		return TRUE;
	}

public:
	void _Init()
	{
		T* pT = static_cast<T*>(this);
		pT->_Init();
	}

protected:
	virtual BOOL CalculateClientRect(RECT &rcClient) = 0;
	virtual void DrawClientArea(CDCHandle dc) = 0;

protected:
	BOOL IsFullScreenAppRunning()
	{
		HWND hWnd	= ::GetForegroundWindow();
		RECT rc		= {0};

		::GetWindowRect(hWnd, &rc);

		if(rc.right - rc.left == ::GetSystemMetrics(SM_CXFULLSCREEN) &&  rc.bottom - rc.top == ::GetSystemMetrics(SM_CYFULLSCREEN)) 
		{
			return TRUE;
		}

		return FALSE;
	}

	void PlaceWindow()
	{
		RECT rcTip;
		CalculateClientRect(rcTip);

		RECT rectDesktopWithoutTaskbar;
		RECT rcWnd;
		RECT rcNewRect;

		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktopWithoutTaskbar, 0);
		::GetWindowRect(m_hWnd, &rcWnd);
		rcNewRect = rcWnd;

		rcNewRect.left   = rectDesktopWithoutTaskbar.right - (rcTip.right) - 3;
		rcNewRect.top    = rectDesktopWithoutTaskbar.bottom - (rcTip.bottom) - 3;
		rcNewRect.bottom = rcNewRect.top + rcTip.bottom;
		rcNewRect.right  = rcNewRect.left + (rcTip.right);

		SetWindowPos(NULL, &rcNewRect, SWP_NOZORDER|SWP_NOACTIVATE);
	}

protected:
	wstring				m_strTitle;
	wstring				m_strText;

	HICON				m_hIcon;
	ICONINFO			m_icoInfo;

	CFont				m_fntTitle;
	CFont				m_fntText;

	COLORREF			m_clrText;
	COLORREF			m_clrBack;
	COLORREF			m_clrBorder;
	COLORREF			m_clrTopLine;

	HWND				m_hParentWnd;
};
