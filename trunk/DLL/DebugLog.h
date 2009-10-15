#pragma once

#ifdef _DEBUG
#define DebugLog g_DebugLog.Log
#else
#define DebugLog __noop
#endif

class CDebugLog
{
public:
	CDebugLog(void);
	~CDebugLog(void);

public:
	BOOL Log(TCHAR *fmt, ...);

protected:
	BOOL OpenLogFile();
	BOOL CloseLogFile();
	BOOL IsLogFileOpen();

protected:
	HANDLE	m_hLogFile;
};

extern CDebugLog g_DebugLog;