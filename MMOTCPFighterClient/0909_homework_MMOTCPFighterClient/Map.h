#pragma once

#define MAP_TILE_SIZE 64

#define MAP_RANGE_TOP 0
#define MAP_RANGE_BOTTOM 6399
#define MAP_RANGE_LEFT 0
#define MAP_RANGE_RIGHT 6399

#define MAP_ARRAY_WIDTH 100
#define MAP_ARRAY_HEIGHT 100

#define SCREEN_TILE_WIDTH 11
#define SCREEN_TILE_HEIGHT 9

class Map
{
private:
	int scrollX, scrollY;

	struct Tile
	{
		int xPos, yPos, xIndex, yIndex;
	};

	Tile tile[MAP_ARRAY_HEIGHT][MAP_ARRAY_WIDTH];
public:
	Map();
	~Map();

	void Draw(BYTE* dest, int destWidth, int destHeight, int destPitch);

	void CheckScrollX(int* scrollX);
	void CheckScrollY(int* scrollY);

	void SetScrollX(int scrollX);
	void SetScrollY(int scrollY);

	int GetScrollX();
	int GetScrollY();
};