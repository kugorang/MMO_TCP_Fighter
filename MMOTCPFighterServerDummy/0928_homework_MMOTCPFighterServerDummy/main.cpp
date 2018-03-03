#include <WinSock2.h>
#include <WS2tcpip.h>
#include <locale.h>
#include <crtdbg.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include "main.h"
#include "Select.h"

#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")	// timeBeginPeriod 용도

// 소켓 관련 전역 변수
map<SOCKET, SOCKETINFO*> socketInfoMap;

UINT successSocket = 0, failSocket = 0;
UINT totalSocket = 0;

UINT recvBytes, recvPacketNum;
UINT sendBytes, sendPacketNum;

UINT selectNum = 0;

// 서버를 죽일 때 사용하는 플래그
static bool deadFlag = false;

// 로그 클래스
Log printLog;

// 프레임 타이머
Timer frameTimer, selectTimer;

int wmain()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 64741; // 중단점을 걸어주는 문구, _CrtSetDbgFlag와 세트
	setlocale(LC_ALL, "");

	timeBeginPeriod(1);

	srand((unsigned int)time(NULL));

	int retval;

	// 윈속 초기화
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	for (int i = 0; i < SOCKETINFO_ARRAY_MAX; i++)
	{
		++totalSocket;
		SOCKETINFO *socketInfo = new SOCKETINFO;

		SOCKET linkSock;

		// socket()
		linkSock = socket(AF_INET, SOCK_STREAM, 0);

		if (linkSock == INVALID_SOCKET)
		{
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"socket()");

			++failSocket;
			closesocket(linkSock);
		}
		else
		{
			// TCP_NODELAY 옵션
			BOOL optval = TRUE;

			if (setsockopt(linkSock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval))
				== SOCKET_ERROR)
			{
				printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"setsockopt()");

				++failSocket;
				closesocket(linkSock);
			}
			else
			{
				// connect()
				SOCKADDR_IN serverAddr;
				ZeroMemory(&serverAddr, sizeof(serverAddr));

				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(5000);
				InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr.s_addr);

				retval = connect(linkSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

				if (retval == SOCKET_ERROR)
				{
					printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"connect()");

					++failSocket;
					closesocket(linkSock);
				}
				else
				{
					static int socketNo = 1;

					printLog.ConsolePrint(LOGLEVEL::LOGINFO,
						L"Number - %d Client Connect", i + 1);

					++successSocket;

					socketInfo->clientSock = linkSock;
					socketInfo->socketNo = socketNo++;
					socketInfo->player = nullptr;

					socketInfoMap.insert(socketInfoMap.begin(),
						map<SOCKET, SOCKETINFO*>::value_type(linkSock, socketInfo));

					socketInfo->playerTimer.TimerStart();
				}
			}
		}
	}

	frameTimer.TimerStart();
	selectTimer.TimerStart();

	do
	{
		Action();
		Network();

		if ((GetAsyncKeyState(VK_F2) & 0x8000) && (GetAsyncKeyState(VK_F3) & 0x8000))
		{
			break;
		}
	} while (!deadFlag);

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

	WSACleanup();

	timeEndPeriod(1);

	return 0;
}

