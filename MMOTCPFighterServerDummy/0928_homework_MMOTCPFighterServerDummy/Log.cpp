#include "Log.h"

Log::Log()
{
	memset(buffer, 0, sizeof(buffer));
	logLevel = LOGLEVEL::LOGDEBUG;
}

Log::~Log() {}

// 소켓 함수 오류 출력 후 종료
void Log::ErrorQuit(WCHAR* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

void Log::ConsolePrint(int logLevel, WCHAR* format, ...)
{
	if (this->logLevel > logLevel
		|| (logLevel < LOGLEVEL::LOGDEBUG && logLevel > LOGLEVEL::LOGINFO))
	{
		return;
	}

	PrintNowTime();

	va_list vaList;
	va_start(vaList, format);

	switch (logLevel)
	{
	case LOGLEVEL::LOGDEBUG:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"DEBUG : ");
		break;
	case LOGLEVEL::LOGWARNING:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"WARNING : ");
		break;
	case LOGLEVEL::LOGERROR:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"ERROR : ");
		break;
	case LOGLEVEL::LOGINFO:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"INFO : ");
		break;
	default:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"LogLevel Error!\r\n");
		wprintf_s(buffer);
		return;
	}

	vswprintf_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), format, vaList);
	va_end(vaList);

	wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"\r\n");

	wprintf_s(buffer);

	wofstream fout;

	fout.open("Log.txt", ios::app | ios::binary);

	fout << buffer;

	fout.close();

	memset(buffer, 0, sizeof(buffer));
}

// 파일에 로그를 출력하는 함수
void Log::FilePrint(int logLevel, WCHAR* format, ...)
{
	if (this->logLevel > logLevel
		|| (logLevel < LOGLEVEL::LOGDEBUG && logLevel > LOGLEVEL::LOGINFO))
	{
		return;
	}

	PrintNowTime();

	va_list vaList;
	va_start(vaList, format);

	switch (logLevel)
	{
	case LOGLEVEL::LOGDEBUG:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"DEBUG : ");
		break;
	case LOGLEVEL::LOGWARNING:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"WARNING : ");
		break;
	case LOGLEVEL::LOGERROR:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"ERROR : ");
		break;
	case LOGLEVEL::LOGINFO:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"INFO : ");
		break;
	default:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"LogLevel Error!\r\n");
		wprintf_s(buffer);
		return;
	}

	vswprintf_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), format, vaList);
	va_end(vaList);

	wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"\r\n");
	
	wofstream fout;

	fout.open("Log.txt", ios::app | ios::binary);

	fout << buffer;

	fout.close();

	memset(buffer, 0, sizeof(buffer));
}

// OutputDebugString 출력 함수
// ODS : OutputDebugString
void Log::ODSPrint(int logLevel, WCHAR* format, ...)
{
	if (this->logLevel > logLevel
		|| (logLevel < LOGLEVEL::LOGDEBUG && logLevel > LOGLEVEL::LOGERROR))
	{
		return;
	}
	
	PrintNowTime();

	va_list vaList;
	va_start(vaList, format);
	
	switch (logLevel)
	{
	case LOGLEVEL::LOGDEBUG:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"DEBUG : ");
		break;
	case LOGLEVEL::LOGWARNING:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"WARNING : ");
		break;
	case LOGLEVEL::LOGERROR:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"ERROR : ");
		break;
	case LOGLEVEL::LOGINFO:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"INFO : ");
		break;
	default:
		wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"LogLevel Error!\r\n");
		OutputDebugString(buffer);
		return;
	}
	
	vswprintf_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), format, vaList);
	va_end(vaList);

	wcscpy_s(buffer + wcslen(buffer), sizeof(buffer) / 2 - wcslen(buffer), L"\r\n");

	OutputDebugString(buffer);

	wofstream fout;

	fout.open("Log.txt", ios::app | ios::binary);

	fout << buffer;

	fout.close();

	memset(buffer, 0, sizeof(buffer));
}

void Log::SetLogLevel(int logLevel)
{
	this->logLevel = logLevel;
}

void Log::PrintNowTime()
{
	SYSTEMTIME stime;

	// structure to store system time (in usual time format)
	FILETIME ltime;

	// structure to store local time (local time in 64 bits)
	FILETIME ftTimeStamp;

	// Gets the current system time
	GetSystemTimeAsFileTime(&ftTimeStamp); 

	// convert in local time and store in ltime
	FileTimeToLocalFileTime(&ftTimeStamp, &ltime);

	// convert in system time and store in stime
	FileTimeToSystemTime(&ltime, &stime);

	wsprintf(buffer, L"[%02d-%02d-%02d %02d:%02d:%02d:%03d] ", (stime.wYear % 1000) % 100, 
		stime.wMonth, stime.wDay, stime.wHour, stime.wMinute, stime.wSecond, stime.wMilliseconds);
}