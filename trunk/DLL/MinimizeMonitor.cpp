#include "StdAfx.h"
#include "MinimizeMonitor.h"

CMinimizeMonitor::CMinimizeMonitor() : m_pCallback(NULL)
{
}

CMinimizeMonitor::~CMinimizeMonitor()
{
}

DWORD CMinimizeMonitor::MonitoringThread()
{
	POINT	cursor;

	DWORD	waitDelay = 5;

	DWORD	keys[]	= {VK_LBUTTON, VK_RBUTTON};
	DWORD	mouseButton;

	HWND	targetWindow;
	LRESULT hitTestResult;

	while(WaitStop(waitDelay))
	{
		mouseButton = 0;

		for(size_t i = 0; i < _countof(keys); ++i)
		{
			if(GetAsyncKeyState(keys[i]) != 0)
			{
				mouseButton = keys[i];
				break;
			}
		}

		if(mouseButton == 0)
		{
			continue;
		}

		GetCursorPos(&cursor);

		targetWindow = WindowFromPoint(cursor);

		if(targetWindow == NULL)
		{
			continue;
		}

		hitTestResult = ::SendMessage(targetWindow, WM_NCHITTEST, 0, MAKELPARAM(cursor.x, cursor.y));

		if(hitTestResult != HTMINBUTTON)
		{
			continue;
		}

		while(GetAsyncKeyState(mouseButton) != 0 && WaitStop(1));

		GetCursorPos(&cursor);

		hitTestResult = ::SendMessage(targetWindow, WM_NCHITTEST, 0, MAKELPARAM(cursor.x, cursor.y));

		switch(mouseButton)
		{
		case VK_LBUTTON:
			{
				if(hitTestResult != HTMINBUTTON && IsIconic(targetWindow) == FALSE)
				{
					continue;
				}
			}
			break;

		case VK_RBUTTON:
			{
				hitTestResult = ::SendMessage(targetWindow, WM_NCHITTEST, 0, MAKELPARAM(cursor.x, cursor.y));

				if(hitTestResult != HTMINBUTTON)
				{
					continue;
				}
			}
			break;
		}

		OnMouseButtonUp(targetWindow, mouseButton, cursor);
	}

	return 0;
}