#include <iostream>
#include "main.h"
#include "Select.h"
#include "MakePacket.h"
#include "Sector.h"

int FDWriteProc(SOCKET sock, SOCKETINFO* socketInfo)
{
	int retval, useSize, notBrokenGetSize;
	RingBuffer* sendQueue = &(socketInfo->sendQueue);

	useSize = sendQueue->GetUseSize();
	notBrokenGetSize = sendQueue->GetNotBrokenGetSize();

	if (useSize <= 0)
	{
		return 0;
	}

	retval = send(sock, (char*)sendQueue->GetFrontPosBufferPtr(), notBrokenGetSize, 0);

	if (retval == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}
	else
	{
		sendQueue->MoveFrontPos(retval);
		socketInfo->lastSendSize = retval;
	}

	return 0;
}

int FDReadProc(SOCKET sock, SOCKETINFO* socketInfo)
{
	int recvSize, recvQueueRetval, useSize, remainSize, notBrokenPutSize;
	BYTE endCode;
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;
	RingBuffer* recvQueue = &(socketInfo->recvQueue);

	remainSize = recvQueue->GetRemainSize();

	if (remainSize <= 0)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR,
			L"FDReadProc() - recvQueue size : 0 [SOCKET_NO:%llu]", socketInfo->socketNo);

		return 0;
	}

	notBrokenPutSize = recvQueue->GetNotBrokenPutSize();

	recvSize = recv(sock, (char*)recvQueue->GetRearPosBufferPtr(), notBrokenPutSize, 0);

	if (recvSize > remainSize)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR,
			L"FDReadProc() - recvQueue size Error, Recv Size : %d [SOCKET_NO:%llu]",
			recvSize, socketInfo->socketNo);

		return SOCKET_ERROR;
	}
	else if (recvSize == SOCKET_ERROR || recvSize == 0)
	{
		return SOCKET_ERROR;
	}
	else
	{
		recvQueue->MoveRearPos(recvSize);
		socketInfo->lastRecvSize = recvSize;
	}

	while (true)
	{
		useSize = recvQueue->GetUseSize();

		if (sizeof(networkPacketHeader) > useSize)
		{
			return 0;
		}

		recvQueue->Peek((BYTE*)&networkPacketHeader, sizeof(networkPacketHeader));

		if (networkPacketHeader.code != NETWORK_PACKET_CODE)
		{
			printLog.ConsolePrint(LOGLEVEL::LOGERROR,
				L"FDReadProc() - networkPackterHeader.code Error [SOCKET_NO:%llu]",
				socketInfo->socketNo);
			return SOCKET_ERROR;
		}

		if ((int)(sizeof(networkPacketHeader) + networkPacketHeader.size + sizeof(endCode)) > useSize)
		{
			return 0;
		}

		recvQueue->MoveFrontPos(sizeof(networkPacketHeader));

		recvQueueRetval = recvQueue->Dequeue(serializationBuffer.GetRearPosBufferPtr(),
			networkPacketHeader.size);

		serializationBuffer.MoveRearPos(recvQueueRetval);

		recvQueue->Dequeue((BYTE*)&endCode, sizeof(endCode));

		if (endCode == NETWORK_PACKET_END)
		{
			PacketProc(sock, networkPacketHeader.type, &serializationBuffer);
		}
		else // endCode != NETWORK_PACKET_END
		{
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"FDReadProc() - EndCode Error [SOCKET_NO:%llu]",
				socketInfo->socketNo);

			return SOCKET_ERROR;
		}
	}
}

