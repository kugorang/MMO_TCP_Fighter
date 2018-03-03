#include "stdafx.h"
#include "0909_homework_MMOTCPFighterClient.h"
#include "WSAAsyncSelect.h"
#include "Player.h"
#include "MakePacket.h"
#include "Map.h"

// 소켓 관련 윈도우 메시지 처리
void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int retval;

	// 오류 발생 여부 확인
	if (WSAGETSELECTERROR(lParam))
	{
		ErrorQuit(L"WSAGETSELECTERROR(lParam)");
		exit(6001);
		//PostQuitMessage(0);
	}

	// 메세지 처리
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
		retval = WSAAsyncSelect((SOCKET)wParam, hWnd, WM_SOCKET,
			FD_READ | FD_WRITE | FD_CLOSE);

		if (retval == SOCKET_ERROR)
		{
			ErrorQuit(L"WSAAsyncSelect()");
			exit(6002);
			//PostQuitMessage(0);
		}
		break;
	case FD_READ:
		FDReadProc();
		break;
	case FD_WRITE:
		FDWriteProc();
		break;
	case FD_CLOSE:
		closesocket(linkSock);
		exit(6003);
		//PostQuitMessage(0);
		break;
	default:
		break;
	}
}

void FDWriteProc()
{
	int retval, useSize, notBrokenGetSize;

	while (true)
	{
		useSize = sendQueue->GetUseSize();
		notBrokenGetSize = sendQueue->GetNotBrokenGetSize();

		if (useSize <= 0)
		{
			return;
		}
		
 		retval = send(linkSock, (char*)sendQueue->GetFrontPosBufferPtr(), notBrokenGetSize, 0);

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				ErrorQuit(L"FDWriteProc()");
				exit(6004);
				//PostQuitMessage(0);
			}
		}
		else
		{
			sendQueue->MoveFrontPos(retval);
		}
	}
}

void FDReadProc()
{
	int retval, useSize, remainSize, notBrokenPutSize;
	NetworkPacketHeader networkPacketHeader;
	BYTE endCode;
	SerializationBuffer serializationBuffer;

	remainSize = recvQueue->GetRemainSize();
	notBrokenPutSize = recvQueue->GetNotBrokenPutSize();

	if (remainSize <= 0)
	{
		return;
	}
		
	retval = recv(linkSock, (char*)recvQueue->GetRearPosBufferPtr(), notBrokenPutSize, 0);

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			ErrorQuit(L"FDReadProc()");
			exit(6005);
			//PostQuitMessage(0);
		}
	}
	else if (retval == 0)
	{
		return;
	}
	else
	{
		recvQueue->MoveRearPos(retval);
	}

	while (true)
	{
		useSize = recvQueue->GetUseSize();

		if (sizeof(networkPacketHeader) >= useSize)
		{
			return;
		}

		recvQueue->Peek((BYTE*)&networkPacketHeader, sizeof(networkPacketHeader));

		if ((int)(sizeof(networkPacketHeader) + networkPacketHeader.size + sizeof(endCode)) > useSize
			|| networkPacketHeader.code != NETWORK_PACKET_CODE)
		{
			return;
		}

		recvQueue->MoveFrontPos(sizeof(networkPacketHeader));
		
		retval = recvQueue->Dequeue(serializationBuffer.GetRearPosBufferPtr(), 
			networkPacketHeader.size);

		recvQueue->Dequeue(&endCode, sizeof(endCode));

		serializationBuffer.MoveRearPos(retval);

		if (endCode == NETWORK_PACKET_END)
		{
			PacketProc(networkPacketHeader.type, &serializationBuffer);
		}
	}
}

void PacketProc(BYTE type, SerializationBuffer* serializationBuffer)
{
	switch (type)
	{
	case PACKETDEFINE_SC_CREATE_MY_CHARACTER:
		RecvCreateMyCharacter(serializationBuffer);
		break;
	case PACKETDEFINE_SC_CREATE_OTHER_CHARACTER:
		RecvCreateOtherCharacter(serializationBuffer);
		break;
	case PACKETDEFINE_SC_DELETE_CHARACTER:
		RecvDeleteCharacter(serializationBuffer);
		break;
	case PACKETDEFINE_SC_MOVE_START:
		RecvMoveStart(serializationBuffer);
		break;
	case PACKETDEFINE_SC_MOVE_STOP:
		RecvMoveStop(serializationBuffer);
		break;
	case PACKETDEFINE_SC_ATTACK1:
		RecvAttack1(serializationBuffer);
		break;
	case PACKETDEFINE_SC_ATTACK2:
		RecvAttack2(serializationBuffer);
		break;
	case PACKETDEFINE_SC_ATTACK3:
		RecvAttack3(serializationBuffer);
		break;
	case PACKETDEFINE_SC_DAMAGE:
		RecvDamage(serializationBuffer);
		break;
	case PACKETDEFINE_SC_SYNC:
		RecvSync(serializationBuffer);
		break;
	default:
		wprintf(L"메세지 타입 에러 - ");
		closesocket(linkSock);
		PostQuitMessage(0);
		break;
	}
}

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 받는 함수
void RecvCreateMyCharacter(SerializationBuffer* serializationBuffer)
{
	DWORD ID;
	BYTE direction;
	WORD x, y;
	BYTE HP;
	
	*serializationBuffer >> ID >> direction >> x >> y >> HP;
	
	myPlayer = new Player(ID, direction, x, y, HP);
	objectList.push_back(myPlayer);

	int scrollX = x - SCREEN_WIDTH / 2;
	int scrollY = y - SCREEN_HEIGHT / 2 - 25;

	tileMap->SetScrollX(scrollX);
	tileMap->SetScrollY(scrollY);
}

