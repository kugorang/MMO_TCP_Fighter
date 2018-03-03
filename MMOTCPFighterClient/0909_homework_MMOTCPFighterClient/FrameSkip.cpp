#include "stdafx.h"
#include "FrameSkip.h"
#include "0909_homework_MMOTCPFighterClient.h"

FrameSkip::FrameSkip() 
{
	cumulativeTime = 0;

	start.QuadPart = 0;
	end.QuadPart = 0;
	frequency.QuadPart = 0;

	// ���ػ� Ÿ�̸��� �ֱ⸦ ��´�.
	QueryPerformanceFrequency(&frequency);
}

FrameSkip::~FrameSkip() {}

bool FrameSkip::IsFrameNotDelayed()
{
	if (start.QuadPart == 0)
	{
		return false;
	}

	// �� ������ �������� ��´�.
	QueryPerformanceCounter(&end);

	// �� ������ ���������� ���� ������ �������� �� �� ���������� 1000���� ���� ���� ������.
	// 1�� ������ �ֱ⸦ 1000 ���� �������� ������ 1�и��� ���� �߻��ϴ� �������� ���� ���̴�.
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
	// ���� ������ �������� ��´�.
	QueryPerformanceCounter(&start);
}