// Packet을 처리하는 함수
void PacketProc(SOCKET sock, WORD type, SerializationBuffer* serializationBuffer)
{
	switch (type)
	{
	case PACKETDEFINE_CS_MOVE_START:
		RecvMoveStart(sock, serializationBuffer);
		break;
	case PACKETDEFINE_CS_MOVE_STOP:
		RecvMoveStop(sock, serializationBuffer);
		break;
	case PACKETDEFINE_CS_ATTACK1:
		RecvAttack1(sock, serializationBuffer);
		break;
	case PACKETDEFINE_CS_ATTACK2:
		RecvAttack2(sock, serializationBuffer);
		break;
	case PACKETDEFINE_CS_ATTACK3:
		RecvAttack3(sock, serializationBuffer);
		break;
	default:
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"PacketProc() - Type Error [SOCKET_NO:%llu]",
			socketInfoMap.find(sock)->second->socketNo);
		RemoveSocketInfo(sock);
		break;
	}
}

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 보내는 함수
void SendCreateMyCharacter(SOCKET sock)
{
	static int newPlayerUserID = 1;
	static int playerPosX = 1;
	static int playerPosY = 1;

	BYTE newPlayerDirection = PACKETDEFINE_MOVE_DIR_LL;
	BYTE newPlayerHP = 100;
	WORD newPlayerPosX = rand() % GENERATE_WIDTH;
	WORD newPlayerPosY = rand() % GENERATE_HEIGHT;

	//// 바둑판으로 세우는 코드
	//WORD newPlayerPosX = 70 * playerPosX++;
	//WORD newPlayerPosY = 70 * playerPosY;	

	//if (playerPosX == 10)
	//{
	//	playerPosX = 1;
	//	++playerPosY;
	//}

	PLAYER *newPlayer = new PLAYER;

	newPlayer->userID = newPlayerUserID;
	newPlayer->direction = newPlayerDirection;
	newPlayer->posX = newPlayerPosX;
	newPlayer->posY = newPlayerPosY;
	newPlayer->hp = newPlayerHP;
	newPlayer->clientSock = sock;

	newPlayer->moveFlag = false;
	newPlayer->leftFlag = true;

	SOCKETINFO* createSocketInfo = socketInfoMap.find(sock)->second;

	createSocketInfo->player = newPlayer;

	sector.InsertSector(createSocketInfo, newPlayerPosX / SECTOR_WIDTH,
		newPlayerPosY / SECTOR_HEIGHT, SECTORFLAG::CONNECTION);

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketCreateMyCharacter(&networkPacketHeader, &serializationBuffer,
		newPlayerUserID, newPlayerDirection, newPlayerPosX, newPlayerPosY, newPlayerHP);

	SendUnicast(sock, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendCreateMyCharacter() - myID:%d, posX:%d, posY:%d, [SOCKET_NO:%llu]",
		newPlayerUserID, newPlayerPosX, newPlayerPosY, createSocketInfo->socketNo);*/

	++newPlayerUserID;
}

// 1. 다른 클라이언트의 캐릭터 생성 패킷을 보내는 함수
void SendCreateOtherCharacter(SOCKET sock, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp, bool leftFlag)
{
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	if (leftFlag)
	{
		MakePacketCreateOtherCharacter(&networkPacketHeader, &serializationBuffer,
			userID, PACKETDEFINE_MOVE_DIR_LL, posX, posY, hp);
	}
	else
	{
		MakePacketCreateOtherCharacter(&networkPacketHeader, &serializationBuffer,
			userID, PACKETDEFINE_MOVE_DIR_RR, posX, posY, hp);
	}

	SendUnicast(sock, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendCreateOtherCharacter() - otherID:%d, posX:%d, posY:%d [SOCKET_NO:%llu]",
		userID, posX, posY, socketInfoMap.find(sock)->second->socketNo);*/
}

// 2. 캐릭터 삭제 패킷을 보내는 함수
void SendDeleteCharacter(SOCKET sock, PLAYER* deletePlayer)
{
	if (deletePlayer != nullptr)
	{
		NetworkPacketHeader networkPacketHeader;
		SerializationBuffer serializationBuffer;

		MakePacketDeleteCharacter(&networkPacketHeader, &serializationBuffer, deletePlayer->userID);

		SendUnicast(sock, networkPacketHeader, &serializationBuffer);

		/*printLog.ODSPrint(LOGLEVEL::LOGDEBUG, L"SendDeleteCharacter() - deleteID:%d [SOCKET_NO:%llu]",
			deletePlayer->userID, socketInfoMap.find(sock)->second->socketNo);*/
	}
}

// 10. 캐릭터 이동 시작 패킷을 받는 함수
void RecvMoveStart(SOCKET sock, SerializationBuffer* serializationBuffer)
{
	BYTE direction;
	WORD posX, posY;

	*serializationBuffer >> direction >> posX >> posY;

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"RecvMoveStart() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		posX, posY, socketInfoMap.find(sock)->second->socketNo);*/

	SendMoveStart(sock, direction, posX, posY);
}

