#include <iostream>
#include "main.h"
#include "Select.h"
#include "MakePacket.h"

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
	RingBuffer* recvQueue = &(socketInfo->recvQueue);
	
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	remainSize = recvQueue->GetRemainSize();

	if (remainSize <= 0)
	{
		return 0;
	}

	notBrokenPutSize = recvQueue->GetNotBrokenPutSize();

	recvSize = recv(sock, (char*)recvQueue->GetRearPosBufferPtr(), notBrokenPutSize, 0);
	
	if (recvSize > remainSize)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR,
			L"FDReadProc() - recvQueue size Error, Recv Size - %d [SOCKET_NO:%llu]",
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
				L"FDReadProc() - networkPacketHeader.code Error [SOCKET_NO:%llu]",
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
			printLog.ConsolePrint(LOGLEVEL::LOGERROR,
				L"FDReadProc() - EndCode Error [SOCKET_NO:%llu]", socketInfo->socketNo);

			return SOCKET_ERROR;
		}
	}
}

// Packet을 처리하는 함수
void PacketProc(SOCKET sock, WORD type, SerializationBuffer* serializationBuffer)
{
	switch (type)
	{
	case PACKETDEFINE_SC_CREATE_MY_CHARACTER:
		RecvCreateMyCharacter(sock, serializationBuffer);
		break;
	case PACKETDEFINE_SC_SYNC:
		RecvSync(sock, serializationBuffer);
		break;
	case PACKETDEFINE_SC_DELETE_CHARACTER:
	case PACKETDEFINE_SC_CREATE_OTHER_CHARACTER:
	case PACKETDEFINE_SC_MOVE_START:
	case PACKETDEFINE_SC_MOVE_STOP:
	case PACKETDEFINE_SC_ATTACK1:
	case PACKETDEFINE_SC_ATTACK2:
	case PACKETDEFINE_SC_ATTACK3:
	case PACKETDEFINE_SC_DAMAGE:
		serializationBuffer->MoveFrontPos(serializationBuffer->GetUseSize());
		break;
	default:
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"PacketProc() - Type Error [SOCKET_NO:%llu]",
			socketInfoMap.find(sock)->second->socketNo);

		--successSocket;
		++failSocket;
		RemoveSocketInfo(sock);
		break;
	}
}

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 받는 함수
void RecvCreateMyCharacter(SOCKET sock, SerializationBuffer* serializationBuffer)
{
	SOCKETINFO *mySocketInfo = socketInfoMap.find(sock)->second;
	
	DWORD ID;
	BYTE direction;
	WORD x, y;
	BYTE HP;

	*serializationBuffer >> ID >> direction >> x >> y >> HP;

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"RecvCreateMyCharacter() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		x, y, mySocketInfo->socketNo);*/

	PLAYER *myPlayer = new PLAYER;

	myPlayer->clientSock = sock;
	myPlayer->userID = ID;
	myPlayer->direction = direction;
	myPlayer->posX = x;
	myPlayer->posY = y;
	myPlayer->hp = HP;
	myPlayer->moveFlag = false;
	
	mySocketInfo->player = myPlayer;
}

