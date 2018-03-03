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
	
	// �ð� ���
	void PrintNowTime();
public:
	Log();
	~Log();

	// ���� �Լ� ���� ��� �� ����
	void ErrorQuit(WCHAR* msg);

	// �ֿܼ� �α׸� ����ϴ� �Լ�
	void ConsolePrint(int logLevel, WCHAR* format, ...);

	// ���Ͽ� �α׸� ����ϴ� �Լ�
	void FilePrint(int logLevel, WCHAR* format, ...);

	// OutputDebugString ��� �Լ�
	void ODSPrint(int logLevel, WCHAR* format, ...);
	
	void SetLogLevel(int logLevel);
};