// 11. 캐릭터 이동 시작 패킷을 보내는 함수
void SendMoveStart(SOCKET sock, BYTE direction, WORD posX, WORD posY, PLAYER **movePlayer)
{
	SOCKETINFO* socketInfo = socketInfoMap.find(sock)->second;
	UINT64 socketNo = socketInfo->socketNo;

	bool myPlayer = false;

	if (movePlayer == nullptr)
	{
		movePlayer = &(socketInfo->player);
		myPlayer = true;
	}

	if (*movePlayer == nullptr)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGWARNING, L"SendMovePlayer() - Cannot Find Player [SOCKET_NO:%llu]",
			socketNo);
		return;
	}

	DeadReckoning *movePlayerDeadReckoning = &((*movePlayer)->deadReckoning);
	bool *moveFlag = &((*movePlayer)->moveFlag);

	if (myPlayer && *moveFlag)
	{
		movePlayerDeadReckoning->CheckPos(sock, direction, &posX, &posY);
	}

	(*movePlayer)->direction = direction;
	(*movePlayer)->posX = posX;
	(*movePlayer)->posY = posY;
	*moveFlag = true;

	switch (direction)
	{
	case PACKETDEFINE_MOVE_DIR_LU:
	case PACKETDEFINE_MOVE_DIR_LL:
	case PACKETDEFINE_MOVE_DIR_LD:
		(*movePlayer)->leftFlag = true;
		break;
	case PACKETDEFINE_MOVE_DIR_RU:
	case PACKETDEFINE_MOVE_DIR_RR:
	case PACKETDEFINE_MOVE_DIR_RD:
		(*movePlayer)->leftFlag = false;
		break;
	}

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketMoveStart(&networkPacketHeader, &serializationBuffer,
		(*movePlayer)->userID, direction, posX, posY);

	if (myPlayer)
	{
		SendAroundSector(sock, networkPacketHeader, &serializationBuffer, sock);
		movePlayerDeadReckoning->Start(posX, posY);
	}
	else
	{
		SendUnicast(sock, networkPacketHeader, &serializationBuffer);
	}

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendMoveStart() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		posX, posY, socketNo);*/
}

// 12. 캐릭터 이동 중지 패킷을 받는 함수
void RecvMoveStop(SOCKET sock, SerializationBuffer* serializationBuffer)
{
	BYTE direction;
	WORD posX, posY;

	*serializationBuffer >> direction >> posX >> posY;

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"RecvMoveStop() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		posX, posY, socketInfoMap.find(sock)->second->socketNo);*/

	SendMoveStop(sock, direction, posX, posY);
}

// 13. 캐릭터 이동 중지 패킷을 보내는 함수
void SendMoveStop(SOCKET sock, BYTE direction, WORD posX, WORD posY, PLAYER **stopPlayer)
{
	SOCKETINFO* socketInfo = socketInfoMap.find(sock)->second;
	UINT64 socketNo = socketInfo->socketNo;

	if (stopPlayer == nullptr)
	{
		stopPlayer = &(socketInfo->player);
	}

	if (*stopPlayer == nullptr)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGWARNING, L"SendMoveStop() - Cannot Find Player [SOCKET_NO:%llu]",
			socketNo);
		return;
	}

	bool *moveFlag = &((*stopPlayer)->moveFlag);

	if (*moveFlag)
	{
		((*stopPlayer)->deadReckoning).CheckPos(sock, direction, &posX, &posY);
	}

	(*stopPlayer)->direction = direction;
	(*stopPlayer)->posX = posX;
	(*stopPlayer)->posY = posY;
	*moveFlag = false;

	switch (direction)
	{
	case PACKETDEFINE_MOVE_DIR_LU:
	case PACKETDEFINE_MOVE_DIR_LL:
	case PACKETDEFINE_MOVE_DIR_LD:
		(*stopPlayer)->leftFlag = true;
		break;
	case PACKETDEFINE_MOVE_DIR_RU:
	case PACKETDEFINE_MOVE_DIR_RR:
	case PACKETDEFINE_MOVE_DIR_RD:
		(*stopPlayer)->leftFlag = false;
		break;
	}

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketMoveStop(&networkPacketHeader, &serializationBuffer,
		(*stopPlayer)->userID, direction, posX, posY);

	SendAroundSector(sock, networkPacketHeader, &serializationBuffer, sock);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendMoveStop() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		posX, posY, socketNo);*/
}

// 20. 캐릭터 공격 패킷을 받는 함수
void RecvAttack1(SOCKET sock, SerializationBuffer* serializationBuffer)
{
	BYTE attackDirection;
	WORD attackX, attackY;

	*serializationBuffer >> attackDirection >> attackX >> attackY;

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"RecvAttack1() - direction:%d posX:%d posY:%d [SOCKET_NO:%llu]",
		attackDirection, attackX, attackY, socketInfoMap.find(sock)->second->socketNo);*/

	SendAttack1(sock, attackDirection, attackX, attackY);
}

