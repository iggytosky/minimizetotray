#pragma once

class CActivityMonitor
{
public:
	CActivityMonitor(void);
	~CActivityMonitor(void);

public:
	BOOL Start();
	BOOL Stop();
	BOOL IsStarted();

	BOOL Pause();
	BOOL Continue();
	BOOL IsPaused();

	BOOL WaitStop(DWORD millisecs);

private:
	static DWORD WINAPI _MonitoringThread(LPVOID pParam);
	virtual DWORD MonitoringThread() = 0;

private:
	HANDLE							m_eventStop;
	HANDLE							m_eventPause;

	HANDLE							m_threadMonitor;
};
