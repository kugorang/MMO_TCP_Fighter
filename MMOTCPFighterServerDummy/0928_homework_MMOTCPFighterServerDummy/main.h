#pragma once

#include <map>
#include "RingBuffer.h"
#include "PacketDefine.h"
#include "Timer.h"
#include "DeadReckoning.h"
#include "Log.h"

using namespace std;

#define SOCKETINFO_ARRAY_MAX 1000

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MAP_WIDTH 6400
#define MAP_HEIGHT 6400

#define DEFAULT_FRAME_TIME 20	// 50 프레임 기준
#define FRAME_INTERVAL_OPTION 1

// 캐릭터 정보 저장을 위한 구조체
struct PLAYER
{
	WORD posX, posY;
	int userID;
	BYTE hp, direction;
	bool moveFlag;
	SOCKET clientSock;
	
	DeadReckoning deadReckoning;
};

// 소켓 정보 저장을 위한 구조체
struct SOCKETINFO
{
	PLAYER *player;

	UINT64 socketNo;
	SOCKET clientSock;

	Timer playerTimer;
	RingBuffer recvQueue;
	RingBuffer sendQueue;
	
	int lastSendSize;
	int lastRecvSize;
};

extern class Log printLog;

extern map<SOCKET, SOCKETINFO*> socketInfoMap;

extern UINT successSocket, failSocket;

void Network();
void Action();