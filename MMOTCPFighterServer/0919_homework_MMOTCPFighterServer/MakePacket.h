#pragma once

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 만드는 함수
void MakePacketCreateMyCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp);

// 1. 다른 클라이언트의 캐릭터 생성 패킷을 만드는 함수
void MakePacketCreateOtherCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID, BYTE direction,
	WORD posX, WORD posY, BYTE hp);

// 2. 캐릭터 삭제 패킷을 만드는 함수
void MakePacketDeleteCharacter(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, int userID);

// 11. 캐릭터 이동 시작 패킷을 만드는 함수
void MakePacketMoveStart(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, 
	int userID, BYTE direction, WORD posX, WORD posY);

// 13. 캐릭터 이동 중지 패킷을 만드는 함수
void MakePacketMoveStop(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD posX, WORD posY);

// 21. 캐릭터 공격 패킷을 만드는 함수
void MakePacketAttack1(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, 
	int userID, BYTE direction, WORD x, WORD y);

// 23. 캐릭터 공격 패킷을 만드는 함수
void MakePacketAttack2(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD x, WORD y);

// 25. 캐릭터 공격 패킷을 만드는 함수
void MakePacketAttack3(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, BYTE direction, WORD x, WORD y);

// 30. 캐릭터 데미지 패킷을 만드는 함수
void MakePacketDamage(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int attackID, int damageID, BYTE damageHP);

// 251. 동기화를 위한 패킷을 만드는 함수
void MakePacketSync(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer,
	int userID, WORD x, WORD y);