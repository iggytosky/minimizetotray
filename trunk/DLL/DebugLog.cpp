#include "StdAfx.h"
#include "DebugLog.h"

#ifdef _DEBUG
CDebugLog g_DebugLog;
#endif

CDebugLog::CDebugLog(void) : m_hLogFile(INVALID_HANDLE_VALUE)
{
	OpenLogFile();
}

CDebugLog::~CDebugLog(void)
{
	CloseLogFile();
}

BOOL CDebugLog::Log(TCHAR *fmt, ...)
{
	if(m_hLogFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	size_t nLen;

	TCHAR szOutBuffer[1024]	= {0};
	TCHAR szInBuffer[1024]	= {0};

	DWORD dwInBuffer	= _countof(szInBuffer);
	DWORD dwOutBuffer	= _countof(szOutBuffer);
	DWORD dwBytesWritten;

	auto va_list	argptr;

	time_t			rawtime		= 0;
	struct tm 		timeinfo	= {0};

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	_wasctime_s(szInBuffer, _countof(szInBuffer), &timeinfo);

	nLen = wcslen(szInBuffer);
	szInBuffer[nLen - 1] = ' ';

	va_start(argptr, fmt);
	vswprintf_s(szInBuffer + nLen, _countof(szInBuffer) - nLen - 1, fmt, argptr);
	va_end(argptr);

	nLen = wcslen(szInBuffer);

	if(nLen == 0)
	{
		return TRUE;
	}

	wcscat_s(szInBuffer, _countof(szInBuffer), _T("\n"));

	dwInBuffer			= (DWORD)wcslen(szInBuffer);
	char *pszOutBuffer	= new char[dwInBuffer + 4];
	WideCharToMultiByte(CP_ACP, 0, szInBuffer, -1, pszOutBuffer, dwInBuffer + 1, NULL, NULL);

	if(m_hLogFile == INVALID_HANDLE_VALUE)
	{
		delete [] pszOutBuffer;
		return FALSE;
	}

	OutputDebugString(szInBuffer);
	WriteFile(m_hLogFile, pszOutBuffer, dwInBuffer, &dwBytesWritten, NULL);

	delete [] pszOutBuffer;

	return TRUE;
}

BOOL CDebugLog::OpenLogFile()
{
	if(IsLogFileOpen())
	{
		return TRUE;
	}

	wstring strFilePath = _T("d:\\logs\\chrome.txt");

	m_hLogFile = ::CreateFile(strFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, TRUNCATE_EXISTING, 0, NULL);

	return IsLogFileOpen();
}

BOOL CDebugLog::CloseLogFile()
{
	if(IsLogFileOpen() == FALSE)
	{
		return FALSE;
	}

	return CloseHandle(m_hLogFile);
}

BOOL CDebugLog::IsLogFileOpen()
{
	return (m_hLogFile != INVALID_HANDLE_VALUE);
}