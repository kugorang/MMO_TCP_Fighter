#include "main.h"
#include "Select.h"
#include "MakePacket.h"

// 0. Ŭ���̾�Ʈ �ڽ��� ĳ���� �Ҵ� ��Ŷ�� ����� �Լ�
void MakePacketCreateMyCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_CREATE_MY_CHARACTER;

	*serializationBuffer << userID << direction << posX << posY << hp << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 1. �ٸ� Ŭ���̾�Ʈ�� ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketCreateOtherCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_CREATE_OTHER_CHARACTER;

	*serializationBuffer << userID << direction << posX << posY << hp << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 2. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketDeleteCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_DELETE_CHARACTER;

	*serializationBuffer << userID << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 11. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStart(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD posX, WORD posY)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_MOVE_START;

	*serializationBuffer << userID << direction << posX << posY << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 13. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStop(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD posX, WORD posY)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_MOVE_STOP;

	*serializationBuffer << userID << direction << posX << posY << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 21. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack1(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_ATTACK1;

	*serializationBuffer << userID << direction << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 23. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack2(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_ATTACK2;

	*serializationBuffer << userID << direction << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 25. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack3(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_ATTACK3;

	*serializationBuffer << userID << direction << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 30. ĳ���� ������ ��Ŷ�� ����� �Լ�
void MakePacketDamage(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int attackID, int damageID, BYTE damageHP)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_DAMAGE;

	*serializationBuffer << attackID << damageID << damageHP << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 251. ����ȭ�� ���� ��Ŷ�� ����� �Լ�
void MakePacketSync(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_SYNC;

	*serializationBuffer << userID << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}