#include "main.h"
#include "Select.h"
#include "Sector.h"

Sector::Sector()
{
	memset(insertSector, 0, sizeof(insertSector));
	memset(removeSector, 0, sizeof(removeSector));
}

Sector::~Sector() {}

void Sector::CheckSector(SOCKETINFO* checkSocketInfo)
{
	// cp : checkPlayer
	PLAYER* checkPlayer = checkSocketInfo->player;
	POINT* csiSectorIndex = &(checkSocketInfo->sectorIndex);

	int cpSectorIndexX = csiSectorIndex->x;
	int cpSectorIndexY = csiSectorIndex->y;
	int cpPosXIndex = checkPlayer->posX / SECTOR_WIDTH;
	int cpPosYIndex = checkPlayer->posY / SECTOR_HEIGHT;

	if (cpSectorIndexX != cpPosXIndex || cpSectorIndexY != cpPosYIndex)
	{
		CheckAroundSector(cpSectorIndexX, cpSectorIndexY, cpPosXIndex, cpPosYIndex);

		DeleteSector(checkSocketInfo, cpSectorIndexX, cpSectorIndexY, SECTORFLAG::MOVE);
		InsertSector(checkSocketInfo, cpPosXIndex, cpPosYIndex, SECTORFLAG::MOVE);
	}
}

void Sector::CheckAroundSector(int oldIndexX, int oldIndexY, int newIndexX, int newIndexY)
{
	int xDirection = oldIndexX - newIndexX;

	if (xDirection >= 1)
	{
		xDirection = 1;
	}
	else if (xDirection <= -1)
	{
		xDirection = -1;
	}
	else
	{
		xDirection = 0;
	}

	int yDirection = oldIndexY - newIndexY;

	if (yDirection >= 1)
	{
		yDirection = 1;
	}
	else if (yDirection <= -1)
	{
		yDirection = -1;
	}
	else
	{
		yDirection = 0;
	}

	switch (xDirection)
	{
	case -1:	// 오른쪽으로 이동
		switch (yDirection)
		{
		case -1:	// RD 이동
			removeSector[0][0] = 1;
			removeSector[0][1] = 1;
			removeSector[0][2] = 1;
			removeSector[1][0] = 1;
			removeSector[2][0] = 1;

			insertSector[0][2] = 1;
			insertSector[1][2] = 1;
			insertSector[2][0] = 1;
			insertSector[2][1] = 1;
			insertSector[2][2] = 1;
			break;
		case 0:	// RR 이동
			removeSector[0][0] = 1;
			removeSector[1][0] = 1;
			removeSector[2][0] = 1;

			insertSector[0][2] = 1;
			insertSector[1][2] = 1;
			insertSector[2][2] = 1;
			break;
		case 1:	// RU 이동
			removeSector[0][0] = 1;
			removeSector[1][0] = 1;
			removeSector[2][0] = 1;
			removeSector[2][1] = 1;
			removeSector[2][2] = 1;

			insertSector[0][0] = 1;
			insertSector[0][1] = 1;
			insertSector[0][2] = 1;
			insertSector[1][2] = 1;
			insertSector[2][2] = 1;
			break;
		default:
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"CheckAroundSector() - xDirection:-1");
			break;
		}
		break;
	case 0:		// 위, 아래로 이동
		switch (yDirection)
		{
		case -1:	// DD 이동
			removeSector[0][0] = 1;
			removeSector[0][1] = 1;
			removeSector[0][2] = 1;

			insertSector[2][0] = 1;
			insertSector[2][1] = 1;
			insertSector[2][2] = 1;
			break;
		case 1:	// UU 이동
			removeSector[2][0] = 1;
			removeSector[2][1] = 1;
			removeSector[2][2] = 1;

			insertSector[0][0] = 1;
			insertSector[0][1] = 1;
			insertSector[0][2] = 1;
			break;
		default:
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"CheckAroundSector() - xDirection:0");
			break;
		}
		break;
	case 1:		// 왼쪽으로 이동
		switch (yDirection)
		{
		case -1:	// LD 이동
			removeSector[0][0] = 1;
			removeSector[0][1] = 1;
			removeSector[0][2] = 1;
			removeSector[1][2] = 1;
			removeSector[2][2] = 1;

			insertSector[0][0] = 1;
			insertSector[1][0] = 1;
			insertSector[2][0] = 1;
			insertSector[2][1] = 1;
			insertSector[2][2] = 1;
			break;
		case 0:	// LL 이동
			removeSector[0][2] = 1;
			removeSector[1][2] = 1;
			removeSector[2][2] = 1;

			insertSector[0][0] = 1;
			insertSector[1][0] = 1;
			insertSector[2][0] = 1;
			break;
		case 1:	// LU 이동
			removeSector[0][2] = 1;
			removeSector[1][2] = 1;
			removeSector[2][0] = 1;
			removeSector[2][1] = 1;
			removeSector[2][2] = 1;

			insertSector[0][0] = 1;
			insertSector[0][1] = 1;
			insertSector[0][2] = 1;
			insertSector[1][0] = 1;
			insertSector[2][0] = 1;
			break;
		default:
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"CheckAroundSector() - xDirection:1");
			break;
		}
		break;
	default:
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"CheckAroundSector() - xDirection:null");
		break;
	}
}

