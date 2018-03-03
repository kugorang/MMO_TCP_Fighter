#include "stdafx.h"
#include "FrameSkip.h"
#include "0909_homework_MMOTCPFighterClient.h"

FrameSkip::FrameSkip() 
{
	cumulativeTime = 0;

	start.QuadPart = 0;
	end.QuadPart = 0;
	frequency.QuadPart = 0;

	// 고해상도 타이머의 주기를 얻는다.
	QueryPerformanceFrequency(&frequency);
}

FrameSkip::~FrameSkip() {}

bool FrameSkip::IsFrameNotDelayed()
{
	if (start.QuadPart == 0)
	{
		return false;
	}

	// 끝 시점의 진동수를 얻는다.
	QueryPerformanceCounter(&end);

	// 끝 시점의 진동수에서 시작 시점의 진동수를 뺀 후 진동수에서 1000으로 나눈 값을 나눈다.
	// 1초 기준의 주기를 1000 으로 나누었기 때문에 1밀리초 동안 발생하는 진동수로 나눈 셈이다.
	DWORD ms_interval = (DWORD)((end.QuadPart - start.QuadPart) / (frequency.QuadPart / 1000));
	
	if (ms_interval < TIME_INTERVAL && cumulativeTime < TIME_INTERVAL)
	{
		Sleep(TIME_INTERVAL - ms_interval);
		cumulativeTime += TIME_INTERVAL;
	}
	else
	{
		cumulativeTime += ms_interval;
	}

	cumulativeTime -= TIME_INTERVAL;

	if (cumulativeTime >= TIME_INTERVAL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void FrameSkip::FrameSkipStart()
{		
	// 시작 시점의 진동수를 얻는다.
	QueryPerformanceCounter(&start);
}