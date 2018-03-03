#include <WinSock2.h>
#include <WS2tcpip.h>
#include <locale.h>
#include <crtdbg.h>
#include <time.h>
#include <mmsystem.h>
#include "main.h"
#include "Select.h"
#include "Sector.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")	// timeBeginPeriod 용도

// 소켓 관련 전역 변수
static SOCKET listenSock;
map<SOCKET, SOCKETINFO*> socketInfoMap;

// 서버를 죽일 때 사용하는 플래그
static bool deadFlag = false;

// 총 소켓 수를 체크하는 변수
int totalSockets = 0;

// 루프 횟수
static unsigned int loopNum = 0;

// 섹터 클래스
Sector sector;

// 로그 클래스
Log printLog;

// cf : check frame - 프레임 시간 검사를 위한 타이머 변수
// sf : show frame - 프레임을 표시하기 위한 타이머 변수
Timer cfTimer, sfTimer, loopTimer;

int wmain()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 146683; // 중단점을 걸어주는 문구, _CrtSetDbgFlag와 세트
	setlocale(LC_ALL, "");

	timeBeginPeriod(1);

	srand((UINT)time(NULL));

	int retVal;

	// 윈속 초기화
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	// socket()
	listenSock = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSock == INVALID_SOCKET)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"socket()");
		return -1;
	}

	// TCP_NODELAY 옵션
	BOOL optval = TRUE;

	if (setsockopt(listenSock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"setsockopt()");
		return -1;
	}

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5000);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	retVal = bind(listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	if (retVal == SOCKET_ERROR)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"bind()");
		return -1;
	}

	// listen()
	retVal = listen(listenSock, SOMAXCONN);

	if (retVal == SOCKET_ERROR)
	{
		printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"listen()");
		return -1;
	}
		
	printLog.ConsolePrint(LOGLEVEL::LOGINFO, L"Server Open!");

	cfTimer.TimerStart();
	sfTimer.TimerStart();
	loopTimer.TimerStart();
		
	do
	{
		Network();
		Action();

		//if (GetKeyState(VK_F8) & 0x0001)	// F8키 토글 상태가 1일 때 (on일 때)
		//{
		//	if (!printPacketLog)
		//	{
		//		printPacketLog = true;
		//		wprintf(L" * printPacketLog = true\n");
		//	}
		//}
		//else // F8키 토글 상태가 0일 때 (off일 때)
		//{
		//	if (printPacketLog)
		//	{
		//		printPacketLog = false;
		//		wprintf(L" * printPacketLog = false\n");
		//	}
		//}

		loopTimer.TimerEnd();

		if (loopTimer.CheckTime() >= 1000)
		{
			double maxLoopInterval = loopTimer.CheckTimeGap(true);

			// 시작 시점의 진동수를 얻는다.
			loopTimer.TimerStart();

			printLog.ConsolePrint(LOGLEVEL::LOGINFO, L"Loop:%d [%.3lfms]", loopNum, maxLoopInterval);

			loopNum = 0;
		}

		++loopNum;

	} while (deadFlag || !((GetAsyncKeyState(VK_F1) & 0x8000) && (GetAsyncKeyState(VK_F12) & 0x8000)));
	
	// 강제로 종료 시에 하는 작업.
	// 서버에 접속한 소켓 끊어주고, 동적할당 해제.
	map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIter;
	map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIterEnd = socketInfoMap.end();

	for (socketInfoMapIter = socketInfoMap.begin();
		socketInfoMapIter != socketInfoMapIterEnd;)
	{
		if (socketInfoMapIter->second != nullptr)
		{
			socketInfoMapIter = RemoveSocketInfo(socketInfoMapIter->first);
		}
		else
		{
			++socketInfoMapIter;
		}
	}

	timeEndPeriod(1);
	
	return 0;
}

