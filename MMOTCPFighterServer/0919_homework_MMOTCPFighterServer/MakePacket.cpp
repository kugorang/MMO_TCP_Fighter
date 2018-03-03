#include "main.h"
#include "Select.h"
#include "MakePacket.h"

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 만드는 함수
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

// 1. 다른 클라이언트의 캐릭터 생성 패킷을 만드는 함수
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

// 2. 캐릭터 삭제 패킷을 만드는 함수
void MakePacketDeleteCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_DELETE_CHARACTER;

	*serializationBuffer << userID << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 11. 캐릭터 이동 시작 패킷을 만드는 함수
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

// 13. 캐릭터 이동 중지 패킷을 만드는 함수
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

// 21. 캐릭터 공격 패킷을 만드는 함수
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

// 23. 캐릭터 공격 패킷을 만드는 함수
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

// 25. 캐릭터 공격 패킷을 만드는 함수
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

// 30. 캐릭터 데미지 패킷을 만드는 함수
void MakePacketDamage(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int attackID, int damageID, BYTE damageHP)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_DAMAGE;

	*serializationBuffer << attackID << damageID << damageHP << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}

// 251. 동기화를 위한 패킷을 만드는 함수
void MakePacketSync(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, WORD x, WORD y)
{
	BYTE endCode = NETWORK_PACKET_END;

	networkPacketHeader->code = NETWORK_PACKET_CODE;
	networkPacketHeader->type = PACKETDEFINE_SC_SYNC;

	*serializationBuffer << userID << x << y << endCode;

	networkPacketHeader->size = serializationBuffer->GetUseSize() - (BYTE)sizeof(endCode);
}