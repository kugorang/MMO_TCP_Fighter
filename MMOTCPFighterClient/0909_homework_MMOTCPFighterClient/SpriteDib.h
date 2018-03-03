#pragma once

#include "stdafx.h"

const int SCREENDIB_BIT_COUNT = 32;
const int SCREENDIB_DRAWLEN_MAX = 100;

class SpriteDib
{
protected:
	// DIB 스프라이트 구조체.
	// 스프라이트 이미지와 사이즈 정보를 가짐.
	struct Sprite
	{
		BYTE* byteImage;				// 스프라이트 이미지 포인터.
		int width, height, pitch;		// 너비, 높이, 피치
		int bitCount;

		// 중점은 해당 스프라이트를 출력하기 위한 기준점이다.
		// 사람 캐릭터의 경우 보통 땅의 위치를 중점으로 잡는다.
		// 게임 로직 상의 캐릭터 좌표는 캐릭터가 서 있는 위치이며, 그림 출력은 중점을 빼서 이미지를 출력한다.
		int centerPointX, centerPointY;	// 중점 x, 중점 y
	};

	// Sprite 배열 정보.
	int maxSprite;
	Sprite* sprite;

	// 컬러키 색상으로 사용할 색.
	DWORD colorKeyColor;
public:
	// Parameters : (int)최대 스프라이트 개수, (DWORD)컬러키
	SpriteDib(int maxSprite, DWORD colorKey);
	virtual ~SpriteDib();

	// BMP 파일을 읽어서 하나의 프레임으로 저장한다.
	BOOL LoadDibSprite(int spriteIndex, WCHAR* fileName, int centerPointX, int centerPointY);

	// 해당 스프라이트 해제.
	void ReleaseSprite(int spriteIndex);

	// 특정 메모리 위치에 스프라이트를 출력한다. (컬러키, 클리핑 처리)
	void DrawSprite(int spriteIndex, int drawX, int drawY, BYTE* byteDest, 
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);

	// 특정 메모리 위치에 스프라이트를 반투명으로 출력한다. (컬러키, 클리핑 처리) 
	void DrawSpriteTranslucence(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);

	// 특정 메모리 위치에 스프라이트를 붉게 출력한다. (컬러키, 클리핑 처리) 
	void DrawSpriteRed(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);

	// 특정 메모리 위치에 이미지를 출력한다. (클리핑 처리)
	void DrawImage(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);
};