void Sector::InsertSector(SOCKETINFO* insertSocketInfo, int posXIndex, int posYIndex, int sectorFlag)
{
	// ip : insertPlayer (메인 플레이어)
	PLAYER *insertPlayer = insertSocketInfo->player;
	SOCKET ipSock = insertSocketInfo->clientSock;
	UINT64 ipSockNo = insertSocketInfo->socketNo;
	int ipUserID = 0;

	if (insertPlayer != nullptr)
	{
		ipUserID = insertPlayer->userID;
	}

	switch (sectorFlag)
	{
	case SECTORFLAG::MOVE:
		for (int yIndex = -1; yIndex <= 1; ++yIndex)
		{
			for (int xIndex = -1; xIndex <= 1; ++xIndex)
			{
				if (insertSector[yIndex + 1][xIndex + 1] == 1)
				{
					if (posXIndex + xIndex < 0 || posXIndex + xIndex > SECTOR_WIDTH_MAX - 1
						|| posYIndex + yIndex < 0 || posYIndex + yIndex > SECTOR_HEIGHT_MAX - 1)
					{
						continue;
					}

					// is : insertSector
					list<SOCKETINFO*>* isPlayerList = &playerList[posYIndex + yIndex][posXIndex + xIndex];

					list<SOCKETINFO*>::iterator isPlayerListIter;
					list<SOCKETINFO*>::iterator isPlayerListEnd = isPlayerList->end();

					for (isPlayerListIter = isPlayerList->begin();
						isPlayerListIter != isPlayerListEnd;
						++isPlayerListIter)
					{
						PLAYER* isp = (*isPlayerListIter)->player;

						if (isp == nullptr)
						{
							continue;
						}

						// isp : insertSector Player						
						int ispUserID = isp->userID;

						if (isp->userID == ipUserID)
						{
							continue;
						}

						SOCKET ispSock = (*isPlayerListIter)->clientSock;
						BYTE ispDirection = isp->direction;
						WORD ispPosX = isp->posX;
						WORD ispPosY = isp->posY;

						// ip : insertPlayer
						BYTE ipDirection = insertPlayer->direction;
						WORD ipPosX = insertPlayer->posX;
						WORD ipPosY = insertPlayer->posY;

						SendCreateOtherCharacter(ispSock, ipUserID,
							ipDirection, ipPosX, ipPosY, insertPlayer->hp, insertPlayer->leftFlag);

						SendCreateOtherCharacter(ipSock, isp->userID,
							ispDirection, ispPosX, ispPosY, isp->hp, isp->leftFlag);

						if (insertPlayer->moveFlag)
						{
							SendMoveStart(ispSock, ipDirection, ipPosX, ipPosY, &insertPlayer);
						}

						if (isp->moveFlag)
						{
							SendMoveStart(ipSock, ispDirection, ispPosX, ispPosY, &isp);
						}
					}
				}
			}
		}
		break;
	case SECTORFLAG::CONNECTION:
		for (int yIndex = -1; yIndex <= 1; ++yIndex)
		{
			for (int xIndex = -1; xIndex <= 1; ++xIndex)
			{
				if (posXIndex + xIndex < 0 || posXIndex + xIndex > SECTOR_WIDTH_MAX - 1
					|| posYIndex + yIndex < 0 || posYIndex + yIndex > SECTOR_HEIGHT_MAX - 1)
				{
					continue;
				}

				// is : insertSector
				list<SOCKETINFO*>* isPlayerList = &playerList[posYIndex + yIndex][posXIndex + xIndex];

				list<SOCKETINFO*>::iterator isPlayerListIter;
				list<SOCKETINFO*>::iterator isPlayerListEnd = isPlayerList->end();

				for (isPlayerListIter = isPlayerList->begin();
					isPlayerListIter != isPlayerListEnd;
					++isPlayerListIter)
				{
					// ip : insertPlayer
					SOCKET ipSock = insertPlayer->clientSock;
					BYTE ipDirection = insertPlayer->direction;
					WORD ipPosX = insertPlayer->posX;
					WORD ipPosY = insertPlayer->posY;

					// isp : insertSector Player (다른 플레이어)
					SOCKET ispSock = (*isPlayerListIter)->clientSock;
					PLAYER* isPlayer = (*isPlayerListIter)->player;

					if (isPlayer != nullptr)
					{
						BYTE ispDirection = isPlayer->direction;
						WORD ispPosX = isPlayer->posX;
						WORD ispPosY = isPlayer->posY;

						// 메인 플레이어에게 다른 플레이어 정보를 알려준다.
						SendCreateOtherCharacter(ipSock, isPlayer->userID,
							ispDirection, ispPosX, ispPosY, isPlayer->hp, isPlayer->leftFlag);
					}

					// 다른 플레이어에게 메인 플레이어 정보를 알려준다.
					SendCreateOtherCharacter(ispSock, ipUserID,
						ipDirection, ipPosX, ipPosY, insertPlayer->hp, insertPlayer->leftFlag);
				}
			}
		}
		break;
	default:
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"InsertSector() - sectorFlag Error [SOCKET_NO:%llu]",
			ipSockNo);
		RemoveSocketInfo(ipSock);
		break;
	}

	// ip : insertPlayer
	POINT* ipSectorIndex = &(insertSocketInfo->sectorIndex);

	ipSectorIndex->x = posXIndex;
	ipSectorIndex->y = posYIndex;

	if (insertSocketInfo != nullptr)
	{
		playerList[posYIndex][posXIndex].push_back(insertSocketInfo);
	}
	else
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR,
			L"InsertSector() - insertSocketInfo == nullptr [SOCKET_NO:%llu]", ipSockNo);
		RemoveSocketInfo(ipSock);
	}

	memset(insertSector, 0, sizeof(insertSector));

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"InsertSector() - xIndex:%d, yIndex:%d [SOCKET_NO:%llu]",
		posXIndex, posYIndex, ipSockNo);*/
}

