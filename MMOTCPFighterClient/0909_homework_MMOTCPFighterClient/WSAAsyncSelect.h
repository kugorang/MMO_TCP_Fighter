#pragma once

#include "PacketDefine.h"
#include "SerializationBuffer.h"

// 소켓 관련 윈도우 메시지 처리
void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// FD_WRITE 처리 함수
void FDWriteProc();

// FD_READ 처리 함수
void FDReadProc();

// Packet을 처리하는 함수
void PacketProc(BYTE type, SerializationBuffer* serializationBuffer);

// 0. 클라이언트 자신의 캐릭터 할당 패킷을 받는 함수
void RecvCreateMyCharacter(SerializationBuffer* serializationBuffer);

// 1. 다른 클라이언트의 캐릭터 생성 패킷을 받는 함수
void RecvCreateOtherCharacter(SerializationBuffer* serializationBuffer);

// 2. 캐릭터 삭제 패킷을 받는 함수
void RecvDeleteCharacter(SerializationBuffer* serializationBuffer);

// 10. 캐릭터 이동 시작 패킷을 보내는 함수
void SendMoveStart();

// 11. 캐릭터 이동 시작 패킷을 받는 함수
void RecvMoveStart(SerializationBuffer* serializationBuffer);

// 12. 캐릭터 이동 중지 패킷을 보내는 함수
void SendMoveStop();

// 13. 캐릭터 이동 중지 패킷을 받는 함수
void RecvMoveStop(SerializationBuffer* serializationBuffer);

// 20. 캐릭터 공격 패킷을 보내는 함수
void SendAttack1();

// 21. 캐릭터 공격 패킷을 받는 함수
void RecvAttack1(SerializationBuffer* serializationBuffer);

// 22. 캐릭터 공격 패킷을 보내는 함수
void SendAttack2();

// 23. 캐릭터 공격 패킷을 받는 함수
void RecvAttack2(SerializationBuffer* serializationBuffer);

// 24. 캐릭터 공격 패킷을 보내는 함수
void SendAttack3();

// 25. 캐릭터 공격 패킷을 받는 함수
void RecvAttack3(SerializationBuffer* serializationBuffer);

// 30. 캐릭터 데미지 패킷을 받는 함수
void RecvDamage(SerializationBuffer* serializationBuffer);

// 251. 동기화를 위한 패킷을 받는 함수
void RecvSync(SerializationBuffer* serializationBuffer);

// 서버로 패킷을 보내는 함수
void SendToServer(NetworkPacketHeader networkPacketHeader,
	SerializationBuffer* serializationBuffer);

// 소켓 함수 오류 출력 후 종료
void ErrorQuit(WCHAR* msg);

// 소켓 함수 오류 출력
void ErrorDisplay(WCHAR* msg);