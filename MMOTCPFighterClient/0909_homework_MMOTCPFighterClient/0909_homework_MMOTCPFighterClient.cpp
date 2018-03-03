// 0909_homework_MMOTCPFighterClient.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include <mmsystem.h>
#include <crtdbg.h>
#include "0909_homework_MMOTCPFighterClient.h"
#include "FrameSkip.h"
#include "Player.h"
#include "WSAAsyncSelect.h"
#include "Effect.h"

#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

// 전역 변수:
HWND hWnd;

SpriteDib* spriteDib = new SpriteDib(66, 0x00ffffff);
ScreenDib* screenDib = new ScreenDib(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
static class FrameSkip* frameSkip = new class FrameSkip();

Map* tileMap = new Map();

list<BaseObject*> objectList;
Player* myPlayer;

static bool isActiveApp;

static int frame = 0;

int debugServerPosX = 0, debugServerPosY = 0;
int debugClientPosX = 0, debugClientPosY = 0;

// 시작, 끝, 주기 진동수를 얻을 구조체를 선언한다.
static LARGE_INTEGER frameStart, frameEnd, frameFrequency;
static DWORD cumulativeTime;

static HDC hdc;
static TCHAR frameStr[4] = { 0, };
DrawMapIndex drawMapIndex[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];

// 소켓 관련 전역 변수
static WCHAR inputIP[16] = { 0, };

SOCKET linkSock;
RingBuffer* recvQueue = new RingBuffer();
RingBuffer* sendQueue = new RingBuffer();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{	
	// TODO: 여기에 코드를 입력합니다.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);
	//_crtBreakAlloc = 124; // 중단점을 걸어주는 문구, _CrtSetDbgFlag와 세트
	// {,,msvcr120d.dll}_crtBreakAlloc <- 조사식에 이걸 써도 된다! 짱 편함!
	if (!InitialGame())
	{
		OutputDebugString(L"파일을 불러오는 데 실패했습니다.\n");
		exit(4001);
	}

	timeBeginPeriod(1);

	// 윈속 초기화
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	MSG msg;
	WNDCLASSEX wcex;
	RECT windowRect;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY0909_HOMEWORK_MMOTCPFIGHTERCLIENT));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"0909_homework_MMOTCPFighterClient";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);
	
	DialogBox(hInstance, MAKEINTRESOURCE(ID_ADDR), NULL, DialogProc);

	hWnd = CreateWindow(L"0909_homework_MMOTCPFighterClient", L"0909_homework_MMOTCPFighterClient", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	windowRect.top = 0;
	windowRect.left = 0;
	windowRect.right = SCREEN_WIDTH;
	windowRect.bottom = SCREEN_HEIGHT;

	AdjustWindowRect(&windowRect, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL);

	MoveWindow(hWnd, (GetSystemMetrics(SM_CXSCREEN) - SCREEN_WIDTH) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - SCREEN_HEIGHT) / 2, windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top, TRUE);

	// 기본 메시지 루프입니다.
	while (true)
	{		
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
		}
	}

	return (int)msg.wParam;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int retval;
	list<BaseObject*>::iterator objectListIter;
	list<BaseObject*>::iterator objectListIterEnd = objectList.end();

	// TCP_NODELAY 옵션
	BOOL optval = TRUE;

	switch (message)
	{
	case WM_CREATE:
		// socket()
		linkSock = socket(AF_INET, SOCK_STREAM, 0);
		
		if (setsockopt(linkSock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval)) == SOCKET_ERROR)
		{
			ErrorQuit(L"setsockopt()");
		}

		// WSAAsyncSelect()
		retval = WSAAsyncSelect(linkSock, hWnd, WM_SOCKET, FD_CONNECT | FD_CLOSE);

		if (retval == SOCKET_ERROR)
		{
			ErrorQuit(L"WSAAsyncSelect()");
			exit(-1);
		}

		SOCKADDR_IN serverAddr;
		serverAddr.sin_family = AF_INET;
		InetPton(AF_INET, inputIP, &serverAddr.sin_addr.s_addr);
		serverAddr.sin_port = htons(5000);

		retval = connect(linkSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				ErrorQuit(L"connect()");
				exit(-1);
			}
		}

		cumulativeTime = 0;

		frameStart.QuadPart = 0;
		frameEnd.QuadPart = 0;
		frameFrequency.QuadPart = 0;

		// 고해상도 타이머의 주기를 얻는다.
		QueryPerformanceFrequency(&frameFrequency);

		// 시작 시점의 진동수를 얻는다.
		QueryPerformanceCounter(&frameStart);
		break;
	case WM_SOCKET:
		ProcessSocketMessage(hWnd, message, wParam, lParam);
		break;
	case WM_IME_NOTIFY:
		DestroyWindow(ImmGetDefaultIMEWnd(hWnd));
		break;
	case WM_ACTIVATEAPP:
		isActiveApp = FALSE != wParam;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		timeEndPeriod(1);

		delete spriteDib;
		delete screenDib;
		delete tileMap;
		delete frameSkip;

		for (objectListIter = objectList.begin();
			objectListIter != objectListIterEnd;)
		{
			if (*objectListIter != nullptr)
			{
				delete *objectListIter;
				objectListIter = objectList.erase(objectListIter);
			}
		}

		delete recvQueue;
		delete sendQueue;

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 다이얼로그 윈도우 프로시저
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND editBox;

	switch (msg)
	{
	case WM_INITDIALOG:
		editBox = GetDlgItem(hWnd, ID_EDIT);
		SetWindowText(editBox, L"192.168.10.116");

		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case ID_OK:
			GetDlgItemText(hWnd, ID_EDIT, inputIP, 16);
			EndDialog(hWnd, 0);

			return TRUE;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		PostQuitMessage(0);
	}

	return FALSE;
}

