#pragma once

#define TIME_INTERVAL 20

class FrameSkip
{
private:
	// ����, ��, �ֱ� �������� ���� ����ü�� �����Ѵ�.
	LARGE_INTEGER start, end, frequency;
	DWORD cumulativeTime;
public:
	FrameSkip();
	~FrameSkip();

	void FrameSkipStart();
	bool IsFrameNotDelayed();
};