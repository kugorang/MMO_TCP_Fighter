#pragma once

#include "PacketDefine.h"
#include "SerializationBuffer.h"

// ���� ���� ������ �޽��� ó��
void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// FD_WRITE ó�� �Լ�
void FDWriteProc();

// FD_READ ó�� �Լ�
void FDReadProc();

// Packet�� ó���ϴ� �Լ�
void PacketProc(BYTE type, SerializationBuffer* serializationBuffer);

// 0. Ŭ���̾�Ʈ �ڽ��� ĳ���� �Ҵ� ��Ŷ�� �޴� �Լ�
void RecvCreateMyCharacter(SerializationBuffer* serializationBuffer);

// 1. �ٸ� Ŭ���̾�Ʈ�� ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvCreateOtherCharacter(SerializationBuffer* serializationBuffer);

// 2. ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvDeleteCharacter(SerializationBuffer* serializationBuffer);

// 10. ĳ���� �̵� ���� ��Ŷ�� ������ �Լ�
void SendMoveStart();

// 11. ĳ���� �̵� ���� ��Ŷ�� �޴� �Լ�
void RecvMoveStart(SerializationBuffer* serializationBuffer);

// 12. ĳ���� �̵� ���� ��Ŷ�� ������ �Լ�
void SendMoveStop();

// 13. ĳ���� �̵� ���� ��Ŷ�� �޴� �Լ�
void RecvMoveStop(SerializationBuffer* serializationBuffer);

// 20. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack1();

// 21. ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvAttack1(SerializationBuffer* serializationBuffer);

// 22. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack2();

// 23. ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvAttack2(SerializationBuffer* serializationBuffer);

// 24. ĳ���� ���� ��Ŷ�� ������ �Լ�
void SendAttack3();

// 25. ĳ���� ���� ��Ŷ�� �޴� �Լ�
void RecvAttack3(SerializationBuffer* serializationBuffer);

// 30. ĳ���� ������ ��Ŷ�� �޴� �Լ�
void RecvDamage(SerializationBuffer* serializationBuffer);

// 251. ����ȭ�� ���� ��Ŷ�� �޴� �Լ�
void RecvSync(SerializationBuffer* serializationBuffer);

// ������ ��Ŷ�� ������ �Լ�
void SendToServer(NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);

// ���� �Լ� ���� ��� �� ����
void ErrorQuit(WCHAR* msg);

// ���� �Լ� ���� ���
void ErrorDisplay(WCHAR* msg);