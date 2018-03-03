#include "main.h"
#include "Select.h"
#include "Sector.h"

DeadReckoning::DeadReckoning() {}

DeadReckoning::DeadReckoning(WORD startPosX, WORD startPosY)
{
	this->startPosX = startPosX;
	this->startPosY = startPosY;

	checkPosTimer.TimerStart();
	checkSameDirectionTimer.TimerStart();
}

DeadReckoning::~DeadReckoning() {}

bool DeadReckoning::CheckSameDirection()
{
	// 끝 시점의 진동수를 얻는다.
	checkSameDirectionTimer.TimerEnd();

	if (checkSameDirectionTimer.CheckTime() >= TIME_GAP_INTERVAL)
	{
		checkSameDirectionTimer.TimerStart();
		return true;
	}
	else
	{
		return false;
	}
}

void DeadReckoning::CheckPos(SOCKET sock, BYTE direction, WORD *posX, WORD *posY)
{
	// 끝 시점의 진동수를 얻는다.
	checkPosTimer.TimerEnd();

	SOCKETINFO* checkSocketInfo = socketInfoMap.find(sock)->second;
	PLAYER **checkPlayer = &(checkSocketInfo->player);

	if ((*checkPlayer) == nullptr)
	{
		return;
	}

	UINT64 socketNo = checkSocketInfo->socketNo;
	
	short checkPlayerPosX = (short)((*checkPlayer)->posX);
	short checkPlayerPosY = (short)((*checkPlayer)->posY);

	// 클라이언트와 서버의 좌표가 많은 차이가 날 때
	if ((*posX < checkPlayerPosX - CHECK_X_RANGE / 2 || *posX > checkPlayerPosX + CHECK_X_RANGE / 2)
		|| (*posY < checkPlayerPosY - CHECK_Y_RANGE / 2 || *posY > checkPlayerPosY + CHECK_Y_RANGE / 2))
	{
		WORD frameCount = (WORD)((double)checkPosTimer.CheckTime() / (DEFAULT_FRAME_TIME * FRAME_INTERVAL_OPTION));
		
		short deadReckoningPosX = *posX, deadReckoningPosY = *posY;

		BYTE cpDirection = (*checkPlayer)->direction;

		switch (cpDirection)
		{
		case PACKETDEFINE_MOVE_DIR_LL:
			deadReckoningPosX = startPosX - frameCount * CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosX < 0)
			{
				deadReckoningPosX = 0;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_LU:
			deadReckoningPosY = startPosY - frameCount * CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
			deadReckoningPosX = startPosX - frameCount * CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosX < 0 && deadReckoningPosY < 0)
			{
				// y절편을 먼저 구한다.
				int yIntercept = startPosY - startPosX * CHARACTER_UD_MOVE_RANGE / CHARACTER_LR_MOVE_RANGE;

				// 맵 범위 안에 y절편이 있다면, y절편을 구한다.
				if (yIntercept > 0)
				{
					deadReckoningPosY = yIntercept;
					deadReckoningPosX = 0;
				}
				// 맵 범위 밖에 y절편이 있다면, x절편을 구한다.
				else
				{
					deadReckoningPosX = startPosX - startPosY * CHARACTER_LR_MOVE_RANGE / CHARACTER_UD_MOVE_RANGE;
					deadReckoningPosY = 0;
				}
			}
			else if (deadReckoningPosY < 0)	// x절편 구하기
			{
				deadReckoningPosX = startPosX - startPosY * CHARACTER_LR_MOVE_RANGE / CHARACTER_UD_MOVE_RANGE;
				deadReckoningPosY = 0;
			}
			else if (deadReckoningPosX < 0)	// y절편 구하기
			{
				deadReckoningPosY = startPosY - startPosX * CHARACTER_UD_MOVE_RANGE / CHARACTER_LR_MOVE_RANGE;
				deadReckoningPosX = 0;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_LD:
			deadReckoningPosX = startPosX - frameCount * CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
			deadReckoningPosY = startPosY + frameCount * CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosX < 0 && deadReckoningPosY > MAP_HEIGHT - 1)
			{
				// y절편을 먼저 구한다.
				int yIntercept = startPosY + startPosX * CHARACTER_UD_MOVE_RANGE / CHARACTER_LR_MOVE_RANGE;
				
				// 맵 범위 안에 y절편이 있다면, y절편을 구한다.
				if (yIntercept < MAP_HEIGHT)
				{
					deadReckoningPosY = yIntercept;
					deadReckoningPosX = 0;
				}
				else // 맵 범위 밖에 y절편이 있다면, y가 HEIGHT - 1 일 때 x값을 구한다.
				{
					deadReckoningPosX = startPosX - (MAP_HEIGHT - 1 - startPosY) * CHARACTER_LR_MOVE_RANGE
						/ CHARACTER_UD_MOVE_RANGE;
					deadReckoningPosY = MAP_HEIGHT - 1;
				}
			}
			else if (deadReckoningPosX < 0)	// y절편 구하기
			{
				deadReckoningPosY = startPosY + startPosX * CHARACTER_UD_MOVE_RANGE / CHARACTER_LR_MOVE_RANGE;
				deadReckoningPosX = 0;
			}
			else if (deadReckoningPosY > MAP_HEIGHT - 1)	// y가 HEIGHT - 1 일 때 x값 구하기
			{
				deadReckoningPosX = startPosX - (MAP_HEIGHT - 1 - startPosY) * CHARACTER_LR_MOVE_RANGE 
					/ CHARACTER_UD_MOVE_RANGE;
				deadReckoningPosY = MAP_HEIGHT - 1;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_RU:
			deadReckoningPosX = startPosX + frameCount * CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
			deadReckoningPosY = startPosY - frameCount * CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosX > MAP_WIDTH - 1 && deadReckoningPosY < 0)
			{
				// x가 WIDTH - 1 일 때 y값을 구한다.
				int yValue = startPosY - (MAP_WIDTH - 1 - startPosX) * CHARACTER_UD_MOVE_RANGE 
					/ CHARACTER_LR_MOVE_RANGE;

				// y값이 0 보다 크다면, x가 MAP_WIDTH - 1 일 때 y값을 구한다.
				if (yValue > 0)
				{
					deadReckoningPosY = yValue;
					deadReckoningPosX = MAP_WIDTH - 1;
				}
				else // y값이 0 보다 작다면, x절편을 구한다.
				{
					deadReckoningPosX = startPosX - startPosY * CHARACTER_LR_MOVE_RANGE / CHARACTER_UD_MOVE_RANGE;
					deadReckoningPosY = 0;
				}
			}
			else if (deadReckoningPosX > MAP_WIDTH - 1)	// x가 MAP_WIDTH - 1 일 때, y값을 구한다.
			{
				deadReckoningPosY = startPosY - (MAP_WIDTH - 1 - startPosX) * CHARACTER_UD_MOVE_RANGE
					/ CHARACTER_LR_MOVE_RANGE;
				deadReckoningPosX = MAP_WIDTH - 1;
			}
			else if (deadReckoningPosY < 0)	// x절편을 구한다.
			{
				deadReckoningPosX = startPosX + startPosY * CHARACTER_LR_MOVE_RANGE / CHARACTER_UD_MOVE_RANGE;
				deadReckoningPosY = 0;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_RR:
			deadReckoningPosX = startPosX + frameCount * CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosX > MAP_WIDTH - 1)
			{
				deadReckoningPosX = MAP_WIDTH - 1;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_RD:
			deadReckoningPosX = startPosX + frameCount * CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
			deadReckoningPosY = startPosY + frameCount * CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosX > MAP_WIDTH - 1 && deadReckoningPosY > MAP_HEIGHT - 1)
			{
				// x가 WIDTH - 1 일 때 y값을 구한다.
				int yValue = startPosY + 
					(MAP_WIDTH - 1 - startPosX) * CHARACTER_UD_MOVE_RANGE / CHARACTER_LR_MOVE_RANGE;

				// 맵 범위 안에 y절편이 있다면, x가 MAP_WIDTH - 1 일 때 y값을 구한다.
				if (yValue < MAP_HEIGHT)
				{
					deadReckoningPosY = yValue;
					deadReckoningPosX = MAP_HEIGHT - 1;
				}
				else // 맵 범위 밖에 y절편이 있다면, y가 MAP_HEIGHT - 1 일 때 x값을 구한다.
				{
					deadReckoningPosX = (MAP_HEIGHT - 1 - startPosY) * CHARACTER_LR_MOVE_RANGE
						/ CHARACTER_UD_MOVE_RANGE + startPosX;
					deadReckoningPosY = MAP_HEIGHT - 1;
				}
			}
			else if (deadReckoningPosX > MAP_WIDTH - 1)	// x가 MAP_WIDTH - 1 일 때 y값 구하기
			{
				deadReckoningPosY = (MAP_WIDTH - 1 - startPosX) * CHARACTER_UD_MOVE_RANGE 
					/ CHARACTER_LR_MOVE_RANGE + startPosY;
				deadReckoningPosX = MAP_WIDTH - 1;
			}
			else if (deadReckoningPosY > MAP_HEIGHT - 1)	// y가 MAP_HEIGHT - 1 일 때 x값 구하기
			{
				deadReckoningPosX = (MAP_HEIGHT - 1 - startPosY) * CHARACTER_LR_MOVE_RANGE
					/ CHARACTER_UD_MOVE_RANGE + startPosX;
				deadReckoningPosY = MAP_HEIGHT - 1;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_UU:
			deadReckoningPosY = startPosY - frameCount * CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosY < 0)
			{
				deadReckoningPosY = 0;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_DD:
			deadReckoningPosY = startPosY + frameCount * CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

			if (deadReckoningPosY > MAP_HEIGHT - 1)
			{
				deadReckoningPosY = MAP_HEIGHT - 1;
			}
			break;
		default:
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"CheckPos() MOVE_DIR Error [SOCKET_NO:%llu]",
				socketNo);
			RemoveSocketInfo(sock);
			break;
		}
		
		// 데드 레커닝 좌표와 클라이언트 좌표가 많은 차이가 날 때
		// 클라이언트와 서버 좌표 모두 보정
		if ((*posX < deadReckoningPosX - CHECK_X_RANGE / 2 || *posX > deadReckoningPosX + CHECK_X_RANGE / 2)
			|| (*posY < deadReckoningPosY - CHECK_Y_RANGE / 2 || *posY > deadReckoningPosY + CHECK_Y_RANGE / 2))
		{
			printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"CheckPos() StartPosX:%d StartPosY:%d [SOCKET_NO:%llu]",
				startPosX, startPosY, socketNo);
			printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"CheckPos() DRPosX:%d DRPosY:%d [SOCKET_NO:%llu]",
				deadReckoningPosX, deadReckoningPosY, socketNo);
			printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"CheckPos() Direction:%d Frame:%d [SOCKET_NO:%llu]",
				cpDirection, frameCount, socketNo);
			printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"CheckPos() OldPosX:%d OldPosY:%d [SOCKET_NO:%llu]",
				*posX, *posY, socketNo);

			SendSync(sock, (*checkPlayer)->userID, deadReckoningPosX, deadReckoningPosY);
		}

		*posX = deadReckoningPosX;
		*posY = deadReckoningPosY;
	}
}

void DeadReckoning::Start(WORD startPosX, WORD startPosY)
{
	this->startPosX = startPosX;
	this->startPosY = startPosY;

	checkPosTimer.TimerStart();
	checkSameDirectionTimer.TimerStart();
}