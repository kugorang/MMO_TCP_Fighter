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
#pragma comment(lib, "winmm.lib")	// timeBeginPeriod �뵵

// ���� ���� ���� ����
static SOCKET listenSock;
map<SOCKET, SOCKETINFO*> socketInfoMap;

// ������ ���� �� ����ϴ� �÷���
static bool deadFlag = false;

// �� ���� ���� üũ�ϴ� ����
int totalSockets = 0;

// ���� Ƚ��
static unsigned int loopNum = 0;

// ���� Ŭ����
Sector sector;

// �α� Ŭ����
Log printLog;

// cf : check frame - ������ �ð� �˻縦 ���� Ÿ�̸� ����
// sf : show frame - �������� ǥ���ϱ� ���� Ÿ�̸� ����
Timer cfTimer, sfTimer, loopTimer;

int wmain()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 146683; // �ߴ����� �ɾ��ִ� ����, _CrtSetDbgFlag�� ��Ʈ
	setlocale(LC_ALL, "");

	timeBeginPeriod(1);

	srand((UINT)time(NULL));

	int retVal;

	// ���� �ʱ�ȭ
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

	// TCP_NODELAY �ɼ�
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

		//if (GetKeyState(VK_F8) & 0x0001)	// F8Ű ��� ���°� 1�� �� (on�� ��)
		//{
		//	if (!printPacketLog)
		//	{
		//		printPacketLog = true;
		//		wprintf(L" * printPacketLog = true\n");
		//	}
		//}
		//else // F8Ű ��� ���°� 0�� �� (off�� ��)
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

			// ���� ������ �������� ��´�.
			loopTimer.TimerStart();

			printLog.ConsolePrint(LOGLEVEL::LOGINFO, L"Loop:%d [%.3lfms]", loopNum, maxLoopInterval);

			loopNum = 0;
		}

		++loopNum;

	} while (deadFlag || !((GetAsyncKeyState(VK_F1) & 0x8000) && (GetAsyncKeyState(VK_F12) & 0x8000)));
	
	// ������ ���� �ÿ� �ϴ� �۾�.
	// ������ ������ ���� �����ְ�, �����Ҵ� ����.
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

		// ���� �� �ʱ�ȭ
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		// listenSock�� �б� �¿� �ִ´�.
		FD_SET(listenSock, &rset);
		map<SOCKET, SOCKETINFO*>::iterator socketInfoMapEnd = socketInfoMap.end();

		// ���� ���� ���� ��� Ŭ���̾�Ʈ���� ������ �б� �¿� �ִ´�.
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

		// select() �Լ��� ȣ���Ѵ�.
		timeval time;
		time.tv_sec = 0;
		time.tv_usec = 0;

		retval = select(0, &rset, &wset, NULL, &time);

		if (retval == SOCKET_ERROR)
		{
			printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"select()");
			return;
		}

		// ���� set �˻� : Ŭ���̾�Ʈ ���� ����
		// select() �Լ��� �����ϸ� ���� �б� set�� �˻��Ͽ�, ������ Ŭ���̾�Ʈ�� �ִ��� Ȯ���Ѵ�.
		// ���� ��� ������ �б� set�� �ִٸ� Ŭ���̾�Ʈ�� ������ ����̴�.
		if (FD_ISSET(listenSock, &rset))
		{
			addrLen = sizeof(clientAddr);

			// accept �Լ��� ȣ���Ѵ�.
			clientSock = accept(listenSock, (SOCKADDR*)&clientAddr, &addrLen);

			// INVALIED_SOCKET�� ���ϵǸ� ������ ȭ�鿡 ����Ѵ�.
			if (clientSock == INVALID_SOCKET)
			{
				printLog.ConsolePrint(LOGLEVEL::LOGERROR, L"accept()");
				closesocket(clientSock);
			}
			else
			{
				// AddSocketInfo() �Լ��� ȣ���Ͽ� ���� ������ �߰��Ѵ�.
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

		// ���� set �˻� : ������ ���
		// select() �Լ��� ������ �����ϴ� ������ ������ ���������� ��ü������ � ��������
		// ���������� �����Ƿ�, �����ϰ� �ִ� ��� ���Ͽ� ���� ���� set�� ��� �ִ��� ���θ� Ȯ���ؾ� �Ѵ�.
		for (checkSockSetIter; checkSockSetIter != socketInfoMapEnd;)
		{
			SOCKET iterSock = checkSockSetIter->first;
			SOCKETINFO* iterSocketInfo = checkSockSetIter->second;

			++socketCount;

			if (socketCount > 64)
			{
				break;
			}

			// ������ �б� �¿� ��� �ִٸ� recv() �Լ��� ȣ���Ͽ� �����͸� �д´�.
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

			// ������ ���� �¿� ��� �ִٸ� send() �Լ��� ȣ���Ͽ� �����͸� ����.
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

	// �� ������ �������� ��´�.
	cfTimer.TimerEnd();
		
	if (cfTimer.CheckTime() >= (DEFAULT_FRAME_TIME * FRAME_INTERVAL_OPTION))	// 25 ������
	{
		// ���� ������ �������� ��´�.
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
		
	// �� ������ �������� ��´�.
	sfTimer.TimerEnd();

	// 1�ʸ��� �������� ����Ѵ�.
	if (sfTimer.CheckTime() >= 1000)
	{
		double maxFrameInterval = sfTimer.CheckTimeGap(true);

		// ���� ������ �������� ��´�.
		sfTimer.TimerStart();

		printLog.ConsolePrint(LOGLEVEL::LOGINFO,
			L"Frame:%d [%.3lfms], SocketNum:%d", 
			printFrame, maxFrameInterval, totalSockets);

		printFrame = 0;
	}
}