// 21. 캐릭터 공격 패킷을 보내는 함수
void SendAttack1(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY)
{
	SOCKETINFO* socketInfo = socketInfoMap.find(sock)->second;
	PLAYER **attackPlayer = &(socketInfo->player);
	UINT64 socketNo = socketInfo->socketNo;

	if (*attackPlayer == nullptr)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGWARNING, L"SendAttack1() - Cannot Find Player [SOCKET_NO:%llu]",
			socketNo);
		return;
	}

	bool *moveFlag = &((*attackPlayer)->moveFlag);

	if (*moveFlag)
	{
		((*attackPlayer)->deadReckoning).CheckPos(sock, attackDirection, &attackX, &attackY);
	}

	int attackUserID = (*attackPlayer)->userID;
	(*attackPlayer)->posX = attackX;
	(*attackPlayer)->posY = attackY;
	*moveFlag = false;

	switch (attackDirection)
	{
	case PACKETDEFINE_MOVE_DIR_LU:
	case PACKETDEFINE_MOVE_DIR_LL:
	case PACKETDEFINE_MOVE_DIR_LD:
		attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		(*attackPlayer)->leftFlag = true;
		break;
	case PACKETDEFINE_MOVE_DIR_RU:
	case PACKETDEFINE_MOVE_DIR_RR:
	case PACKETDEFINE_MOVE_DIR_RD:
		attackDirection = PACKETDEFINE_MOVE_DIR_RR;
		(*attackPlayer)->leftFlag = false;
		break;
	case PACKETDEFINE_MOVE_DIR_UU:
	case PACKETDEFINE_MOVE_DIR_DD:
		if ((*attackPlayer)->leftFlag)
		{
			attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		}
		else
		{
			attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		}
		break;
	}

	(*attackPlayer)->direction = attackDirection;

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack1(&networkPacketHeader, &serializationBuffer,
		attackUserID, attackDirection, attackX, attackY);

	SendAroundSector(sock, networkPacketHeader, &serializationBuffer, sock);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendAttack1() - direction:%d posX:%d, posY:%d [SOCKET_NO:%llu]",
		attackDirection, attackX, attackY, socketNo);*/

	int sectorIndexX = socketInfo->sectorIndex.x;
	int sectorIndexY = socketInfo->sectorIndex.y;

	for (int yIndex = sectorIndexY - 1; yIndex <= sectorIndexY + 1; ++yIndex)
	{
		for (int xIndex = sectorIndexX - 1; xIndex <= sectorIndexX + 1; ++xIndex)
		{
			if (xIndex < 0 || xIndex > SECTOR_WIDTH_MAX - 1
				|| yIndex < 0 || yIndex > SECTOR_HEIGHT_MAX - 1)
			{
				continue;
			}

			list<SOCKETINFO*> *playerList = sector.FindSector(xIndex, yIndex);

			list<SOCKETINFO*>::iterator playerListIter;
			list<SOCKETINFO*>::iterator playerListIterEnd = playerList->end();

			for (playerListIter = playerList->begin();
				playerListIter != playerListIterEnd;
				++playerListIter)
			{
				PLAYER *iterPlayer = (*playerListIter)->player;
				SOCKET iterSock = (*playerListIter)->clientSock;

				if (iterPlayer == nullptr || iterPlayer->userID == (*attackPlayer)->userID)
				{
					continue;
				}

				int iterPosX = iterPlayer->posX;
				int iterPosY = iterPlayer->posY;

				// 오른쪽 공격
				if (attackDirection == PACKETDEFINE_MOVE_DIR_RR)
				{
					if ((iterPosX >= attackX && iterPosX <= attackX + ATTACK1_RANGE_X)
						&& (iterPosY >= attackY - ATTACK1_RANGE_Y / 2 && iterPosY <= attackY + ATTACK1_RANGE_Y / 2))
					{
						int damageHP = iterPlayer->hp - ATTACK1_DAMAGE;

						if (damageHP < 0)
						{
							damageHP = 0;
						}

						/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"damageX:%d damageY:%d [SOCKET_NO:%llu]",
							iterPosX, iterPosY, socketNo);*/
						SendDamage(iterSock, attackUserID, iterPlayer->userID, damageHP);
					}
				}
				// 왼쪽 공격
				else if (attackDirection == PACKETDEFINE_MOVE_DIR_LL)
				{
					if ((iterPosX >= attackX - ATTACK1_RANGE_X && iterPosX <= attackX)
						&& (iterPosY >= attackY - ATTACK1_RANGE_Y / 2 && iterPosY <= attackY + ATTACK1_RANGE_Y / 2))
					{
						int damageHP = iterPlayer->hp - ATTACK1_DAMAGE;

						if (damageHP < 0)
						{
							damageHP = 0;
						}

						/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"damageX:%d damageY:%d [SOCKET_NO:%llu]",
							iterPosX, iterPosY, socketNo);*/
						SendDamage(iterSock, attackUserID, iterPlayer->userID, damageHP);
					}
				}
			}
		}
	}
}

