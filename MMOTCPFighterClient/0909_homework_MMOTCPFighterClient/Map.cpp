#include "stdafx.h"
#include "Map.h"
#include "Player.h"
#include "0909_homework_MMOTCPFighterClient.h"

Map::Map()
{
	memset(tile, 0, sizeof(tile));

	for (int y = 0; y < MAP_ARRAY_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_ARRAY_WIDTH; x++)
		{
			tile[y][x].xPos = x * MAP_TILE_SIZE;
			tile[y][x].yPos = y * MAP_TILE_SIZE;
			tile[y][x].xIndex = x;
			tile[y][x].yIndex = y;
		}
	}

	scrollX = -1;
	scrollY = -1;
}

Map::~Map()
{

}

void Map::Draw(BYTE* dest, int destWidth, int destHeight, int destPitch)
{
	int xPos = tile[scrollY / MAP_TILE_SIZE][scrollX / MAP_TILE_SIZE].xPos;
	int yPos = tile[scrollY / MAP_TILE_SIZE][scrollX / MAP_TILE_SIZE].yPos;
	
	xPos -= scrollX;
	yPos -= scrollY;

	memset(drawMapIndex, 0, sizeof(drawMapIndex));
	
	for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
		{
			spriteDib->DrawImage(TILE, xPos + x * MAP_TILE_SIZE, yPos + y * MAP_TILE_SIZE,
				dest, destWidth, destHeight, destPitch);

			drawMapIndex[y][x].xPos = xPos + x * MAP_TILE_SIZE;
			drawMapIndex[y][x].yPos = yPos + y * MAP_TILE_SIZE;

			int xIndex = (scrollX + x * MAP_TILE_SIZE) / MAP_TILE_SIZE;
			int yIndex = (scrollY + y * MAP_TILE_SIZE) / MAP_TILE_SIZE;

			wsprintf(drawMapIndex[y][x].xIndex, TEXT("%d"), xIndex);
			wsprintf(drawMapIndex[y][x].yIndex, TEXT("%d"), yIndex);
		}
	}
}

void Map::CheckScrollX(int* scrollX)
{
	if (myPlayer == nullptr || myPlayer->IsMoveRangeOver())
	{
		return;
	}

	int myPlayerXPos = myPlayer->GetXPos();

	if (myPlayerXPos < SCREEN_WIDTH / 2)
	{
		*scrollX = 0;
	}
	
	if (myPlayerXPos > MAP_TILE_SIZE * MAP_ARRAY_WIDTH - SCREEN_WIDTH / 2)
	{
		*scrollX = MAP_TILE_SIZE * MAP_ARRAY_WIDTH - SCREEN_WIDTH;
	}
}

void Map::CheckScrollY(int* scrollY)
{
	if (myPlayer == nullptr || myPlayer->IsMoveRangeOver())
	{
		return;
	}

	int myPlayerYPos = myPlayer->GetYPos();

	if (myPlayerYPos < SCREEN_HEIGHT / 2 + 25)
	{

		*scrollY = 0;
	}

	if (myPlayerYPos > MAP_TILE_SIZE * MAP_ARRAY_HEIGHT - SCREEN_HEIGHT / 2 + 25)
	{
		*scrollY = MAP_TILE_SIZE * MAP_ARRAY_HEIGHT - SCREEN_HEIGHT;
	}	
}

void Map::SetScrollX(int scrollX)
{
	this->scrollX = scrollX;

	CheckScrollX(&(this->scrollX));
}

void Map::SetScrollY(int scrollY)
{
	this->scrollY = scrollY;

	CheckScrollY(&(this->scrollY));
}

int Map::GetScrollX()
{
	return scrollX;
}

int Map::GetScrollY()
{
	return scrollY;
}