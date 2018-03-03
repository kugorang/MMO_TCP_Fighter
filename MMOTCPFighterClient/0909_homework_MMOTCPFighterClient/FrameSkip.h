#pragma once

#define TIME_INTERVAL 20

class FrameSkip
{
private:
	// 시작, 끝, 주기 진동수를 얻을 구조체를 선언한다.
	LARGE_INTEGER start, end, frequency;
	DWORD cumulativeTime;
public:
	FrameSkip();
	~FrameSkip();

	void FrameSkipStart();
	bool IsFrameNotDelayed();
};