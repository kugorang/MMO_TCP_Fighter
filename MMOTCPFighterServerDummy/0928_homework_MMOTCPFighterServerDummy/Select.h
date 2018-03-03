#pragma once

#define CHARACTER_LR_MOVE_RANGE 3
#define CHARACTER_UD_MOVE_RANGE 2

#include "SerializationBuffer.h"

// FD_WRITE 처리 함수
int FDWriteProc(SOCKET sock, SOCKETINFO* socketInfo);

// FD_READ 처리 함수
int FDReadProc(SOCKET sock, SOCKETINFO* socketInfo);

// Packet을 처리하는 함수
void PacketProc(SOCKET sock, WORD type, SerializationBuffer* serializationBuffer);

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 받는 함수
void RecvCreateMyCharacter(SOCKET sock, SerializationBuffer* serializationBuffer);

// 10. 캐릭터 이동 시작 패킷을 보내는 함수
void SendMoveStart(SOCKET sock);

// 12. 캐릭터 이동 중지 패킷을 보내는 함수
void SendMoveStop(SOCKET sock);

// 20. 캐릭터 공격 패킷을 보내는 함수
void SendAttack1(SOCKET sock);

// 22. 캐릭터 공격 패킷을 보내는 함수
void SendAttack2(SOCKET sock);

// 24. 캐릭터 공격 패킷을 보내는 함수
void SendAttack3(SOCKET sock);

// 251. 동기화를 위한 패킷을 받는 함수
void RecvSync(SOCKET sock, SerializationBuffer* serializationBuffer);

// 클라이언트 접속을 해제하는 함수
map<SOCKET, SOCKETINFO*>::iterator RemoveSocketInfo(SOCKET sock);

// 서버로 패킷을 보내는 함수
void SendToServer(SOCKETINFO* socketInfo, 
	NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);