// 22. 캐릭터 공격 패킷을 받는 함수
void RecvAttack2(SOCKET sock, SerializationBuffer* serializationBuffer)
{
	BYTE attackDirection;
	WORD attackX, attackY;

	*serializationBuffer >> attackDirection >> attackX >> attackY;

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"RecvAttack2() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		attackX, attackY, socketInfoMap.find(sock)->second->socketNo);*/

	SendAttack2(sock, attackDirection, attackX, attackY);
}

// 23. 캐릭터 공격 패킷을 보내는 함수
void SendAttack2(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY)
{
	SOCKETINFO* socketInfo = socketInfoMap.find(sock)->second;
	PLAYER **attackPlayer = &(socketInfo->player);

	if (*attackPlayer == nullptr)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGWARNING, L"SendAttack2 - Cannot Find Player [SOCKET_NO:%llu]",
			socketInfo->socketNo);
		return;
	}

	bool *moveFlag = &((*attackPlayer)->moveFlag);

	if (*moveFlag)
	{
		((*attackPlayer)->deadReckoning).CheckPos(sock, attackDirection, &attackX, &attackY);
	}

	int attackUserID = (*attackPlayer)->userID;
	(*attackPlayer)->posX = attackX;
	(*attackPlayer)->posY = attackY;
	*moveFlag = false;

	switch (attackDirection)
	{
	case PACKETDEFINE_MOVE_DIR_LU:
	case PACKETDEFINE_MOVE_DIR_LL:
	case PACKETDEFINE_MOVE_DIR_LD:
		attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		(*attackPlayer)->leftFlag = true;
		break;
	case PACKETDEFINE_MOVE_DIR_RU:
	case PACKETDEFINE_MOVE_DIR_RR:
	case PACKETDEFINE_MOVE_DIR_RD:
		attackDirection = PACKETDEFINE_MOVE_DIR_RR;
		(*attackPlayer)->leftFlag = false;
		break;
	case PACKETDEFINE_MOVE_DIR_UU:
	case PACKETDEFINE_MOVE_DIR_DD:
		if ((*attackPlayer)->leftFlag)
		{
			attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		}
		else
		{
			attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		}
		break;
	}

	(*attackPlayer)->direction = attackDirection;

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack2(&networkPacketHeader, &serializationBuffer,
		attackUserID, attackDirection, attackX, attackY);

	SendAroundSector(sock, networkPacketHeader, &serializationBuffer, sock);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendAttack2() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		attackX, attackY, socketInfo->socketNo);*/

	int sectorIndexX = socketInfo->sectorIndex.x;
	int sectorIndexY = socketInfo->sectorIndex.y;

	for (int yIndex = sectorIndexY - 1; yIndex <= sectorIndexY + 1; ++yIndex)
	{
		for (int xIndex = sectorIndexX - 1; xIndex <= sectorIndexX + 1; ++xIndex)
		{
			if (xIndex < 0 || xIndex > SECTOR_WIDTH_MAX - 1
				|| yIndex < 0 || yIndex > SECTOR_HEIGHT_MAX - 1)
			{
				continue;
			}

			list<SOCKETINFO*> *playerList = sector.FindSector(xIndex, yIndex);

			list<SOCKETINFO*>::iterator playerListIter;
			list<SOCKETINFO*>::iterator playerListIterEnd = playerList->end();

			for (playerListIter = playerList->begin();
				playerListIter != playerListIterEnd;
				++playerListIter)
			{
				PLAYER *iterPlayer = (*playerListIter)->player;
				SOCKET iterSock = (*playerListIter)->clientSock;

				if (iterPlayer == nullptr || iterPlayer->userID == (*attackPlayer)->userID)
				{
					continue;
				}

				int iterPosX = iterPlayer->posX;
				int iterPosY = iterPlayer->posY;

				// 오른쪽 공격
				if (attackDirection == PACKETDEFINE_MOVE_DIR_RR)
				{
					if ((iterPosX >= attackX && iterPosX <= attackX + ATTACK2_RANGE_X)
						&& (iterPosY >= attackY - ATTACK2_RANGE_Y / 2 && iterPosY <= attackY + ATTACK2_RANGE_Y / 2))
					{
						int damageHP = iterPlayer->hp - ATTACK2_DAMAGE;

						if (damageHP < 0)
						{
							damageHP = 0;
						}

						SendDamage(iterSock, attackUserID, iterPlayer->userID, damageHP);
					}
				}
				// 왼쪽 공격
				else if (attackDirection == PACKETDEFINE_MOVE_DIR_LL)
				{
					if ((iterPosX >= attackX - ATTACK2_RANGE_X && iterPosX <= attackX)
						&& (iterPosY >= attackY - ATTACK2_RANGE_Y / 2 && iterPosY <= attackY + ATTACK2_RANGE_Y / 2))
					{
						int damageHP = iterPlayer->hp - ATTACK2_DAMAGE;

						if (damageHP < 0)
						{
							damageHP = 0;
						}

						SendDamage(iterSock, attackUserID, iterPlayer->userID, damageHP);
					}
				}
			}
		}
	}
}