bool InitialGame()
{
	return LoadSprite();
}

void Update()
{
	bool isFrameNotDelayed;

	if (isActiveApp)
	{
		KeyProcess();
	}

	Action();

	// 끝 시점의 진동수를 얻는다.
	QueryPerformanceCounter(&frameEnd);

	DWORD timeGap = 
		(DWORD)((frameEnd.QuadPart - frameStart.QuadPart) / (frameFrequency.QuadPart / 1000));
	
	// 시작 시점의 진동수를 얻는다.
	QueryPerformanceCounter(&frameStart);
	
	cumulativeTime += timeGap;
	++frame;

	if (cumulativeTime >= 1000)
	{
		wsprintf(frameStr, TEXT("%d"), frame);
		
		cumulativeTime -= 1000;
		frame = 0;
	}
	
	isFrameNotDelayed = frameSkip->IsFrameNotDelayed();

	frameSkip->FrameSkipStart();

	if (isFrameNotDelayed)
	{
		Draw();
		screenDib->DrawBuffer(hWnd);

		hdc = GetDC(hWnd);
		/*SetTextAlign(hdc, TA_CENTER);

		for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
		{
			for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
			{
				TextOut(hdc, drawMapIndex[y][x].xPos + MAP_TILE_SIZE / 4 * 1,
					drawMapIndex[y][x].yPos + MAP_TILE_SIZE / 5 * 2,
					drawMapIndex[y][x].xIndex, lstrlen(drawMapIndex[y][x].xIndex));
				TextOut(hdc, drawMapIndex[y][x].xPos + MAP_TILE_SIZE / 4 * 3,
					drawMapIndex[y][x].yPos + MAP_TILE_SIZE / 5 * 2,
					drawMapIndex[y][x].yIndex, lstrlen(drawMapIndex[y][x].yIndex));
			}
		}*/

		//if (myPlayer != nullptr)
		//{
		//	int scrollX = tileMap->GetScrollX();
		//	int scrollY = tileMap->GetScrollY();

		//	int playerID = myPlayer->GetUserID();
		//	int playerPosX = myPlayer->GetXPos();
		//	int playerPosY = myPlayer->GetYPos();

		//	WCHAR posBuffer[16] = { 0, };

		//	wsprintf(posBuffer, L"[");
		//	wsprintf(posBuffer + 1, L"%4d", playerID);
		//	wsprintf(posBuffer + 5, L"]");
		//	wsprintf(posBuffer + 6, L"%4d", playerPosX);
		//	wsprintf(posBuffer + 10, L":");
		//	wsprintf(posBuffer + 11, L"%4d", playerPosY);

		//	TextOut(hdc, playerPosX - scrollX, playerPosY - scrollY + 17, posBuffer, lstrlen(posBuffer));

		//	/*SetTextAlign(hdc, TA_LEFT);

		//	WCHAR debugServerPosBuffer[16] = { 0, };

		//	wsprintf(debugServerPosBuffer, L"S:");
		//	wsprintf(debugServerPosBuffer + 2, L"%4d", debugServerPosX);
		//	wsprintf(debugServerPosBuffer + 6, L",");
		//	wsprintf(debugServerPosBuffer + 7, L"%4d", debugServerPosY);

		//	TextOut(hdc, 0, 20, debugServerPosBuffer, lstrlen(debugServerPosBuffer));

		//	WCHAR debugClientPosBuffer[16] = { 0, };

		//	wsprintf(debugClientPosBuffer, L"C:");
		//	wsprintf(debugClientPosBuffer + 2, L"%4d", debugClientPosX);
		//	wsprintf(debugClientPosBuffer + 6, L",");
		//	wsprintf(debugClientPosBuffer + 7, L"%4d", debugClientPosY);

		//	TextOut(hdc, 0, 40, debugClientPosBuffer, lstrlen(debugClientPosBuffer));

		//	WCHAR debugGapBuffer[16] = { 0, };

		//	wsprintf(debugGapBuffer, L"%4d", debugServerPosX - debugClientPosX);
		//	wsprintf(debugGapBuffer + 4, L", ");
		//	wsprintf(debugGapBuffer + 6, L"%4d", debugServerPosY - debugClientPosY);
		//				
		//	TextOut(hdc, 30, 0, debugGapBuffer, lstrlen(debugGapBuffer));*/
		//}

		SetTextAlign(hdc, TA_LEFT);
		TextOut(hdc, 0, 0, frameStr, lstrlen(frameStr));
		ReleaseDC(hWnd, hdc);
	}
}

