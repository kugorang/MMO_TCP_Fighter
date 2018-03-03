#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

enum LOGLEVEL
{
	LOGDEBUG,
	LOGWARNING,
	LOGERROR,
	LOGINFO
};

class Log
{
private:
	int logLevel;

	WCHAR buffer[2048];
	
	// 시간 출력
	void PrintNowTime();
public:
	Log();
	~Log();

	// 소켓 함수 오류 출력 후 종료
	void ErrorQuit(WCHAR* msg);

	// 콘솔에 로그를 출력하는 함수
	void ConsolePrint(int logLevel, WCHAR* format, ...);

	// 파일에 로그를 출력하는 함수
	void FilePrint(int logLevel, WCHAR* format, ...);

	// OutputDebugString 출력 함수
	void ODSPrint(int logLevel, WCHAR* format, ...);
	
	void SetLogLevel(int logLevel);
};