// 24. 캐릭터 공격 패킷을 받는 함수
void RecvAttack3(SOCKET sock, SerializationBuffer* serializationBuffer)
{
	BYTE attackDirection;
	WORD attackX, attackY;

	*serializationBuffer >> attackDirection >> attackX >> attackY;

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"RecvAttack3() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		attackX, attackY, socketInfoMap.find(sock)->second->socketNo);*/

	SendAttack3(sock, attackDirection, attackX, attackY);
}

// 25. 캐릭터 공격 패킷을 보내는 함수
void SendAttack3(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY)
{
	SOCKETINFO* socketInfo = socketInfoMap.find(sock)->second;
	PLAYER **attackPlayer = &(socketInfo->player);

	if (*attackPlayer == nullptr)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGWARNING, L"SendAttack3() - Cannot Find Player [SOCKET_NO:%llu]",
			socketInfo->socketNo);
		return;
	}

	bool *moveFlag = &((*attackPlayer)->moveFlag);

	if (*moveFlag)
	{
		((*attackPlayer)->deadReckoning).CheckPos(sock, attackDirection, &attackX, &attackY);
	}

	int attackUserID = (*attackPlayer)->userID;
	(*attackPlayer)->posX = attackX;
	(*attackPlayer)->posY = attackY;
	*moveFlag = false;

	switch (attackDirection)
	{
	case PACKETDEFINE_MOVE_DIR_LU:
	case PACKETDEFINE_MOVE_DIR_LL:
	case PACKETDEFINE_MOVE_DIR_LD:
		attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		(*attackPlayer)->leftFlag = true;
		break;
	case PACKETDEFINE_MOVE_DIR_RU:
	case PACKETDEFINE_MOVE_DIR_RR:
	case PACKETDEFINE_MOVE_DIR_RD:
		attackDirection = PACKETDEFINE_MOVE_DIR_RR;
		(*attackPlayer)->leftFlag = false;
		break;
	case PACKETDEFINE_MOVE_DIR_UU:
	case PACKETDEFINE_MOVE_DIR_DD:
		if ((*attackPlayer)->leftFlag)
		{
			attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		}
		else
		{
			attackDirection = PACKETDEFINE_MOVE_DIR_LL;
		}
		break;
	}

	(*attackPlayer)->direction = attackDirection;

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack3(&networkPacketHeader, &serializationBuffer,
		attackUserID, attackDirection, attackX, attackY);

	SendAroundSector(sock, networkPacketHeader, &serializationBuffer, sock);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendAttack3() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		attackX, attackY, socketInfo->socketNo);*/

	int sectorIndexX = socketInfo->sectorIndex.x;
	int sectorIndexY = socketInfo->sectorIndex.y;

	for (int yIndex = sectorIndexY - 1; yIndex <= sectorIndexY + 1; ++yIndex)
	{
		for (int xIndex = sectorIndexX - 1; xIndex <= sectorIndexX + 1; ++xIndex)
		{
			if (xIndex < 0 || xIndex > SECTOR_WIDTH_MAX - 1
				|| yIndex < 0 || yIndex > SECTOR_HEIGHT_MAX - 1)
			{
				continue;
			}

			list<SOCKETINFO*> *playerList = sector.FindSector(xIndex, yIndex);

			list<SOCKETINFO*>::iterator playerListIter;
			list<SOCKETINFO*>::iterator playerListIterEnd = playerList->end();

			for (playerListIter = playerList->begin();
				playerListIter != playerListIterEnd;
				++playerListIter)
			{
				PLAYER *iterPlayer = (*playerListIter)->player;
				SOCKET iterSock = (*playerListIter)->clientSock;

				if (iterPlayer == nullptr || iterPlayer->userID == (*attackPlayer)->userID)
				{
					continue;
				}

				int iterPosX = iterPlayer->posX;
				int iterPosY = iterPlayer->posY;

				// 오른쪽 공격
				if (attackDirection == PACKETDEFINE_MOVE_DIR_RR)
				{
					if ((iterPosX >= attackX && iterPosX <= attackX + ATTACK3_RANGE_X)
						&& (iterPosY >= attackY - ATTACK3_RANGE_Y / 2 && iterPosY <= attackY + ATTACK3_RANGE_Y / 2))
					{
						int damageHP = iterPlayer->hp - ATTACK3_DAMAGE;

						if (damageHP < 0)
						{
							damageHP = 0;
						}

						SendDamage(iterSock, attackUserID, iterPlayer->userID, damageHP);
					}
				}
				// 왼쪽 공격
				else if (attackDirection == PACKETDEFINE_MOVE_DIR_LL)
				{
					if ((iterPosX >= attackX - ATTACK3_RANGE_X && iterPosX <= attackX)
						&& (iterPosY >= attackY - ATTACK3_RANGE_Y / 2 && iterPosY <= attackY + ATTACK3_RANGE_Y / 2))
					{
						int damageHP = iterPlayer->hp - ATTACK3_DAMAGE;

						if (damageHP < 0)
						{
							damageHP = 0;
						}

						SendDamage(iterSock, attackUserID, iterPlayer->userID, damageHP);
					}
				}
			}
		}
	}
}

