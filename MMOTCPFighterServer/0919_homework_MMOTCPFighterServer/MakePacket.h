#pragma once

// 0. Ŭ���̾�Ʈ �ڽ��� ĳ���� �Ҵ� ��Ŷ�� ����� �Լ�
void MakePacketCreateMyCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp);

// 1. �ٸ� Ŭ���̾�Ʈ�� ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketCreateOtherCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp);

// 2. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketDeleteCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID);

// 11. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStart(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, 
	int userID, BYTE direction, WORD posX, WORD posY);

// 13. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStop(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD posX, WORD posY);

// 21. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack1(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, 
	int userID, BYTE direction, WORD x, WORD y);

// 23. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack2(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD x, WORD y);

// 25. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack3(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD x, WORD y);

// 30. ĳ���� ������ ��Ŷ�� ����� �Լ�
void MakePacketDamage(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int attackID, int damageID, BYTE damageHP);

// 251. ����ȭ�� ���� ��Ŷ�� ����� �Լ�
void MakePacketSync(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, WORD x, WORD y);