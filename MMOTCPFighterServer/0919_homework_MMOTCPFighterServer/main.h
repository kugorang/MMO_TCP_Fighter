#pragma once

#include <map>
#include <inttypes.h>
#include "RingBuffer.h"
#include "PacketDefine.h"
#include "Timer.h"
#include "DeadReckoning.h"
#include "Log.h"

using namespace std;

#define SOCKETINFO_ARRAY_MAX 5000

#define MAP_WIDTH 6400
#define MAP_HEIGHT 6400

#define DEFAULT_FRAME_TIME 20	// 50 프레임 기준
#define FRAME_INTERVAL_OPTION 2

// 섹터 정보 저장을 위한 구조체
struct AROUNDSECTOR
{
	int sectorNum;
	POINT sectorIndex;
};

struct SOCKETINFO;

// 캐릭터 정보 저장을 위한 구조체
struct PLAYER
{
	WORD posX, posY;
	int userID;
	BYTE hp, direction;
	bool moveFlag, leftFlag;
	SOCKET clientSock;

	DeadReckoning deadReckoning;
};

// 소켓 정보 저장을 위한 구조체
struct SOCKETINFO
{
	PLAYER *player;

	UINT64 socketNo;
	SOCKET clientSock;	

	RingBuffer recvQueue;
	RingBuffer sendQueue;

	POINT sectorIndex;

	int lastSendSize;
	int lastRecvSize;
};

// 섹터 클래스
extern class Sector sector;

// 로그 클래스
extern class Log printLog;

// 소켓 관리 map
extern map<SOCKET, SOCKETINFO*> socketInfoMap;

extern int totalSockets;

void Network();
void Action();