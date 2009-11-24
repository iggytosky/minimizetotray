#include "StdAfx.h"
#include "resource.h"
#include "TrayPopup.h"

#define TIMER_HIDE_DELAY_VALUE (10 * 1000)

CTrayPopup::CTrayPopup(void)
{
	m_bCloseBoxHighlite				= FALSE;
	m_pSetLayeredWindowAttributes	= NULL;
}

CTrayPopup::~CTrayPopup(void)
{
}

void CTrayPopup::_Init()
{
	//////////////////////////////////////////////////////////////////////////
	// Transparency
	//////////////////////////////////////////////////////////////////////////

	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	m_pSetLayeredWindowAttributes = (procSetLayeredWindowAttributes)GetProcAddress(hUser32, "SetLayeredWindowAttributes");

	if(HasTransSupport())
	{
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	}

	//////////////////////////////////////////////////////////////////////////
	// Fonts
	//////////////////////////////////////////////////////////////////////////

	NONCLIENTMETRICS ncm	= { 0 };
	ncm.cbSize				= sizeof(ncm);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

	if(m_fntText.IsNull() == FALSE)
	{
		m_fntText.DeleteObject();
	}

	ncm.lfMenuFont.lfHeight = 15;
	m_fntText.CreateFontIndirect(&ncm.lfMenuFont);

	if(m_fntMarlett.IsNull() == FALSE)
	{
		m_fntMarlett.DeleteObject();
	}

	m_fntMarlett.CreateFont(10, 0, 0, 0, FW_NORMAL, 0, 0, 0, SYMBOL_CHARSET, 0, 0, 0, 0, _T("marlett"));

	//////////////////////////////////////////////////////////////////////////
	// Colors
	//////////////////////////////////////////////////////////////////////////

	m_crBorder	= RGB(58, 117, 189);
	m_crTop		= RGB(82, 150, 222);
	m_crBottom	= RGB(212, 229, 247);
}

void CTrayPopup::PopupWindow(LPCTSTR lpszTitle, LPCTSTR lpszText)
{
	if(IsWindowVisible())
	{
		SetText(lpszText);

		PlaceWindow();
	}
	else
	{
		GetCursorPos(&m_ptInitialCurcorPos);

		SetText(lpszText);

		if(HasTransSupport())
		{
			m_cbFade = 100;
			m_pSetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 255), m_cbFade, LWA_ALPHA);
			SetTimer(TIMER_SHOW_WINDOW, 20);
		}

		ShowWindow(NULL);

		SetTimer(TIMER_HIDE_DELAY, 10 * 1000);
	}
}

void CTrayPopup::PopdownWindow()
{
	m_bCloseBoxHighlite = FALSE;

	if(IsWindowVisible())
	{
		if(HasTransSupport())
		{
			m_cbFade = 230;
			SetTimer(TIMER_HIDE_WINDOW, 20);
		}
		else
		{
			HideWindow();
		}
	}
}

