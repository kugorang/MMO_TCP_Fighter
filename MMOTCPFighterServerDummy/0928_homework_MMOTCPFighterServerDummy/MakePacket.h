#pragma once

// 10. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStart(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 12. ĳ���� �̵� ���� ��Ŷ�� ����� �Լ�
void MakePacketMoveStop(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 20. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack1(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 22. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack2(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);

// 24. ĳ���� ���� ��Ŷ�� ����� �Լ�
void MakePacketAttack3(NetworkPacketHeader* networkPacketHeader,
	SerializationBuffer* serializationBuffer, BYTE direction, WORD x, WORD y);