void Network()
{
	int retval, addrLen;
	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	FD_SET rset, wset;

	map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIter = socketInfoMap.begin();
	map<SOCKET, SOCKETINFO*>::iterator checkSockSetIter = socketInfoMap.begin();

	int loopNum = totalSockets / 64;

	for (int i = 0; i <= loopNum; i++)
	{
		int socketCount = 1;

		// 소켓 셋 초기화
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		// listenSock을 읽기 셋에 넣는다.
		FD_SET(listenSock, &rset);
		map<SOCKET, SOCKETINFO*>::iterator socketInfoMapEnd = socketInfoMap.end();

		// 현재 접속 중인 모든 클라이언트들의 소켓을 읽기 셋에 넣는다.
		for (socketInfoMapIter; socketInfoMapIter != socketInfoMapEnd; ++socketInfoMapIter)
		{
			FD_SET(socketInfoMapIter->first, &rset);

			if (socketInfoMapIter->second->sendQueue.GetUseSize() > 0)
			{
				FD_SET(socketInfoMapIter->first, &wset);
			}

			++socketCount;

			if (socketCount >= 64)
			{
				++socketInfoMapIter;
				break;
			}
		}

		socketCount = 1;

		// select() 함수를 호출한다.
		timeval time;
		time.tv_sec = 0;
		time.tv_usec = 0;

		retval = select(0, &rset, &wset, NULL, &time);

		if (retval == SOCKET_ERROR)
		{
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"select()");
			return;
		}

		// 소켓 set 검사 : 클라이언트 접속 수용
		// select() 함수가 리턴하면 먼저 읽기 set을 검사하여, 접속한 클라이언트가 있는지 확인한다.
		// 연결 대기 소켓이 읽기 set에 있다면 클라이언트가 접속한 경우이다.
		if (FD_ISSET(listenSock, &rset))
		{
			addrLen = sizeof(clientAddr);

			// accept 함수를 호출한다.
			clientSock = accept(listenSock, (SOCKADDR*)&clientAddr, &addrLen);

			// INVALIED_SOCKET이 리턴되면 오류를 화면에 출력한다.
			if (clientSock == INVALID_SOCKET)
			{
				printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"accept()");
				closesocket(clientSock);
			}
			else
			{
				// AddSocketInfo() 함수를 호출하여 소켓 정보를 추가한다.
				if (AddSocketInfo(clientSock) == false)
				{
					printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"AddSocketinfo(clientSock) == false");
					closesocket(clientSock);
				}
				else // AddSocketInfo(clientSock) == true
				{
					SendCreateMyCharacter(clientSock);
				}
			}
		}

		// 소켓 set 검사 : 데이터 통신
		// select() 함수는 조건을 만족하는 소켓의 개수를 리턴하지만 구체적으로 어떤 소켓인지
		// 가르쳐주지 않으므로, 관리하고 있는 모든 소켓에 대해 소켓 set에 들어 있는지 여부를 확인해야 한다.
		for (checkSockSetIter; checkSockSetIter != socketInfoMapEnd;)
		{
			SOCKET iterSock = checkSockSetIter->first;
			SOCKETINFO* iterSocketInfo = checkSockSetIter->second;

			++socketCount;

			if (socketCount > 64)
			{
				break;
			}

			// 소켓이 읽기 셋에 들어 있다면 recv() 함수를 호출하여 데이터를 읽는다.
			if (FD_ISSET(iterSock, &rset))
			{
				retval = FDReadProc(iterSock, iterSocketInfo);

				if (retval == SOCKET_ERROR)
				{
					if (checkSockSetIter != socketInfoMapEnd)
					{
						checkSockSetIter = RemoveSocketInfo(iterSock);
					}
					else
					{
						++checkSockSetIter;
					}

					continue;
				}
			}

			// 소켓이 쓰기 셋에 들어 있다면 send() 함수를 호출하여 데이터를 쓴다.
			if (FD_ISSET(iterSock, &wset))
			{
				retval = FDWriteProc(iterSock, iterSocketInfo);

				if (retval == SOCKET_ERROR)
				{
					if (checkSockSetIter != socketInfoMapEnd)
					{
						checkSockSetIter = RemoveSocketInfo(iterSock);
					}
					else
					{
						++checkSockSetIter;
					}

					continue;
				}
			}

			++checkSockSetIter;
		}
	}	
}

