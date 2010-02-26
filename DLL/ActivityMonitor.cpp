#include "StdAfx.h"
#include "ActivityMonitor.h"

CActivityMonitor::CActivityMonitor() : 
	m_eventStop(NULL), m_eventPause(NULL), 
	m_threadMonitor(NULL)
{
}

CActivityMonitor::~CActivityMonitor()
{
}

BOOL CActivityMonitor::WaitStop(DWORD millisecs)
{
	return (WaitForSingleObject(m_eventStop, millisecs) != WAIT_OBJECT_0);
}

BOOL CActivityMonitor::IsStarted()
{
	if(m_threadMonitor == NULL)
	{
		return FALSE;
	}

	return (WaitForSingleObject(m_threadMonitor, 0) != WAIT_TIMEOUT);
}

BOOL CActivityMonitor::Start()
{
	if(IsStarted())
	{
		return TRUE;
	}

	m_eventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_eventPause = CreateEvent(NULL, TRUE, FALSE, NULL);

	ResetEvent(m_eventStop);
	ResetEvent(m_eventStop);

	m_threadMonitor = CreateThread(NULL, 0, _MonitoringThread, this, 0, NULL);

	return IsStarted();
}

BOOL CActivityMonitor::Stop()
{
	if(IsStarted() == FALSE)
	{
		return TRUE;
	}

	if(IsPaused())
	{
		Continue();
	}

	SetEvent(m_eventStop);

	WaitForSingleObject(m_threadMonitor, INFINITE);

	CloseHandle(m_eventStop);
	m_eventStop = NULL;

	CloseHandle(m_eventPause);
	m_eventPause = NULL;

	CloseHandle(m_threadMonitor);
	m_threadMonitor = NULL;

	return IsStarted();
}

BOOL CActivityMonitor::Pause()
{
	if(IsStarted() == FALSE)
	{
		return TRUE;
	}

	if(IsPaused())
	{
		return TRUE;
	}

	SetEvent(m_eventPause);

	return TRUE;
}

BOOL CActivityMonitor::Continue()
{
	if(IsStarted() == FALSE)
	{
		return FALSE;
	}

	if(IsPaused() == FALSE)
	{
		return TRUE;
	}

	ResetEvent(m_eventPause);

	return TRUE;
}

BOOL CActivityMonitor::IsPaused()
{
	if(IsStarted() == FALSE)
	{
		return FALSE;
	}

	return (WaitForSingleObject(m_eventPause, 0) == WAIT_TIMEOUT);
}

DWORD CActivityMonitor::_MonitoringThread(LPVOID pParam)
{
	CActivityMonitor *pMonitor = reinterpret_cast<CActivityMonitor*>(pParam);

	return pMonitor->MonitoringThread();
}