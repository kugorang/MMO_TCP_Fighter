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

// FD_WRITE ó�� �Լ�
int FDWriteProc(SOCKET sock, SOCKETINFO* socketInfo);

// FD_READ ó�� �Լ�
int FDReadProc(SOCKET sock, SOCKETINFO* socketInfo);

// Packet�� ó���ϴ� �Լ�
void PacketProc(SOCKET sock, WORD type, SerializationBuffer* serializationBuffer);

// 0. Ŭ���̾�Ʈ �ڽ��� ĳ���� �Ҵ� ��Ŷ�� ������ �Լ�
void SendCreateMyCharacter(SOCKET sock);

// 1. �ٸ� Ŭ���̾�Ʈ�� ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendCreateOtherCharacter(SOCKET sock, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp, bool leftFlag);

// 2. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendDeleteCharacter(SOCKET sock, PLAYER* deletePlayer);

// 10. ĳ���� �̵� ���� ��Ŷ�� �޴� �Լ�
void RecvMoveStart(SOCKET sock, SerializationBuffer* serializationBuffer);

// 11. ĳ���� �̵� ���� ��Ŷ�� ������ �Լ�
void SendMoveStart(SOCKET sock, BYTE direction, WORD posX, WORD posY, 
	PLAYER **movePlayer = nullptr);

// 12. ĳ���� �̵� ���� ��Ŷ�� �޴� �Լ�
void RecvMoveStop(SOCKET sock, SerializationBuffer* serializationBuffer);

// 13. ĳ���� �̵� ���� ��Ŷ�� ������ �Լ�
void SendMoveStop(SOCKET sock, BYTE direction, WORD posX, WORD posY, 
	PLAYER **stopPlayer = nullptr);

// 20. ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvAttack1(SOCKET sock, SerializationBuffer* serializationBuffer);

// 21. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack1(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY);

// 22. ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvAttack2(SOCKET sock, SerializationBuffer* serializationBuffer);

// 23. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack2(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY);

// 24. ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvAttack3(SOCKET sock, SerializationBuffer* serializationBuffer);

// 25. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack3(SOCKET sock, BYTE attackDirection, WORD attackX, WORD attackY);

// 30. ĳ���� ������ ��Ŷ�� ������ �Լ�
void SendDamage(SOCKET sock, int attackID, int damageID, BYTE damageHP);

// 251. ����ȭ�� ���� ��Ŷ�� ������ �Լ�
void SendSync(SOCKET sock, int userID, WORD posX, WORD posY);

// ���� ���� �Լ�
bool AddSocketInfo(SOCKET sock);
map<SOCKET, SOCKETINFO*>::iterator RemoveSocketInfo(SOCKET sock);

// �� ������Ը� �۽��ϴ� �Լ�
void SendUnicast(SOCKET sock,
	NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);

// �� ������Ը� �۽��ϴ� �Լ�
void SendUnicast(SOCKETINFO* socketInfo, 
	NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);

// �� ����� ���Ϳ� �۽��ϴ� �Լ� (excepSock : ������ ����)
void SendOneSector(SOCKET mainSock, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock = 0);

// �� ����� ���� �� �ֺ� ���Ϳ� �۽��ϴ� �Լ� (excepSock : ������ ����)
void SendAroundSector(SOCKET mainSock, NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock = 0);

// ��� ����鿡�� �۽��ϴ� �Լ� (excepSock : ������ ����)
void SendBroadcast(NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer, SOCKET exceptSock = 0);