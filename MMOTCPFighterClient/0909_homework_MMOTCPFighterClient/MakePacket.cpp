#include "stdafx.h"
#include "MakePacket.h"

// 10. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStart(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_CS_MOVE_START;
	
	*serializationBuffer << direction << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 12. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStop(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_CS_MOVE_STOP;

	*serializationBuffer << direction << x << y << endCode;
	
	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 20. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack1(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;	
	networkPacketHeader->type = PACKETDEFINE_CS_ATTACK1;

	*serializationBuffer << direction << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 22. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack2(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;	
	networkPacketHeader->type = PACKETDEFINE_CS_ATTACK2;

	*serializationBuffer << direction << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 24. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack3(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_CS_ATTACK3;

	*serializationBuffer << direction << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}