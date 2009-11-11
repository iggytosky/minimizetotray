#include "StdAfx.h"
#include "MonitoringThread.h"

CMonitoringThread::CMonitoringThread(void) : m_hThread(NULL), m_hEventStop(NULL)
{
}

CMonitoringThread::~CMonitoringThread(void)
{
}

BOOL CMonitoringThread::RunMonitoring()
{
	DWORD dwThreadId = 0;

	if(IsMonitoringRunning())
	{
		if(StopMonitoring() == FALSE)
		{
			return FALSE;
		}
	}

	m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);

	if(m_hEventStop == NULL)
	{
		return FALSE;
	}

	m_hThread = CreateThread(NULL, 0, _WorketThread, this, 0, &dwThreadId);

	if(m_hThread != NULL)
	{
		return TRUE;
	}
	else
	{
		CloseHandle(m_hEventStop);

		return FALSE;
	}
}

BOOL CMonitoringThread::StopMonitoring()
{
	if(IsMonitoringRunning() == FALSE)
	{
		return TRUE;
	}

	SetEvent(m_hEventStop);

	if(WaitForSingleObject(m_hThread, 2000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_hThread, 1);

		if(WaitForSingleObject(m_hThread, 2000) == WAIT_TIMEOUT)
		{
			return FALSE;
		}
	}

	CloseHandle(m_hThread);
	m_hThread = NULL;

	CloseHandle(m_hEventStop);
	m_hEventStop = NULL;

	return TRUE;
}

BOOL CMonitoringThread::IsMonitoringRunning()
{
	return (WaitForSingleObject(m_hThread, 0) != WAIT_OBJECT_0);
}

DWORD WINAPI CMonitoringThread::_WorketThread(LPVOID pParam)
{
	return ((CMonitoringThread*)pParam)->WorketThread();
}

DWORD CMonitoringThread::WorketThread()
{
	DWORD dwSleep = 200;

	do 
	{
		if(Worker() == FALSE)
		{
			break;
		}
	} 
	while(WaitForSingleObject(m_hEventStop, dwSleep) == WAIT_TIMEOUT);

	return 0;
}