void Sector::DeleteSector(SOCKETINFO* deleteSocketInfo, int sectorIndexX, int sectorIndexY, int sectorFlag)
{
	// dp : deletePlayer
	SOCKET dpSock = deleteSocketInfo->clientSock;
	PLAYER *deletePlayer = deleteSocketInfo->player;
	UINT64 dpSockNo = deleteSocketInfo->socketNo;

	DeadReckoning *dpDeadReckoning = nullptr;

	if (deletePlayer != nullptr)
	{
		dpDeadReckoning = &(deletePlayer->deadReckoning);
	}

	switch (sectorFlag)
	{
	case SECTORFLAG::MOVE:
		if (deletePlayer != nullptr)
		{
			for (int yIndex = -1; yIndex <= 1; ++yIndex)
			{
				for (int xIndex = -1; xIndex <= 1; ++xIndex)
				{
					if (removeSector[yIndex + 1][xIndex + 1] == 1)
					{
						if (sectorIndexX + xIndex < 0 || sectorIndexX + xIndex > SECTOR_WIDTH_MAX - 1
							|| sectorIndexY + yIndex < 0 || sectorIndexY + yIndex > SECTOR_HEIGHT_MAX - 1)
						{
							continue;
						}

						// ds : deleteSector
						list<SOCKETINFO*>* dsPlayerList =
							&playerList[sectorIndexY + yIndex][sectorIndexX + xIndex];

						list<SOCKETINFO*>::iterator dsPlayerListIter;
						list<SOCKETINFO*>::iterator dsPlayerListEnd = dsPlayerList->end();

						for (dsPlayerListIter = dsPlayerList->begin();
							dsPlayerListIter != dsPlayerListEnd;
							++dsPlayerListIter)
						{
							// dsp : deleteSector Player
							SOCKET dspSock = (*dsPlayerListIter)->clientSock;

							if (dpSock == dspSock)
							{
								continue;
							}

							// 메인 플레이어에게 다른 플레이어를 삭제하도록 한다.
							SendDeleteCharacter(dpSock, (*dsPlayerListIter)->player);

							// 다른 플레이어가 메인 플레이어를 삭제하도록 한다.
							SendDeleteCharacter(dspSock, deletePlayer);
						}
					}
				}
			}

			playerList[sectorIndexY][sectorIndexX].remove(deleteSocketInfo);

			memset(removeSector, 0, sizeof(removeSector));

			/*printLog.ODSPrint(LOGLEVEL::LOGDEBUG, L"DeleteSector() - xIndex:%d, yIndex:%d [SOCKET_NO:%llu]",
				sectorIndexX, sectorIndexY, dpSockNo);*/
		}

		break;
	case SECTORFLAG::DIE:
		if (deletePlayer != nullptr)
		{
			for (int yIndex = -1; yIndex <= 1; ++yIndex)
			{
				for (int xIndex = -1; xIndex <= 1; ++xIndex)
				{
					if (sectorIndexX + xIndex < 0 || sectorIndexX + xIndex > SECTOR_WIDTH_MAX - 1
						|| sectorIndexY + yIndex < 0 || sectorIndexY + yIndex > SECTOR_HEIGHT_MAX - 1)
					{
						continue;
					}

					// ds : deleteSector
					list<SOCKETINFO*>* dsPlayerList = &playerList[sectorIndexY + yIndex][sectorIndexX + xIndex];

					list<SOCKETINFO*>::iterator dsPlayerListIter;
					list<SOCKETINFO*>::iterator dsPlayerListEnd = dsPlayerList->end();

					for (dsPlayerListIter = dsPlayerList->begin();
						dsPlayerListIter != dsPlayerListEnd;
						++dsPlayerListIter)
					{
						// 다른 플레이어가 메인 플레이어를 삭제하도록 한다.
						SendDeleteCharacter((*dsPlayerListIter)->clientSock, deletePlayer);
					}
				}
			}

			/*printLog.ODSPrint(LOGLEVEL::LOGDEBUG, L"DeleteSector() - xIndex:%d, yIndex:%d [SOCKET_NO:%llu]",
				sectorIndexX, sectorIndexY, dpSockNo);*/

			deleteSocketInfo->player = nullptr;

			delete deletePlayer;
			deletePlayer = nullptr;
		}

		break;
	case SECTORFLAG::DISCONNECTION:
		if (deletePlayer != nullptr)
		{
			for (int yIndex = -1; yIndex <= 1; ++yIndex)
			{
				for (int xIndex = -1; xIndex <= 1; ++xIndex)
				{
					if (sectorIndexX + xIndex < 0 || sectorIndexX + xIndex > SECTOR_WIDTH_MAX - 1
						|| sectorIndexY + yIndex < 0 || sectorIndexY + yIndex > SECTOR_HEIGHT_MAX - 1)
					{
						continue;
					}

					// ds : deleteSector
					list<SOCKETINFO*>* dsPlayerList = &playerList[sectorIndexY + yIndex][sectorIndexX + xIndex];

					list<SOCKETINFO*>::iterator dsPlayerListIter;
					list<SOCKETINFO*>::iterator dsPlayerListEnd = dsPlayerList->end();

					for (dsPlayerListIter = dsPlayerList->begin();
						dsPlayerListIter != dsPlayerListEnd;
						++dsPlayerListIter)
					{
						// 다른 플레이어가 메인 플레이어를 삭제하도록 한다.
						SendDeleteCharacter((*dsPlayerListIter)->clientSock, deletePlayer);
					}
				}
			}

			/*printLog.ODSPrint(LOGLEVEL::LOGDEBUG, L"DeleteSector() - xIndex:%d, yIndex:%d [SOCKET_NO:%llu]",
				sectorIndexX, sectorIndexY, dpSockNo);*/

			delete deletePlayer;
			deletePlayer = nullptr;
		}

		playerList[sectorIndexY][sectorIndexX].remove(deleteSocketInfo);

		break;
	default:
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"DeleteSector() - sectorFlag Error [SOCKET_NO:%llu]", dpSockNo);
		RemoveSocketInfo(dpSock);
		break;
	}
}

list<SOCKETINFO*>* Sector::FindSector(int sectorIndexX, int sectorIndexY)
{
	return &playerList[sectorIndexY][sectorIndexX];
}