// 1. 다른 클라이언트의 캐릭터 생성 패킷을 받는 함수
void RecvCreateOtherCharacter(SerializationBuffer* serializationBuffer)
{
	DWORD ID;
	BYTE direction;
	WORD x, y;
	BYTE HP;

	*serializationBuffer >> ID >> direction >> x >> y >> HP;

	Player* tmpPlayer = new Player(ID, direction, x, y, HP);
	objectList.push_back(tmpPlayer);
}

// 2. 캐릭터 삭제 패킷을 받는 함수
void RecvDeleteCharacter(SerializationBuffer* serializationBuffer)
{
	DWORD ID;

	*serializationBuffer >> ID;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == ID)
		{
			if (myPlayer != nullptr && ID == myPlayer->GetUserID())
			{
				myPlayer = nullptr;
			}

			delete *objectListIter;
			objectList.erase(objectListIter);

			break;
		}
	}
}

// 10. 캐릭터 이동 시작 패킷을 보내는 함수
void SendMoveStart()
{
	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->GetDirection();
	WORD x = myPlayer->GetXPos();
	WORD y = myPlayer->GetYPos();

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketMoveStart(&networkPacketHeader, &serializationBuffer, direction, x, y);

	SendToServer(networkPacketHeader, &serializationBuffer);

	FDWriteProc();
}

// 11. 캐릭터 이동 시작 패킷을 받는 함수
void RecvMoveStart(SerializationBuffer* serializationBuffer)
{
	DWORD ID;
	BYTE direction;
	WORD x, y;
	
	*serializationBuffer >> ID >> direction >> x >> y;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == ID)
		{
			Player* startPlayer = (Player*)(*objectListIter);
			startPlayer->SetActionStatus(direction);
			startPlayer->SetPosition(x, y);
			startPlayer->SetDirection(direction);

			break;
		}
	}
}

// 12. 캐릭터 이동 중지 패킷을 보내는 함수
void SendMoveStop()
{
	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->GetAnimDirection();
	WORD x = myPlayer->GetXPos();
	WORD y = myPlayer->GetYPos();
		
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketMoveStop(&networkPacketHeader, &serializationBuffer, direction, x, y);

	SendToServer(networkPacketHeader, &serializationBuffer);

	FDWriteProc();
}

// 13. 캐릭터 이동 중지 패킷을 받는 함수
void RecvMoveStop(SerializationBuffer* serializationBuffer)
{
	DWORD ID;
	BYTE direction;
	WORD x, y;

	*serializationBuffer >> ID >> direction >> x >> y;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == ID)
		{
			Player* stopPlayer = (Player*)(*objectListIter);
			stopPlayer->SetActionStatus(ACTION_STAND);
			stopPlayer->SetPosition(x, y);
			stopPlayer->SetDirection(direction);

			break;
		}
	}
}

// 20. 캐릭터 공격 패킷을 보내는 함수
void SendAttack1()
{
	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->GetAnimDirection();
	WORD x = myPlayer->GetXPos();
	WORD y = myPlayer->GetYPos();
	
	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack1(&networkPacketHeader, &serializationBuffer, direction, x, y);

	SendToServer(networkPacketHeader, &serializationBuffer);

	FDWriteProc();
}

// 21. 캐릭터 공격 패킷을 받는 함수
void RecvAttack1(SerializationBuffer* serializationBuffer)
{
	DWORD ID;
	BYTE direction;
	WORD x, y;

	*serializationBuffer >> ID >> direction >> x >> y;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == ID)
		{
			Player* attackPlayer = (Player*)(*objectListIter);
			attackPlayer->SetActionStatus(ACTION_ATTACK_01);
			attackPlayer->SetPosition(x, y);
			attackPlayer->SetDirection(direction);

			if (attackPlayer->GetAnimDirection() == ACTION_MOVE_LL)
			{
				attackPlayer->SetAnimation(PLAYER_ATTACK1_L_01, 4, 3);
			}
			else
			{
				attackPlayer->SetAnimation(PLAYER_ATTACK1_R_01, 4, 3);
			}

			break;
		}
	}
}

// 22. 캐릭터 공격 패킷을 보내는 함수
void SendAttack2()
{
	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->GetAnimDirection();
	WORD x = myPlayer->GetXPos();
	WORD y = myPlayer->GetYPos();

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;

	MakePacketAttack2(&networkPacketHeader, &serializationBuffer, direction, x, y);

	SendToServer(networkPacketHeader, &serializationBuffer);

	FDWriteProc();
}

