#pragma once

#define TIME_GAP_INTERVAL 3000

#include <Windows.h>

class DeadReckoning
{
private:
	WORD startPosX, startPosY;

	Timer checkPosTimer;
	Timer checkSameDirectionTimer;
public:
	DeadReckoning();
	DeadReckoning(WORD startPosX, WORD startPosY);
	~DeadReckoning();

	bool CheckSameDirection();
	void CheckPos(SOCKET sock, BYTE direction, WORD *posX, WORD *posY);

	void Start(WORD startPosX, WORD startPosY);

	void SetStartPos(WORD startPosX, WORD startPosY);
};