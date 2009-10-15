#pragma once

class CMonitoringThread
{
public:
	CMonitoringThread(void);
	~CMonitoringThread(void);

public:
	BOOL RunMonitoring();
	BOOL StopMonitoring();
	BOOL IsMonitoringRunning();

protected:
	static DWORD WINAPI _WorketThread(LPVOID pParam);
	DWORD WorketThread();

	virtual BOOL Worker() = 0;

protected:
	HANDLE	m_hThread;
	HANDLE	m_hEventStop;
};
