#pragma once

#include "stdafx.h"

const int SCREENDIB_BIT_COUNT = 32;
const int SCREENDIB_DRAWLEN_MAX = 100;

class SpriteDib
{
protected:
	// DIB ��������Ʈ ����ü.
	// ��������Ʈ �̹����� ������ ������ ����.
	struct Sprite
	{
		BYTE* byteImage;				// ��������Ʈ �̹��� ������.
		int width, height, pitch;		// �ʺ�, ����, ��ġ
		int bitCount;

		// ������ �ش� ��������Ʈ�� ����ϱ� ���� �������̴�.
		// ��� ĳ������ ��� ���� ���� ��ġ�� �������� ��´�.
		// ���� ���� ���� ĳ���� ��ǥ�� ĳ���Ͱ� �� �ִ� ��ġ�̸�, �׸� ����� ������ ���� �̹����� ����Ѵ�.
		int centerPointX, centerPointY;	// ���� x, ���� y
	};

	// Sprite �迭 ����.
	int maxSprite;
	Sprite* sprite;

	// �÷�Ű �������� ����� ��.
	DWORD colorKeyColor;
public:
	// Parameters : (int)�ִ� ��������Ʈ ����, (DWORD)�÷�Ű
	SpriteDib(int maxSprite, DWORD colorKey);
	virtual ~SpriteDib();

	// BMP ������ �о �ϳ��� ���������� �����Ѵ�.
	BOOL LoadDibSprite(int spriteIndex, WCHAR* fileName, int centerPointX, int centerPointY);

	// �ش� ��������Ʈ ����.
	void ReleaseSprite(int spriteIndex);

	// Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (�÷�Ű, Ŭ���� ó��)
	void DrawSprite(int spriteIndex, int drawX, int drawY, BYTE* byteDest, 
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);

	// Ư�� �޸� ��ġ�� ��������Ʈ�� ���������� ����Ѵ�. (�÷�Ű, Ŭ���� ó��) 
	void DrawSpriteTranslucence(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);

	// Ư�� �޸� ��ġ�� ��������Ʈ�� �Ӱ� ����Ѵ�. (�÷�Ű, Ŭ���� ó��) 
	void DrawSpriteRed(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);

	// Ư�� �޸� ��ġ�� �̹����� ����Ѵ�. (Ŭ���� ó��)
	void DrawImage(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
		int destWidth, int destHeight, int destPitch, int drawLen = SCREENDIB_DRAWLEN_MAX);
};