// 30. 캐릭터 데미지 패킷을 보내는 함수
void SendDamage(SOCKET sock, int attackID, int damageID, BYTE damageHP)
{
	SOCKETINFO* damageSocketInfo = socketInfoMap.find(sock)->second;
	PLAYER* damagePlayer = damageSocketInfo->player;

	// dsi : damageSocketInfo
	POINT* dsiSectorIndex = &(damageSocketInfo->sectorIndex);

	BYTE *damagePlayerHP = &(damagePlayer->hp);
	*damagePlayerHP = damageHP;

	if (*damagePlayerHP <= 0)
	{
		*damagePlayerHP += 100;
	}

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketDamage(&networkPacketHeader, &serializationBuffer, attackID, damageID, damageHP);

	SendAroundSector(sock, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendDamage() - attackID:%d, damageID:%d [SOCKET_NO:%llu]",
		attackID, damageID, damageSocketInfo->socketNo);*/

	/*if (*damagePlayerHP <= 0)
	{
	sector.DeleteSector(damageSocketInfo, dsiSectorIndex->x,
	dsiSectorIndex->y, SECTORFLAG::DIE);
	}
	else
	{
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketDamage(&networkPacketHeader, &serializationBuffer, attackID, damageID, damageHP);

	printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendDamage() - [SOCKET_NO:%llu] (AroundSector)",
	socketInfoMap.find(sock)->second->socketNo);

	SendAroundSector(sock, networkPacketHeader, &serializationBuffer);
	}*/
}

// 251. 동기화를 위한 패킷을 보내는 함수
void SendSync(SOCKET sock, int userID, WORD posX, WORD posY)
{
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketSync(&networkPacketHeader, &serializationBuffer, userID, posX, posY);

	SendUnicast(sock, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendSync() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		posX, posY, socketInfoMap.find(sock)->second->socketNo);*/
}

// 소켓 정보를 추가하는 함수이다.
bool AddSocketInfo(SOCKET sock)
{
	static int sockNum = 1;

	if (totalSockets > SOCKETINFO_ARRAY_MAX)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"AddSocketInfo() - Already Full Clients");
		return false;
	}

	SOCKETINFO* ptr = new SOCKETINFO;
	UINT64 *socketNo = &(ptr->socketNo);

	ptr->clientSock = sock;
	*socketNo = sockNum++;

	printLog.ConsolePrint(LOGLEVEL::LOGINFO, L"AddSocketInfo() - Connect Socket [SOCKET_NO:%llu]", *socketNo);

	socketInfoMap.insert(map<SOCKET, SOCKETINFO*>::value_type(sock, ptr));
	//socketInfoMap[sock] = ptr; // -> 메모리를 추가 할당하기 때문에 사용하지 말 것.

	++totalSockets;

	return true;
}

// 소켓 정보를 삭제하는 함수이다.
map<SOCKET, SOCKETINFO*>::iterator RemoveSocketInfo(SOCKET sock)
{
	map<SOCKET, SOCKETINFO*>::iterator iter = socketInfoMap.find(sock);

	SOCKETINFO* iterSocketInfo = iter->second;

	if (iter != socketInfoMap.end())
	{
		printLog.ConsolePrint(LOGLEVEL::LOGINFO, L"RemoveSocketInfo() - Disconnect Socket [SOCKET_NO:%llu]",
			iterSocketInfo->socketNo);

		// isi : iterSocketInfo
		POINT* isiSectorIndex = &(iterSocketInfo->sectorIndex);

		sector.DeleteSector(iterSocketInfo, isiSectorIndex->x,
			isiSectorIndex->y, SECTORFLAG::DISCONNECTION);

		delete iterSocketInfo;

		closesocket(sock);

		iter = socketInfoMap.erase(iter);
		--totalSockets;
	}

	return iter;
}

