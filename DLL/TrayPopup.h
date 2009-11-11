#pragma once

#include "PopupDialog.h"

typedef BOOL (WINAPI *procSetLayeredWindowAttributes)(HWND hWnd, COLORREF cr, BYTE bAlpha, DWORD dwFlags);

class CTrayPopup : public CPopupDialog<CTrayPopup>
{
public:
	enum { TIMER_HIDE_DELAY = 7234, TIMER_SHOW_WINDOW = 228, TIMER_HIDE_WINDOW = 229 };

	CTrayPopup();
	~CTrayPopup();

	BEGIN_MSG_MAP(CTrayPopup)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseBtnDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseBtnDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		CHAIN_MSG_MAP(CPopupDialog<CTrayPopup>)
	END_MSG_MAP()

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
	void PopupWindow(LPCTSTR lpszTitle, LPCTSTR lpszText);
	void PopdownWindow();

	void _Init();

protected:
	virtual BOOL CalculateClientRect(RECT &rcClient);
	virtual void DrawClientArea(CDCHandle dc);

	BOOL HasTransSupport()
	{
		return (m_pSetLayeredWindowAttributes != NULL);
	}

	BOOL GradientFillRect(HDC hdc, LPCRECT lpRect, COLORREF clrFirst, COLORREF clrLast, BOOL bVertical);
	
protected:
	procSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;

	POINT		m_ptInitialCurcorPos;

	BOOL		m_bCloseBoxHighlite;

	RECT		m_rcCloseBox;
	RECT		m_rcTitle;

	CFont		m_fntText;
	CFont		m_fntMarlett;

	COLORREF	m_crBorder;
	COLORREF	m_crBottom;
	COLORREF	m_crTop;

	BYTE		m_cbFade;
};