void Action()
{
	static int printFrame = 0;

	// 끝 시점의 진동수를 얻는다.
	cfTimer.TimerEnd();
		
	if (cfTimer.CheckTime() >= (DEFAULT_FRAME_TIME * FRAME_INTERVAL_OPTION))	// 25 프레임
	{
		// 시작 시점의 진동수를 얻는다.
		cfTimer.TimerStart();

		map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIter;
		map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIterEnd = socketInfoMap.end();

		for (socketInfoMapIter = socketInfoMap.begin();
			socketInfoMapIter != socketInfoMapIterEnd;)
		{
			SOCKETINFO* iterSocketInfo = socketInfoMapIter->second;
			PLAYER **iterPlayer = &(iterSocketInfo->player);
			UINT64 iterSocketNo = iterSocketInfo->socketNo;

			if (*iterPlayer != nullptr && (*iterPlayer)->moveFlag)
			{
				short *iterPlayerPosX = (short*)&((*iterPlayer)->posX);
				short *iterPlayerPosY = (short*)&((*iterPlayer)->posY);
				int iterPlayerID = (*iterPlayer)->userID;
				BYTE iterPlayerDirection = (*iterPlayer)->direction;

				switch (iterPlayerDirection)
				{
				case PACKETDEFINE_MOVE_DIR_LL:
					*iterPlayerPosX -= CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;

					if (*iterPlayerPosX < 0)
					{
						*iterPlayerPosX += CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				case PACKETDEFINE_MOVE_DIR_LU:
					*iterPlayerPosX -= CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					*iterPlayerPosY -= CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

					if (*iterPlayerPosX < 0 || *iterPlayerPosY < 0)
					{
						*iterPlayerPosX += CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
						*iterPlayerPosY += CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				case PACKETDEFINE_MOVE_DIR_LD:
					*iterPlayerPosX -= CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					*iterPlayerPosY += CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

					if (*iterPlayerPosX < 0 || *iterPlayerPosY > MAP_HEIGHT - 1)
					{
						*iterPlayerPosX += CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
						*iterPlayerPosY -= CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				case PACKETDEFINE_MOVE_DIR_RU:
					*iterPlayerPosX += CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					*iterPlayerPosY -= CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

					if (*iterPlayerPosX > MAP_WIDTH - 1 || *iterPlayerPosY < 0)
					{
						*iterPlayerPosX -= CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
						*iterPlayerPosY += CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				case PACKETDEFINE_MOVE_DIR_RR:
					*iterPlayerPosX += CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;

					if (*iterPlayerPosX > MAP_WIDTH - 1)
					{
						*iterPlayerPosX -= CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				case PACKETDEFINE_MOVE_DIR_RD:
					*iterPlayerPosX += CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					*iterPlayerPosY += CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					
					if (*iterPlayerPosX > MAP_WIDTH - 1 || *iterPlayerPosY > MAP_HEIGHT - 1)
					{
						*iterPlayerPosX -= CHARACTER_LR_MOVE_RANGE * FRAME_INTERVAL_OPTION;
						*iterPlayerPosY -= CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				case PACKETDEFINE_MOVE_DIR_UU:
					*iterPlayerPosY -= CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

					if (*iterPlayerPosY < 0)
					{
						*iterPlayerPosY += CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				case PACKETDEFINE_MOVE_DIR_DD:
					*iterPlayerPosY += CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;

					if (*iterPlayerPosY > MAP_HEIGHT - 1)
					{
						*iterPlayerPosY -= CHARACTER_UD_MOVE_RANGE * FRAME_INTERVAL_OPTION;
					}
					break;
				default:
					printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"Action() - MOVE_DIR Error [SOCKET_NO:%llu]",
						iterSocketNo);

					socketInfoMapIter = RemoveSocketInfo(socketInfoMapIter->first);
					continue;
				}

				sector.CheckSector(iterSocketInfo);
				
				/*printLog.ConsolePrint(LOGLEVEL::LOGDEBUG, L"Action() - MovePos [SOCKET_NO:%llu] x:%d, y:%d",
					iterSocketNo, *iterPlayerPosX, *iterPlayerPosY);*/
			}

			++socketInfoMapIter;
		}

		++printFrame;
	}
		
	// 끝 시점의 진동수를 얻는다.
	sfTimer.TimerEnd();

	// 1초마다 프레임을 출력한다.
	if (sfTimer.CheckTime() >= 1000)
	{
		double maxFrameInterval = sfTimer.CheckTimeGap(true);

		// 시작 시점의 진동수를 얻는다.
		sfTimer.TimerStart();

		printLog.ConsolePrint(LOGLEVEL::LOGINFO,
			L"Frame:%d [%.3lfms], SocketNum:%d", 
			printFrame, maxFrameInterval, totalSockets);

		printFrame = 0;
	}
}