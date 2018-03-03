#pragma once

#include <Windows.h>
#include <list>

#define SECTOR_WIDTH 640
#define SECTOR_HEIGHT 640

#define SECTOR_WIDTH_MAX (MAP_WIDTH / SECTOR_WIDTH)
#define SECTOR_HEIGHT_MAX (MAP_HEIGHT / SECTOR_HEIGHT)

enum SECTORFLAG
{
	MOVE,
	DIE,
	CONNECTION,
	DISCONNECTION
};

class Sector
{
private:	
	int insertSector[3][3];
	int removeSector[3][3];

	list<SOCKETINFO*> playerList[SECTOR_HEIGHT_MAX][SECTOR_WIDTH_MAX];
public:
	Sector();
	~Sector();

	void CheckSector(SOCKETINFO* checkSocketInfo);
	void CheckAroundSector(int oldIndexX, int oldIndexY, int newIndexX, int newIndexY);

	void InsertSector(SOCKETINFO* insertSocketInfo, int sectorIndexX, 
		int sectorIndexY, int sectorFlag);
	void DeleteSector(SOCKETINFO* deleteSocketInfo, int posXIndex, 
		int posYIndex, int sectorFlag);
	
	list<SOCKETINFO*>* FindSector(int sectorIndexX, int sectorIndexY);
};