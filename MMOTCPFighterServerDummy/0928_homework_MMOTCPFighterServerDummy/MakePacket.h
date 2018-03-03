#pragma once

// 10. 캐릭터 이동 시작 패킷을 만드는 함수
void MakePacketMoveStart(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 12. 캐릭터 이동 중지 패킷을 만드는 함수
void MakePacketMoveStop(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 20. 캐릭터 공격 패킷을 만드는 함수
void MakePacketAttack1(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 22. 캐릭터 공격 패킷을 만드는 함수
void MakePacketAttack2(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 24. 캐릭터 공격 패킷을 만드는 함수
void MakePacketAttack3(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);