#pragma once

#include "ActivityMonitor.h"

class CMinimizeMonitorCallback
{
public:
	virtual BOOL OnMouseButtonUp(const HWND &targetWindow, const DWORD &mouseButton, const POINT &coord) = 0;
};

class CMinimizeMonitor : public CActivityMonitor
{
public:
	CMinimizeMonitor();
	virtual ~CMinimizeMonitor();

	void SetCallBack(CMinimizeMonitorCallback *pCallback)
	{
		m_pCallback = pCallback;
	}

protected:
	BOOL OnMouseButtonUp(const HWND &targetWindow, const DWORD &mouseButton, const POINT &coord)
	{
		if(m_pCallback != NULL)
		{
			m_pCallback->OnMouseButtonUp(targetWindow, mouseButton, coord);
		}

		return TRUE;
	}

private:
	virtual DWORD MonitoringThread();

private:
	CMinimizeMonitorCallback	*m_pCallback;
};
