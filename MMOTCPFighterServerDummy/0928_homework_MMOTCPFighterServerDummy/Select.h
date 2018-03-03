#pragma once

#define CHARACTER_LR_MOVE_RANGE 3
#define CHARACTER_UD_MOVE_RANGE 2

#include "SerializationBuffer.h"

// FD_WRITE ó�� �Լ�
int FDWriteProc(SOCKET sock, SOCKETINFO* socketInfo);

// FD_READ ó�� �Լ�
int FDReadProc(SOCKET sock, SOCKETINFO* socketInfo);

// Packet�� ó���ϴ� �Լ�
void PacketProc(SOCKET sock, WORD type, SerializationBuffer* serializationBuffer);

// 0. Ŭ���̾�Ʈ �ڽ��� ĳ���� �Ҵ� ��Ŷ�� �޴� �Լ�
void RecvCreateMyCharacter(SOCKET sock, SerializationBuffer* serializationBuffer);

// 10. ĳ���� �̵� ���� ��Ŷ�� ������ �Լ�
void SendMoveStart(SOCKET sock);

// 12. ĳ���� �̵� ���� ��Ŷ�� ������ �Լ�
void SendMoveStop(SOCKET sock);

// 20. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack1(SOCKET sock);

// 22. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack2(SOCKET sock);

// 24. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack3(SOCKET sock);

// 251. ����ȭ�� ���� ��Ŷ�� �޴� �Լ�
void RecvSync(SOCKET sock, SerializationBuffer* serializationBuffer);

// Ŭ���̾�Ʈ ������ �����ϴ� �Լ�
map<SOCKET, SOCKETINFO*>::iterator RemoveSocketInfo(SOCKET sock);

// ������ ��Ŷ�� ������ �Լ�
void SendToServer(SOCKETINFO* socketInfo, 
	NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);