// 23. 캐릭터 공격 패킷을 받는 함수
void RecvAttack2(SerializationBuffer* serializationBuffer)
{
	DWORD ID;
	BYTE direction;
	WORD x, y;

	*serializationBuffer >> ID >> direction >> x >> y;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == ID)
		{
			Player* attackPlayer = (Player*)(*objectListIter);
			attackPlayer->SetActionStatus(ACTION_ATTACK_02);
			attackPlayer->SetPosition(x, y);
			attackPlayer->SetDirection(direction);

			if (attackPlayer->GetAnimDirection() == ACTION_MOVE_LL)
			{
				attackPlayer->SetAnimation(PLAYER_ATTACK2_L_01, 4, 4);
			}
			else
			{
				attackPlayer->SetAnimation(PLAYER_ATTACK2_R_01, 4, 4);
			}

			break;
		}
	}
}

// 24. 캐릭터 공격 패킷을 보내는 함수
void SendAttack3()
{
	if (myPlayer == nullptr)
	{
		return;
	}

	BYTE direction = myPlayer->GetAnimDirection();
	WORD x = myPlayer->GetXPos();
	WORD y = myPlayer->GetYPos();	

	NetworkPacketHeader networkPacketHeader;
	SerializationBuffer serializationBuffer;	
	
	MakePacketAttack3(&networkPacketHeader, &serializationBuffer, direction, x, y);

	SendToServer(networkPacketHeader, &serializationBuffer);

	FDWriteProc();
}

// 25. 캐릭터 공격 패킷을 받는 함수
void RecvAttack3(SerializationBuffer* serializationBuffer)
{
	DWORD ID;
	BYTE direction;
	WORD x, y;

	*serializationBuffer >> ID >> direction >> x >> y;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == ID)
		{
			Player* attackPlayer = (Player*)(*objectListIter);
			attackPlayer->SetActionStatus(ACTION_ATTACK_03);
			attackPlayer->SetPosition(x, y);
			attackPlayer->SetDirection(direction);

			if (attackPlayer->GetAnimDirection() == ACTION_MOVE_LL)
			{
				attackPlayer->SetAnimation(PLAYER_ATTACK3_L_01, 6, 4);
			}
			else
			{
				attackPlayer->SetAnimation(PLAYER_ATTACK3_R_01, 6, 4);
			}

			break;
		}
	}
}

// 30. 캐릭터 데미지 패킷을 받는 함수
void RecvDamage(SerializationBuffer* serializationBuffer)
{
	DWORD attackID, damageID;
	BYTE damageHP;

	*serializationBuffer >> attackID >> damageID >> damageHP;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	// 공격자에게서 이펙트가 발생하도록 설정.
	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == attackID)
		{
			Player* attackPlayer = (Player*)(*objectListIter);
			attackPlayer->SetEffectFlag(true);

			break;
		}
	}

	// 피격자의 HP를 감소하도록 설정.
	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == damageID)
		{
			Player* damagePlayer = (Player*)(*objectListIter);
			damagePlayer->SetHP(damageHP);

			break;
		}
	}
}

// 251. 동기화를 위한 패킷을 받는 함수
void RecvSync(SerializationBuffer* serializationBuffer)
{
	int userID;
	WORD x, y;

	*serializationBuffer >> userID >> x >> y;

	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	for (objectListIter = objectList.begin();
		objectListIter != objectListIterEnd;
		++objectListIter)
	{
		if ((*objectListIter)->GetUserID() == userID)
		{
			Player* syncPlayer = (Player*)(*objectListIter);

			if (myPlayer != nullptr && syncPlayer == myPlayer)
			{
				int myPlayerPosX = myPlayer->GetXPos();
				int myPlayerPosY = myPlayer->GetYPos();

				debugServerPosX = x;
				debugServerPosY = y;

				debugClientPosX = myPlayerPosX;
				debugClientPosY = myPlayerPosY;

				int scrollX = x - SCREEN_WIDTH / 2;
				int scrollY = y - SCREEN_HEIGHT / 2 - 25;

				tileMap->SetScrollX(scrollX);
				tileMap->SetScrollY(scrollY);
			}

			syncPlayer->SetPosition(x, y);

			break;
		}
	}
}

// 서버로 패킷을 보내는 함수
void SendToServer(NetworkPacketHeader networkPacketHeader, 
	SerializationBuffer* serializationBuffer)
{
	sendQueue->Enqueue((BYTE*)&networkPacketHeader, sizeof(networkPacketHeader));
	sendQueue->Enqueue((BYTE*)serializationBuffer->GetFrontPosBufferPtr(),
		serializationBuffer->GetUseSize());
}

// 소켓 함수 오류 출력 후 종료
void ErrorQuit(WCHAR* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

// 소켓 함수 오류 출력
void ErrorDisplay(WCHAR* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	wprintf(L"[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}