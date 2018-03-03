#include "Timer.h"

Timer::Timer()
{
	lastTime = 0;
	nowTime = 0;
	maxTime = 0;

	startTime.QuadPart = 0;
	endTime.QuadPart = 0;
	frequency.QuadPart = 0;

	// 고해상도 타이머의 주기를 얻는다.
	QueryPerformanceFrequency(&frequency);
}

Timer::~Timer()
{

}

void Timer::TimerStart()
{
	// 시작 시점의 진동수를 얻는다.
	QueryPerformanceCounter(&startTime);

	lastTime = (DWORD)startTime.QuadPart;
}

void Timer::TimerEnd()
{
	// 끝 시점의 진동수를 얻는다.
	QueryPerformanceCounter(&endTime);

	nowTime = (DWORD)endTime.QuadPart;

	CheckTimeGap();
}

DWORD Timer::CheckTime()
{
	return (DWORD)((double)(endTime.QuadPart - startTime.QuadPart) / (double)(frequency.QuadPart / 1000));
}

double Timer::CheckTimeGap(bool printTime)
{
	DWORD timeGap = nowTime - lastTime;
	
	lastTime = nowTime;

	if (timeGap > maxTime)
	{
		maxTime = timeGap;
	}

	double retval = (double)maxTime / (double)(frequency.QuadPart / 1000);

	if (printTime)
	{
		maxTime = 0;
	}

	return retval;
}