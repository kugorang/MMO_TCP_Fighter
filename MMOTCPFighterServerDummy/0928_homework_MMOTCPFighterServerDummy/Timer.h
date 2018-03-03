#pragma once

#include <Windows.h>

class Timer
{
private:
	DWORD lastTime, nowTime, maxTime;

	// 시작, 끝, 주기 진동수를 얻을 구조체를 선언한다.
	LARGE_INTEGER startTime, endTime, frequency;
public:
	Timer();
	~Timer();

	void TimerStart();
	void TimerEnd();

	DWORD CheckTime();
	double CheckTimeGap(bool printTime = false);
};