void KeyProcess()
{
	if (myPlayer == nullptr)
	{
		return;
	}

	if (GetAsyncKeyState(0x5A) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_ATTACK_01);
	}
	else if (GetAsyncKeyState(0x58) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_ATTACK_02);
	}
	else if (GetAsyncKeyState(0x43) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_ATTACK_03);
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8001 && GetAsyncKeyState(VK_UP) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_LU);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8001 && GetAsyncKeyState(VK_UP) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_RU);
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8001 && GetAsyncKeyState(VK_DOWN) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_LD);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8001 && GetAsyncKeyState(VK_DOWN) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_RD);
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_UU);
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_DD);
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_LL);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8001)
	{
		myPlayer->MoveCharacter(ACTION_MOVE_RR);
	}
	else
	{
		myPlayer->MoveCharacter(ACTION_STAND);
	}
}

template <typename T>
struct SORTLIST
{
	bool operator() (const T left, const T right) const
	{
		return left->GetYPos() < right->GetYPos();
	}
};

void Action()
{
	list<BaseObject*>::iterator objectListIter;

	for (objectListIter = objectList.begin();
		objectListIter != objectList.end();)
	{
		(*objectListIter)->Action();

		if ((*objectListIter)->GetObjectType() == ObjectType::EFFECT
			&& ((Effect*)*objectListIter)->IsEndFrame())
		{			
			delete *objectListIter;
			objectListIter = objectList.erase(objectListIter);
		}
		else
		{
			++objectListIter;
		}
	}

	if (objectList.max_size() > 1)
	{
		objectList.sort(SORTLIST<BaseObject*>());
	}
}

//void SortList(int left, int right) 
//{
//	int i = left, j = right;
//	BaseObject* tmp;
//	int pivot = templateLinkedList->GetNum((left + right) / 2)->GetYPos();
//
//	// 분할
//	while (i <= j)
//	{
//		while (templateLinkedList->GetNum(i)->GetYPos() < pivot)
//		{
//			i++;
//		}
//
//		while (templateLinkedList->GetNum(j)->GetYPos() > pivot)
//		{
//			j--;
//		}
//
//		if (i <= j)
//		{
//			if (templateLinkedList->GetNum(i)->GetYPos() != templateLinkedList->GetNum(j)->GetYPos())
//			{
//				tmp = templateLinkedList->GetNum(i);
//				templateLinkedList->SetNum(i, templateLinkedList->GetNum(j));
//				templateLinkedList->SetNum(j, tmp);
//			}
//
//			i++;
//			j--;
//		}
//	}
//
//	// 재귀
//	if (left < j)
//	{
//		SortList(left, j);
//	}
//	if (i < right)
//	{
//		SortList(i, right);
//	}
//}

void Draw()
{
	// 출력 버퍼 포인터.
	BYTE* dest = screenDib->GetDibBuffer();
	int destWidth = screenDib->GetWidth();
	int destHeight = screenDib->GetHeight();
	int destPitch = screenDib->GetPitch();

	//spriteDib->DrawImage(MAP, 0, 0, dest, destWidth, destHeight, destPitch);

	tileMap->Draw(dest, destWidth, destHeight, destPitch);

	list<BaseObject*>::iterator objectListIter;

	for (objectListIter = objectList.begin();
		objectListIter != objectList.end();
		++objectListIter)
	{
		(*objectListIter)->Draw(dest, destWidth, destHeight, destPitch);
	}
}

