#pragma once

#include <Windows.h>

class Timer
{
private:
	DWORD lastTime, nowTime, maxTime;

	// ����, ��, �ֱ� �������� ���� ����ü�� �����Ѵ�.
	LARGE_INTEGER startTime, endTime, frequency;
public:
	Timer();
	~Timer();

	void TimerStart();
	void TimerEnd();

	DWORD CheckTime();
	double CheckTimeGap(bool printTime = false);
};