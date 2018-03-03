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

#define DEFAULT_FRAME_TIME 20	// 50 ������ ����
#define FRAME_INTERVAL_OPTION 2

// ���� ���� ������ ���� ����ü
struct AROUNDSECTOR
{
	int sectorNum;
	POINT sectorIndex;
};

struct SOCKETINFO;

// ĳ���� ���� ������ ���� ����ü
struct PLAYER
{
	WORD posX, posY;
	int userID;
	BYTE hp, direction;
	bool moveFlag, leftFlag;
	SOCKET clientSock;

	DeadReckoning deadReckoning;
};

// ���� ���� ������ ���� ����ü
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

// ���� Ŭ����
extern class Sector sector;

// �α� Ŭ����
extern class Log printLog;

// ���� ���� map
extern map<SOCKET, SOCKETINFO*> socketInfoMap;

extern int totalSockets;

void Network();
void Action();