void Action()
{
	int actionNum = 0, loopNum = 0;
	static int iterPos = 0;

	map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIter;
	map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIterBegin = socketInfoMap.begin();
	map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIterEnd = socketInfoMap.end();

	for (socketInfoMapIter = socketInfoMapIterBegin;
		loopNum < iterPos;
		++socketInfoMapIter)
	{
		if (socketInfoMapIter == socketInfoMapIterEnd)
		{
			socketInfoMapIter = socketInfoMapIterBegin;
		}

		++loopNum;
	}

	for (socketInfoMapIter;
		actionNum < 100;)
	{
		if (socketInfoMapIter == socketInfoMapIterEnd)
		{
			socketInfoMapIter = socketInfoMapIterBegin;
		}

		SOCKETINFO *socketInfo = socketInfoMapIter->second;

		// si = socketInfo
		PLAYER *siPlayer = socketInfo->player;

		if (siPlayer == nullptr)
		{
			++actionNum;
			++socketInfoMapIter;
			++iterPos;
			continue;
		}

		SOCKET clientSock = socketInfoMapIter->first;

		// si : socketInfo
		Timer *siTimer = &(socketInfo->playerTimer);

		siTimer->TimerEnd();

		UINT aiRand = rand() % 2000 + 3000;

		if (siTimer->CheckTime() >= aiRand)
		{
			siTimer->TimerStart();

			int actionRand = rand() % 3;
			int attackRand = rand() % 3;

			switch (actionRand)
			{
			case 0:	// 이동 시작
				SendMoveStart(clientSock);
				break;
			case 1:	// 이동 중지
				if (!siPlayer->moveFlag)
				{
					break;
				}

				SendMoveStop(clientSock);
				break;
			case 2:	// 공격
				switch (attackRand)
				{
				case 0:	// Attack1
					SendAttack1(clientSock);
					break;
				case 1:	// Attack2
					SendAttack2(clientSock);
					break;
				case 2:	// Attack3				
					SendAttack3(clientSock);
					break;
				default:
					printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"Action() - attackRand Error");
					--successSocket;
					++failSocket;
					socketInfoMapIter = RemoveSocketInfo(clientSock);

					++actionNum;
					++socketInfoMapIter;
					++iterPos;
					continue;
				}

				if (siPlayer->moveFlag)
				{
					siPlayer->moveFlag = false;
					SendMoveStop(clientSock);
				}

				break;
			default:
				printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"Action() - actionRand Error");
				--successSocket;
				++failSocket;
				socketInfoMapIter = RemoveSocketInfo(clientSock);

				++actionNum;
				++socketInfoMapIter;
				++iterPos;
				continue;
			}
		}

		++actionNum;
		++socketInfoMapIter;
		++iterPos;

		if (iterPos > SOCKETINFO_ARRAY_MAX - 1)
		{
			iterPos = 0;
		}
	}

	// 끝 시점의 진동수를 얻는다.
	frameTimer.TimerEnd();

	if (frameTimer.CheckTime() >= 1000)
	{
		system("cls");

		// 시작 시점의 진동수를 얻는다.
		frameTimer.TimerStart();

		printLog.ConsolePrint(LOGLEVEL::LOGINFO, L"Total - %d, Success - %d, Fail - %d",
			totalSocket, successSocket, failSocket);
	}
}

void Network()
{
	int retval, loopNum = totalSocket / 64;
	FD_SET rset, wset;

	map<SOCKET, SOCKETINFO*>::iterator socketInfoMapIter = socketInfoMap.begin();
	map<SOCKET, SOCKETINFO*>::iterator checkSockSetIter = socketInfoMap.begin();

	for (int i = 0; i <= loopNum; i++)
	{
		int socketCount = 0;

		// 소켓 셋 초기화
		FD_ZERO(&rset);
		FD_ZERO(&wset);

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

		socketCount = 0;

		// select() 함수를 호출한다.
		timeval time;
		time.tv_sec = 0;
		time.tv_usec = 0;

		retval = select(0, &rset, &wset, NULL, &time);

		if (retval == SOCKET_ERROR)
		{
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"select()");
			deadFlag = true;
			return;
		}
		else
		{
			selectTimer.TimerEnd();

			if (selectTimer.CheckTime() >= 1000)
			{
				selectTimer.TimerStart();

				printLog.ConsolePrint(LOGLEVEL::LOGINFO, L"Select Num - %d", selectNum);

				selectNum = 0;
			}

			++selectNum;
		}

		// 소켓 set 검사 : 데이터 통신
		// select() 함수는 조건을 만족하는 소켓의 개수를 리턴하지만 구체적으로 어떤 소켓인지
		// 가르쳐주지 않으므로, 관리하고 있는 모든 소켓에 대해 소켓 set에 들어 있는지 여부를 확인해야 한다.
		for (checkSockSetIter; checkSockSetIter != socketInfoMapEnd;)
		{
			SOCKET sock = checkSockSetIter->first;
			SOCKETINFO* socketInfo = checkSockSetIter->second;

			++socketCount;

			if (socketCount > 64)
			{
				break;
			}

			// 소켓이 읽기 셋에 들어 있다면 recv() 함수를 호출하여 데이터를 읽는다.
			if (FD_ISSET(sock, &rset))
			{
				retval = FDReadProc(sock, socketInfo);

				if (retval == SOCKET_ERROR)
				{
					--successSocket;
					++failSocket;

					if (checkSockSetIter != socketInfoMapEnd)
					{
						checkSockSetIter = RemoveSocketInfo(sock);
					}
					else
					{
						++checkSockSetIter;
					}

					continue;
				}
			}

			// 소켓이 쓰기 셋에 들어 있다면 send() 함수를 호출하여 데이터를 쓴다.
			if (FD_ISSET(sock, &wset))
			{
				retval = FDWriteProc(sock, socketInfo);

				if (retval == SOCKET_ERROR)
				{
					--successSocket;
					++failSocket;

					if (checkSockSetIter != socketInfoMapEnd)
					{
						checkSockSetIter = RemoveSocketInfo(sock);
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