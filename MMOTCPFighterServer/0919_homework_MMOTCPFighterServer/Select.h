#pragma once

#define ATTACK1_RANGE_X		80
#define ATTACK2_RANGE_X		90
#define ATTACK3_RANGE_X		100
#define ATTACK1_RANGE_Y		10
#define ATTACK2_RANGE_Y		10
#define ATTACK3_RANGE_Y		20

#define ATTACK1_DAMAGE		1
#define ATTACK2_DAMAGE		1
#define ATTACK3_DAMAGE		1

#define CHARACTER_LR_MOVE_RANGE 3
#define CHARACTER_UD_MOVE_RANGE 2

#define GENERATE_WIDTH 6400
#define GENERATE_HEIGHT 6400

#include "SerializationBuffer.h"

// FD_WRITE 처리 함수
int FDWriteProc(SOCKET sock, SOCKETINFO* socketInfo);

// FD_READ 처리 함수
int FDReadProc(SOCKET sock, SOCKETINFO* socketInfo);

// Packet을 처리하는 함수
void PacketProc(SOCKET sock, WORD type, SerializationBuffer* serializationBuffer);

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 보내는 함수
void SendCreateMyCharacter(SOCKET sock);

// 1. 다른 클라이언트의 캐릭터 생성 패킷을 보내는 함수
void SendCreateOtherCharacter(SOCKET sock, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp, bool leftFlag);

// 2. 캐릭터 삭제 패킷을 보내는 함수
void SendDeleteCharacter(SOCKET sock, PLAYER* deletePlayer);

// 10. 캐릭터 이동 시작 패킷을 받는 함수
void RecvMoveStart(SOCKET sock, SerializationBuffer* serializationBuffer);

// 11. 캐릭터 이동 시작 패킷을 보내는 함수
void SendMoveStart(SOCKET sock, BYTE direction, WORD posX, WORD posY, 
	PLAYER **movePlayer = nullptr);

// 12. 캐릭터 이동 중지 패킷을 받는 함수
void RecvMoveStop(SOCKET sock, SerializationBuffer* serializationBuffer);

// 13. 캐릭터 이동 중지 패킷을 보내는 함수
void SendMoveStop(SOCKET sock, BYTE direction, WORD posX, WORD posY, 
	PLAYER **stopPlayer = nullptr);

// 20. 캐릭터 공격 패킷을 받는 함수
void RecvAttack1(SOCKET sock, SerializationBuffer* serializationBuffer);

// 21. 캐릭터 공격 패킷을 보내는 함수
void SendAttack1(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY);

// 22. 캐릭터 공격 패킷을 받는 함수
void RecvAttack2(SOCKET sock, SerializationBuffer* serializationBuffer);

// 23. 캐릭터 공격 패킷을 보내는 함수
void SendAttack2(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY);

// 24. 캐릭터 공격 패킷을 받는 함수
void RecvAttack3(SOCKET sock, SerializationBuffer* serializationBuffer);

// 25. 캐릭터 공격 패킷을 보내는 함수
void SendAttack3(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY);

// 30. 캐릭터 데미지 패킷을 보내는 함수
void SendDamage(SOCKET sock, int attackID, int damageID, BYTE damageHP);

// 251. 동기화를 위한 패킷을 보내는 함수
void SendSync(SOCKET sock, int userID, WORD posX, WORD posY);

// 소켓 관리 함수
bool AddSocketInfo(SOCKET sock);
map<SOCKET, SOCKETINFO*>::iterator RemoveSocketInfo(SOCKET sock);

// 그 사람에게만 송신하는 함수
void SendUnicast(SOCKET sock,
	NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);

// 그 사람에게만 송신하는 함수
void SendUnicast(SOCKETINFO* socketInfo, 
	NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);

// 그 사람의 섹터에 송신하는 함수 (excepSock : 제외할 소켓)
void SendOneSector(SOCKET mainSock, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock = 0);

// 그 사람의 섹터 및 주변 섹터에 송신하는 함수 (excepSock : 제외할 소켓)
void SendAroundSector(SOCKET mainSock, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock = 0);

// 모든 사람들에게 송신하는 함수 (excepSock : 제외할 소켓)
void SendBroadcast(NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock = 0);