bool LoadSprite()
{
	if (!spriteDib->LoadDibSprite(MAP, L"SpriteData\\_Map.bmp", 0, 0))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_L_01, L"SpriteData\\Attack1_L_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_L_02, L"SpriteData\\Attack1_L_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_L_03, L"SpriteData\\Attack1_L_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_L_04, L"SpriteData\\Attack1_L_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_R_01, L"SpriteData\\Attack1_R_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_R_02, L"SpriteData\\Attack1_R_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_R_03, L"SpriteData\\Attack1_R_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK1_R_04, L"SpriteData\\Attack1_R_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_L_01, L"SpriteData\\Attack2_L_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_L_02, L"SpriteData\\Attack2_L_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_L_03, L"SpriteData\\Attack2_L_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_L_04, L"SpriteData\\Attack2_L_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_R_01, L"SpriteData\\Attack2_R_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_R_02, L"SpriteData\\Attack2_R_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_R_03, L"SpriteData\\Attack2_R_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK2_R_04, L"SpriteData\\Attack2_R_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_L_01, L"SpriteData\\Attack3_L_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_L_02, L"SpriteData\\Attack3_L_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_L_03, L"SpriteData\\Attack3_L_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_L_04, L"SpriteData\\Attack3_L_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_L_05, L"SpriteData\\Attack3_L_05.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_L_06, L"SpriteData\\Attack3_L_06.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_R_01, L"SpriteData\\Attack3_R_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_R_02, L"SpriteData\\Attack3_R_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_R_03, L"SpriteData\\Attack3_R_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_R_04, L"SpriteData\\Attack3_R_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_R_05, L"SpriteData\\Attack3_R_05.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_ATTACK3_R_06, L"SpriteData\\Attack3_R_06.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_HPGUAGE, L"SpriteData\\HPGuage.bmp", 0, 0))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_01, L"SpriteData\\Move_L_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_02, L"SpriteData\\Move_L_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_03, L"SpriteData\\Move_L_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_04, L"SpriteData\\Move_L_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_05, L"SpriteData\\Move_L_05.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_06, L"SpriteData\\Move_L_06.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_07, L"SpriteData\\Move_L_07.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_08, L"SpriteData\\Move_L_08.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_09, L"SpriteData\\Move_L_09.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_10, L"SpriteData\\Move_L_10.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_11, L"SpriteData\\Move_L_11.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_L_12, L"SpriteData\\Move_L_12.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_01, L"SpriteData\\Move_R_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_02, L"SpriteData\\Move_R_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_03, L"SpriteData\\Move_R_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_04, L"SpriteData\\Move_R_04.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_05, L"SpriteData\\Move_R_05.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_06, L"SpriteData\\Move_R_06.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_07, L"SpriteData\\Move_R_07.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_08, L"SpriteData\\Move_R_08.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_09, L"SpriteData\\Move_R_09.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_10, L"SpriteData\\Move_R_10.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_11, L"SpriteData\\Move_R_11.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_MOVE_R_12, L"SpriteData\\Move_R_12.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_SHADOW, L"SpriteData\\Shadow.bmp", 32, 4))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_STAND_L_01, L"SpriteData\\Stand_L_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_STAND_L_02, L"SpriteData\\Stand_L_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_STAND_L_03, L"SpriteData\\Stand_L_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_STAND_R_01, L"SpriteData\\Stand_R_01.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_STAND_R_02, L"SpriteData\\Stand_R_02.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(PLAYER_STAND_R_03, L"SpriteData\\Stand_R_03.bmp", 71, 90))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(EFFECT_SPACK_01, L"SpriteData\\xSpark_1.bmp", 70, 70))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(EFFECT_SPACK_02, L"SpriteData\\xSpark_2.bmp", 70, 70))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(EFFECT_SPACK_03, L"SpriteData\\xSpark_3.bmp", 70, 70))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(EFFECT_SPACK_04, L"SpriteData\\xSpark_4.bmp", 70, 70))
	{
		return false;
	}
	if (!spriteDib->LoadDibSprite(TILE, L"SpriteData\\Tile_01.bmp", 0, 0))
	{
		return false;
	}

	return true;
}