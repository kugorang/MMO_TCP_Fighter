#pragma once

#include <Windows.h>
#include <list>
#include "resource.h"
#include "SpriteDib.h"
#include "ScreenDib.h"
#include "BaseObject.h"
#include "RingBuffer.h"
#include "Map.h"

using namespace std;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define WM_SOCKET (WM_USER + 1)

// 전역 변수 extern 선언
extern class SpriteDib* spriteDib;
extern class ScreenDib* screenDib;
extern list<BaseObject*> objectList;
extern class Player* myPlayer;
extern class Map* tileMap;

struct DrawMapIndex
{
	WCHAR xIndex[4];
	WCHAR yIndex[4];

	int xPos, yPos;
};

extern DrawMapIndex drawMapIndex[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];

extern int debugServerPosX, debugServerPosY;
extern int debugClientPosX, debugClientPosY;

extern SOCKET linkSock;
extern class RingBuffer* recvQueue;
extern class RingBuffer* sendQueue;

extern HWND hWnd;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

bool InitialGame();
bool LoadSprite();

void Update();
void KeyProcess();
void Action();
void Draw();
void FrameSkip();

//void SortList(int left, int right);