// 그 사람에게만 송신하는 함수
void SendUnicast(SOCKET sock, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer)
{
	RingBuffer *sendQueue = &(socketInfoMap.find(sock)->second->sendQueue);

	sendQueue->Enqueue((BYTE*)&networkPacketHeader, sizeof(networkPacketHeader));
	sendQueue->Enqueue(serializationBuffer->GetFrontPosBufferPtr(),
		serializationBuffer->GetUseSize());
}

// 그 사람에게만 송신하는 함수
void SendUnicast(SOCKETINFO* socketInfo, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer)
{
	RingBuffer *sendQueue = &(socketInfo->sendQueue);

	sendQueue->Enqueue((BYTE*)&networkPacketHeader, sizeof(networkPacketHeader));
	sendQueue->Enqueue(serializationBuffer->GetFrontPosBufferPtr(),
		serializationBuffer->GetUseSize());
}

// 그 사람의 섹터에 송신하는 함수
void SendOneSector(SOCKET mainSock, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock)
{
	// msp : mainSockPlayer
	POINT* mspSectorIndex = &(socketInfoMap.find(mainSock)->second->sectorIndex);

	list<SOCKETINFO*>* mspSectorList = sector.FindSector(mspSectorIndex->x, mspSectorIndex->y);

	list<SOCKETINFO*>::iterator mspSectorListIter;
	list<SOCKETINFO*>::iterator mspSectorListIterEnd = mspSectorList->end();

	for (mspSectorListIter = mspSectorList->begin();
		mspSectorListIter != mspSectorListIterEnd;
		++mspSectorListIter)
	{
		SOCKET clientSock = (*mspSectorListIter)->clientSock;

		if (exceptSock != 0)
		{
			if (clientSock != exceptSock)
			{
				SendUnicast(clientSock, networkPacketHeader, serializationBuffer);
			}
		}
		else // exceptSocket == 0
		{
			SendUnicast(clientSock, networkPacketHeader, serializationBuffer);
		}
	}
}

// 그 사람의 섹터 및 주변 섹터에 송신하는 함수
void SendAroundSector(SOCKET mainSock, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock)
{
	// msp : mainSockPlayer
	POINT* mspSectorIndex = &(socketInfoMap.find(mainSock)->second->sectorIndex);

	int mspSectorIndexX = mspSectorIndex->x;
	int mspSectorIndexY = mspSectorIndex->y;

	for (int yIndex = mspSectorIndexY - 1; yIndex <= mspSectorIndexY + 1; ++yIndex)
	{
		for (int xIndex = mspSectorIndexX - 1; xIndex <= mspSectorIndexX + 1; ++xIndex)
		{
			if (xIndex < 0 || xIndex > SECTOR_WIDTH_MAX - 1
				|| yIndex < 0 || yIndex > SECTOR_HEIGHT_MAX - 1)
			{
				continue;
			}

			list<SOCKETINFO*>* mspSectorList = sector.FindSector(xIndex, yIndex);

			list<SOCKETINFO*>::iterator mspSectorListIter;
			list<SOCKETINFO*>::iterator mspSectorListIterEnd = mspSectorList->end();

			for (mspSectorListIter = mspSectorList->begin();
				mspSectorListIter != mspSectorListIterEnd;
				++mspSectorListIter)
			{
				SOCKET clientSock = (*mspSectorListIter)->clientSock;

				if (exceptSock != 0)
				{
					if (clientSock != exceptSock)
					{
						SendUnicast(clientSock, networkPacketHeader, serializationBuffer);
					}
				}
				else // exceptSocket == 0
				{
					SendUnicast(clientSock, networkPacketHeader, serializationBuffer);
				}
			}
		}
	}
}

// 나를 제외한 모든 사람들에게 송신하는 함수
void SendBroadcast(NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock)
{
	map<SOCKET, SOCKETINFO*>::iterator iter;
	map<SOCKET, SOCKETINFO*>::iterator iterEnd = socketInfoMap.end();

	for (iter = socketInfoMap.begin(); iter != iterEnd; ++iter)
	{
		if (exceptSock != 0 && iter->first != exceptSock)
		{
			SendUnicast(iter->second, networkPacketHeader, serializationBuffer);
		}
	}
}