BOOL CTrayPopup::CalculateClientRect(RECT &rcClient)
{
	rcClient.left	= 0;
	rcClient.right	= 280;

	CDC dc = GetDC();

	rcClient.top	= 0;
	rcClient.bottom	= 0;

	RECT rcText		= rcClient;
	rcText.bottom	= 500;
	InflateRect(&rcText, -4, -4);

	dc.SelectFont(m_fntText);
	dc.DrawText(m_strText.c_str(), -1, &rcText, DT_WORDBREAK | DT_CALCRECT);

	rcClient.bottom	+= (rcText.bottom - rcText.top);
	rcClient.bottom += 8;

	rcClient.bottom += 16;


	//////////////////////////////////////////////////////////////////////////
	// Title
	//////////////////////////////////////////////////////////////////////////
	m_rcTitle			= rcClient;
	m_rcTitle.bottom	= m_rcTitle.top + 16;
	InflateRect(&m_rcTitle, -2, -2);

	//////////////////////////////////////////////////////////////////////////
	// Close button
	//////////////////////////////////////////////////////////////////////////
	dc.SelectFont(m_fntMarlett);
	dc.DrawText(_T("r"), -1, &m_rcCloseBox, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_CALCRECT);

	int nWidth = (m_rcCloseBox.right - m_rcCloseBox.left);
	int nHeight = (m_rcCloseBox.bottom - m_rcCloseBox.top);

	m_rcCloseBox.right	= m_rcTitle.right - 4;
	m_rcCloseBox.left	= m_rcTitle.right - nWidth;

	m_rcCloseBox.top	=  m_rcTitle.top + ((m_rcTitle.bottom - m_rcTitle.top) - nHeight) / 2;
	m_rcCloseBox.bottom = m_rcCloseBox.top + nHeight;

	/*
	RECT rcText = rcClient;
	InflateRect(&rcText, -4, -4);

	dc.SelectFont(m_fntText);
	dc.DrawText(m_strText.c_str(), -1, &rcText, DT_WORDBREAK | DT_CALCRECT);

	rcClient.top	= 0;
	rcClient.bottom = (rcText.bottom - rcText.top);

	m_rcTitle = rcClient;
	m_rcTitle.bottom = m_rcTitle.top + 16;
	InflateRect(&m_rcTitle, -1, -1);

	dc.SelectFont(m_fntMarlett);
	dc.DrawText(_T("r"), -1, &m_rcCloseBox, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_CALCRECT);
	
	int nWidth = (m_rcCloseBox.right - m_rcCloseBox.left);
	int nHeight = (m_rcCloseBox.bottom - m_rcCloseBox.top);

	m_rcCloseBox.right	= m_rcTitle.right - 4;
	m_rcCloseBox.left	= m_rcTitle.right - nWidth;

	m_rcCloseBox.top	=  m_rcTitle.top + ((m_rcTitle.bottom - m_rcTitle.top) - nHeight) / 2;
	m_rcCloseBox.bottom = m_rcCloseBox.top + nHeight;

	rcClient.bottom += (m_rcTitle.bottom - m_rcTitle.top) + 4;
	*/

	ReleaseDC(dc);

	return TRUE;
}

void CTrayPopup::DrawClientArea(CDCHandle dc)
{
	RECT rcClient;
	GetClientRect(&rcClient);

	//////////////////////////////////////////////////////////////////////////
	// Client area, border and gradient
	//////////////////////////////////////////////////////////////////////////
	dc.Draw3dRect(&rcClient, m_crBorder, m_crBorder);
	InflateRect(&rcClient, -1, -1);
	dc.Draw3dRect(&rcClient, RGB(255, 255, 255), RGB(255, 255, 255));
	InflateRect(&rcClient, -1, -1);

	RECT rcTopGrad		= rcClient;
	rcTopGrad.bottom	= (rcClient.bottom - rcClient.top) / 2;

	RECT rcBottomGrad	= rcClient;
	rcBottomGrad.top	= rcTopGrad.bottom;

	GradientFillRect(dc, &rcTopGrad, RGB(255, 255, 255), RGB(242, 242, 246), TRUE);
	GradientFillRect(dc, &rcBottomGrad, RGB(242, 242, 246), RGB(212, 212, 224), TRUE);

	rcClient.top		= m_rcTitle.bottom;

	//////////////////////////////////////////////////////////////////////////
	// Title
	//////////////////////////////////////////////////////////////////////////
	GradientFillRect(dc, &m_rcTitle, m_crTop, m_crBottom, TRUE);

	//////////////////////////////////////////////////////////////////////////
	// Icon
	//////////////////////////////////////////////////////////////////////////
	if(m_hIcon != NULL)
	{
		dc.DrawIconEx(m_rcTitle.left + 4, m_rcTitle.bottom + 4, m_hIcon, 16, 16);
		rcClient.left += 20;
	}

	//////////////////////////////////////////////////////////////////////////
	// Text
	//////////////////////////////////////////////////////////////////////////	
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_crBorder);

	RECT rcText = rcClient;

	InflateRect(&rcText, -4, -4);

	dc.SelectFont(m_fntText);
	dc.DrawText(m_strText.c_str(), -1, &rcText, DT_WORDBREAK);

	//////////////////////////////////////////////////////////////////////////
	// Close Box
	//////////////////////////////////////////////////////////////////////////
	dc.SelectFont(m_fntMarlett);

	if(m_bCloseBoxHighlite)
	{
		dc.SetTextColor(m_crBottom);
	}

	dc.DrawText(_T("r"), -1, &m_rcCloseBox, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

LRESULT CTrayPopup::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(wParam == TIMER_HIDE_DELAY)
	{
		POINT ptCurrentCursorPos;
		GetCursorPos(&ptCurrentCursorPos);

		if(m_ptInitialCurcorPos.x != ptCurrentCursorPos.x || 
		   m_ptInitialCurcorPos.y != ptCurrentCursorPos.y)
		{
			BOOL bUserActive	= TRUE;

			LASTINPUTINFO lii	= {0};
			lii.cbSize			= sizeof(lii);

			GetLastInputInfo(&lii);

			DWORD dwTimeDelta	= (GetTickCount() - lii.dwTime);

			if(dwTimeDelta > TIMER_HIDE_DELAY_VALUE)
			{
				bUserActive = FALSE;
			}

			if(bUserActive)
			{
				KillTimer(TIMER_HIDE_DELAY);
				PopdownWindow();
			}
		}
	}
	else if(wParam == TIMER_SHOW_WINDOW)
	{
		if(HasTransSupport())
		{
			m_cbFade += 10;
			m_pSetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 255), m_cbFade, LWA_ALPHA);
			if(m_cbFade >= 230)
			{
				KillTimer(TIMER_SHOW_WINDOW);
				m_pSetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 255), 230, LWA_ALPHA);
			}
		}
		else
		{
			KillTimer(TIMER_SHOW_WINDOW);
			return 0;
		}
	}
	else if(wParam == TIMER_HIDE_WINDOW)
	{
		if(HasTransSupport())
		{
			m_cbFade -= 20;
			m_pSetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 255), m_cbFade, LWA_ALPHA);
			if(m_cbFade <= 10)
			{
				KillTimer(TIMER_HIDE_WINDOW);
				HideWindow();
				m_cbFade = 100;
			}
		}
		else
		{
			KillTimer(TIMER_HIDE_WINDOW);
		}
	}
	return 0;
}