// 10. 캐릭터 이동 시작 패킷을 보내는 함수
void SendMoveStart(SOCKET sock)
{
	SOCKETINFO *mySocketInfo = socketInfoMap.find(sock)->second;
	PLAYER *myPlayer = mySocketInfo->player;

	BYTE *myPlayerDirection = &(myPlayer->direction);
	WORD *myPlayerPosX = &(myPlayer->posX);
	WORD *myPlayerPosY = &(myPlayer->posY);
	
	if (myPlayer == nullptr)
	{
		return;
	}
	
	BYTE direction;
	bool reRandFlag;

	do
	{
		reRandFlag = false;
		direction = rand() % 8;

		switch (direction)
		{
		case PACKETDEFINE_MOVE_DIR_LL:
			if (*myPlayerPosX < SCREEN_WIDTH / 2)
			{
				reRandFlag = true;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_LU:
			if (*myPlayerPosX < SCREEN_WIDTH / 2 || *myPlayerPosY < SCREEN_HEIGHT / 2)
			{
				reRandFlag = true;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_UU:
			if (*myPlayerPosY < SCREEN_HEIGHT / 2)
			{
				reRandFlag = true;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_RU:
			if (*myPlayerPosX > MAP_WIDTH - SCREEN_WIDTH / 2 || *myPlayerPosY < SCREEN_HEIGHT / 2)
			{
				reRandFlag = true;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_RR:
			if (*myPlayerPosX > MAP_WIDTH - SCREEN_WIDTH / 2)
			{
				reRandFlag = true;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_RD:
			if (*myPlayerPosX > MAP_WIDTH - SCREEN_WIDTH / 2 || *myPlayerPosY > MAP_HEIGHT - SCREEN_HEIGHT / 2)
			{
				reRandFlag = true;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_DD:
			if (*myPlayerPosY > MAP_HEIGHT - SCREEN_HEIGHT / 2)
			{
				reRandFlag = true;
			}
			break;
		case PACKETDEFINE_MOVE_DIR_LD:
			if (*myPlayerPosX < SCREEN_WIDTH / 2 || *myPlayerPosY > MAP_HEIGHT - SCREEN_HEIGHT / 2)
			{
				reRandFlag = true;
			}
			break;
		}
	} while (reRandFlag);

	if (direction == myPlayer->direction)
	{
		return;
	}
		
	DeadReckoning *movePlayerDeadReckoning = &(myPlayer->deadReckoning);

	bool *moveFlag = &(myPlayer->moveFlag);

	if (*moveFlag)
	{
		movePlayerDeadReckoning->CheckPos(sock, *myPlayerDirection, myPlayerPosX, myPlayerPosY);
	}

	*myPlayerDirection = direction;
	*moveFlag = true;
	
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketMoveStart(&networkPacketHeader, &serializationBuffer, direction, *myPlayerPosX, *myPlayerPosY);

	SendToServer(mySocketInfo, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendMoveStart() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		*myPlayerPosX, *myPlayerPosY, mySocketInfo->socketNo);*/

	movePlayerDeadReckoning->Start(*myPlayerPosX, *myPlayerPosY);		
}

// 12. 캐릭터 이동 중지 패킷을 보내는 함수
void SendMoveStop(SOCKET sock)
{
	SOCKETINFO *mySocketInfo = socketInfoMap.find(sock)->second;
	PLAYER *myPlayer = mySocketInfo->player;

	if (myPlayer == nullptr)
	{
		return;
	}
	
	BYTE direction = myPlayer->direction;
	WORD *myPlayerPosX = &(myPlayer->posX);
	WORD *myPlayerPosY = &(myPlayer->posY);

	DeadReckoning *stopPlayerDeadReckoning = &(myPlayer->deadReckoning);

	bool *moveFlag = &(myPlayer->moveFlag);

	if (*moveFlag)
	{
		stopPlayerDeadReckoning->CheckPos(sock, direction, myPlayerPosX, myPlayerPosY);
	}

	*moveFlag = false;

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketMoveStop(&networkPacketHeader, &serializationBuffer, 
		direction, *myPlayerPosX, *myPlayerPosY);

	SendToServer(mySocketInfo, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendMoveStop() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		*myPlayerPosX, *myPlayerPosY, mySocketInfo->socketNo);*/
}

// 20. 캐릭터 공격 패킷을 보내는 함수
void SendAttack1(SOCKET sock)
{
	SOCKETINFO *mySocketInfo = socketInfoMap.find(sock)->second;
	PLAYER *myPlayer = mySocketInfo->player;

	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->direction;
	WORD *myPlayerPosX = &(myPlayer->posX);
	WORD *myPlayerPosY = &(myPlayer->posY);

	DeadReckoning *attackPlayerDeadReckoning = &(myPlayer->deadReckoning);

	bool *moveFlag = &(myPlayer->moveFlag);

	if (*moveFlag)
	{
		attackPlayerDeadReckoning->CheckPos(sock, direction, myPlayerPosX, myPlayerPosY);
	}

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack1(&networkPacketHeader, &serializationBuffer, direction, *myPlayerPosX, *myPlayerPosY);

	SendToServer(mySocketInfo, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendAttack1() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		*myPlayerPosX, *myPlayerPosY, mySocketInfo->socketNo);*/
}

// 22. 캐릭터 공격 패킷을 보내는 함수
void SendAttack2(SOCKET sock)
{
	SOCKETINFO *mySocketInfo = socketInfoMap.find(sock)->second;
	PLAYER *myPlayer = mySocketInfo->player;

	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->direction;
	WORD *myPlayerPosX = &(myPlayer->posX);
	WORD *myPlayerPosY = &(myPlayer->posY);

	DeadReckoning *attackPlayerDeadReckoning = &(myPlayer->deadReckoning);

	bool *moveFlag = &(myPlayer->moveFlag);

	if (*moveFlag)
	{
		attackPlayerDeadReckoning->CheckPos(sock, direction, myPlayerPosX, myPlayerPosY);
	}

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack2(&networkPacketHeader, &serializationBuffer, direction, *myPlayerPosX, *myPlayerPosY);

	SendToServer(mySocketInfo, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendAttack2() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		*myPlayerPosX, *myPlayerPosY, mySocketInfo->socketNo);*/
}

// 24. 캐릭터 공격 패킷을 보내는 함수
void SendAttack3(SOCKET sock)
{
	SOCKETINFO *mySocketInfo = socketInfoMap.find(sock)->second;
	PLAYER *myPlayer = mySocketInfo->player;
	
	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->direction;
	WORD *myPlayerPosX = &(myPlayer->posX);
	WORD *myPlayerPosY = &(myPlayer->posY);

	DeadReckoning *attackPlayerDeadReckoning = &(myPlayer->deadReckoning);

	bool *moveFlag = &(myPlayer->moveFlag);

	if (*moveFlag)
	{
		attackPlayerDeadReckoning->CheckPos(sock, direction, myPlayerPosX, myPlayerPosY);
	}

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack3(&networkPacketHeader, &serializationBuffer, direction, *myPlayerPosX, *myPlayerPosY);

	SendToServer(mySocketInfo, networkPacketHeader, &serializationBuffer);

	/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"SendAttack3() - posX:%d, posY:%d [SOCKET_NO:%llu]",
		*myPlayerPosX, *myPlayerPosY, mySocketInfo->socketNo);*/
}

// 251. 동기화를 위한 패킷을 받는 함수
void RecvSync(SOCKET sock, SerializationBuffer* serializationBuffer)
{
	SOCKETINFO *mySocketInfo = socketInfoMap.find(sock)->second;
	
	int userID;
	WORD x, y;

	*serializationBuffer >> userID >> x >> y;
	
	printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"RecvSync() posX:%d posY:%d [SOCKET_NO:%llu]",
		x, y, mySocketInfo->socketNo);
	
	PLAYER **myPlayer = &(mySocketInfo->player);

	if (*myPlayer != nullptr && (*myPlayer)->userID == userID)
	{
		(*myPlayer)->posX = x;
		(*myPlayer)->posY = y;
				
		if ((*myPlayer)->moveFlag)
		{
			(*myPlayer)->deadReckoning.SetStartPos(x, y);
		}
	}
}

// 소켓 정보를 삭제하는 함수이다.
map<SOCKET, SOCKETINFO*>::iterator RemoveSocketInfo(SOCKET sock)
{
	map<SOCKET, SOCKETINFO*>::iterator iter = socketInfoMap.find(sock);

	SOCKETINFO* iterSocketInfo = iter->second;
	
	if (iter != socketInfoMap.end())
	{
		printLog.ConsolePrint(LOGLEVEL::LOGINFO,
			L"RemoveSocketInfo() - Disconnect Socket [SOCKET_NO:%llu]",
			iterSocketInfo->socketNo);

		PLAYER *iterPlayer = iterSocketInfo->player;

		if (iterPlayer != nullptr)
		{
			delete iterPlayer;
		}
		
		delete iterSocketInfo;

		closesocket(sock);

		iter = socketInfoMap.erase(iter);
	}

	return iter;
}

// 서버로 패킷을 보내는 함수
void SendToServer(SOCKETINFO* socketInfo, NetworkPacketHeader networkPacketHeader, 
	SerializationBuffer* serializationBuffer)
{
	RingBuffer* sendQueue = &(socketInfo->sendQueue);

	sendQueue->Enqueue((BYTE*)&networkPacketHeader, sizeof(networkPacketHeader));
	sendQueue->Enqueue(serializationBuffer->GetFrontPosBufferPtr(),
		serializationBuffer->GetUseSize());
}