LRESULT CTrayPopup::OnMouseBtnDown(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;

	if(IsWindowVisible())
	{
		PopdownWindow();

		POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);

		if(uMsg == WM_LBUTTONDOWN && PtInRect(&m_rcCloseBox, ptCursor) == FALSE)
		{
		}
	}

	return 0;
}

LRESULT CTrayPopup::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);


	if(m_bCloseBoxHighlite)
	{
		if(PtInRect(&m_rcCloseBox, ptCursor) == FALSE)
		{
			m_bCloseBoxHighlite = FALSE;
			Invalidate();
		}
	}
	else
	{
		if(PtInRect(&m_rcCloseBox, ptCursor))
		{
			m_bCloseBoxHighlite = TRUE;
			Invalidate();
		}
	}

	return 0;
}

BOOL CTrayPopup::GradientFillRect(HDC hdc, LPCRECT lpRect, COLORREF clrFirst, COLORREF clrLast, BOOL bVertical)
{
	CDCHandle dc = hdc;

	if (clrFirst == clrLast)
	{
		COLORREF clrBk = dc.SetBkColor(clrFirst);
		BOOL bSuccess = dc.ExtTextOut(0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
		dc.SetBkColor(clrBk);

		return bSuccess;
	}

	TRIVERTEX vert[2] ;
	GRADIENT_RECT    gRect;

	vert [0] .x      = lpRect->left;
	vert [0] .y      = lpRect->top;
	//vert [0] .Red    = MAKEWORD(0, GetRValue(clrFirst));
	vert [0] .Red    = COLOR16(COLOR16(GetRValue(clrFirst))<<8);
	vert [0] .Green  = COLOR16(COLOR16(GetGValue(clrFirst))<<8);
	vert [0] .Blue   = COLOR16(COLOR16(GetBValue(clrFirst))<<8);
	vert [0] .Alpha  = 0x0000;
	vert [1] .x      = lpRect->right;
	vert [1] .y      = lpRect->bottom; 
	vert [1] .Red    = COLOR16(COLOR16(GetRValue(clrLast))<<8);
	vert [1] .Green  = COLOR16(COLOR16(GetGValue(clrLast))<<8);
	vert [1] .Blue   = COLOR16(COLOR16(GetBValue(clrLast))<<8);
	vert [1] .Alpha  = 0x0000;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	return dc.GradientFill( vert, 2, &gRect, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
}