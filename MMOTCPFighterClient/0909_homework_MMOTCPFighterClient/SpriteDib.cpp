#include "stdafx.h"
#include "SpriteDib.h"

SpriteDib::SpriteDib(int maxSprite, DWORD colorKey)
{
	// �ִ�� �о�� ���� ��ŭ �̸� �Ҵ�޴´�.
	sprite = new Sprite[maxSprite];
	
	memset(sprite, NULL, sizeof(Sprite) * maxSprite);

	this->maxSprite = maxSprite;
	this->colorKeyColor = colorKey;
}

SpriteDib::~SpriteDib()
{
	// ��ü�� ���鼭 ��� �����ش�.
	for (int i = 0; i < maxSprite; i++)
	{
		ReleaseSprite(i);
	}

	delete[] sprite;
}

// BMP ������ �о �ϳ��� ���������� �����Ѵ�.
// Parameters : (int)spriteIndex, (char*)fileName, (int)centerPointX, (int)centerPointY
// Return : (BOOL)true, false
BOOL SpriteDib::LoadDibSprite(int spriteIndex, WCHAR* fileName, int centerPointX, int centerPointY)
{
	HANDLE file;
	DWORD read;
	int pitch, imageSize;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	// ��Ʈ�� ����� ���� BMP ���� Ȯ��
	file = CreateFile((LPCWSTR)fileName, GENERIC_READ, NULL, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == file)
	{
		return false;
	}

	ReadFile(file, &fileHeader, sizeof(BITMAPFILEHEADER), &read, NULL);

	if (fileHeader.bfType != 0x4D42)
	{
		return false;
	}

	ReadFile(file, &infoHeader, sizeof(BITMAPINFOHEADER), &read, NULL);

	// �� ��, �� ���� ��ġ���� ���Ѵ�.
	pitch = infoHeader.biWidth * infoHeader.biBitCount / 8;

	// ��������Ʈ ����ü�� ũ�� ����.
	sprite[spriteIndex].width = infoHeader.biWidth;
	sprite[spriteIndex].height = infoHeader.biHeight;
	sprite[spriteIndex].pitch = pitch;
	sprite[spriteIndex].bitCount = infoHeader.biBitCount;
	sprite[spriteIndex].centerPointX = centerPointX;
	sprite[spriteIndex].centerPointY = centerPointY;

	// �̹����� ���� ��ü ũ�⸦ ���ϰ�, �޸� �Ҵ�.
	imageSize = pitch * infoHeader.biHeight;
	sprite[spriteIndex].byteImage = new BYTE[imageSize];
	memset(sprite[spriteIndex].byteImage, 0, imageSize);

	// �̹��� �κ��� ��������Ʈ ���۷� �о�´�.
	// DIB�� �������� �����Ƿ� �̸� �ٽ� ������.
	// �ӽ� ���ۿ� ���� �ڿ� �������鼭 �����Ѵ�.
	BYTE* tmpBuffer = new BYTE[imageSize];
	memset(tmpBuffer, 0, imageSize);

	ReadFile(file, tmpBuffer, imageSize, &read, NULL);

	for (int i = 0; i < infoHeader.biHeight; i++)
	{
		memcpy(sprite[spriteIndex].byteImage + i * pitch, 
			tmpBuffer + (pitch * (infoHeader.biHeight - 1 - i)), pitch);
	}

	delete tmpBuffer;
	
	CloseHandle(file);
	return true;
}

// �ش� ��������Ʈ ����.
// Parameters : (int)spriteIndex
// Return : (BOOL)true, false
void SpriteDib::ReleaseSprite(int spriteIndex)
{
	// �ִ� �Ҵ�� ��������Ʈ�� �Ѿ�� �� �ȴ�.
	if (spriteIndex >= maxSprite)
	{
		return;
	}

	// == �񱳹����� ����� ���ʿ� ���� ��, = ��ȣ�� �ϳ� �� �� ���� �����Ϸ��� ����ش�.
	if (NULL != sprite[spriteIndex].byteImage)
	{
		// ���� �� �ʱ�ȭ
		delete[] sprite[spriteIndex].byteImage; 
		memset(&sprite[spriteIndex], 0, sizeof(Sprite));
	}
}

// Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ�. (�÷�Ű, Ŭ���� ó��)
void SpriteDib::DrawSprite(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{	
	// �ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}
	
	// ���������� �ʿ� ���� ����
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	
	int bitNum = sprite[spriteIndex].bitCount / 8;

	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = srcWidth * bitNum;
	
	// ��� �������� ��ǥ ���
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// ��ܿ� ���� ��������Ʈ ��� ��ġ ���. (��� Ŭ����)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// �ϴ� ��� ��ġ ���. (�ϴ� Ŭ����)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// ���� ��� ��ġ ���. (���� Ŭ����)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// ������ ��� ��ġ ���. (���� Ŭ����)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;
	
	// ��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (int i = 0; i < srcHeight; i++)
	{
		dest = (DWORD*)destOrigin;
		src = (DWORD*)srcOrigin;

		for (int j = 0; j < srcWidth; j++)
		{
			// �÷�Ű�� �ƴ� �� ȭ�鿡 ��� ���� ĭ���� �̵�.
			if ((*src & 0x00ffffff) != 0x00ffffff)
			{
				*dest = *src;
			}
			
			dest++;
			src++;
		}

		// ȭ��� ��������Ʈ ��� ���� �ٷ� �̵�.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}

// Ư�� �޸� ��ġ�� ��������Ʈ�� ���������� ����Ѵ�. (�÷�Ű, Ŭ���� ó��)
void SpriteDib::DrawSpriteTranslucence(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{
	// �ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}

	// ���������� �ʿ� ���� ����
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = sprite[spriteIndex].pitch * drawLen / SCREENDIB_DRAWLEN_MAX;

	int bitNum = sprite[spriteIndex].bitCount / 8;

	// ��� �������� ��ǥ ���
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// ��ܿ� ���� ��������Ʈ ��� ��ġ ���. (��� Ŭ����)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// �ϴ� ��� ��ġ ���. (�ϴ� Ŭ����)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// ���� ��� ��ġ ���. (���� Ŭ����)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// ������ ��� ��ġ ���. (���� Ŭ����)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;

	// ��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (int i = 0; i < srcHeight; i++)
	{
		dest = (DWORD*)destOrigin;
		src = (DWORD*)srcOrigin;

		for (int j = 0; j < srcWidth; j++)
		{
			// �÷�Ű�� �ƴ� �� ȭ�鿡 ��� ���� ĭ���� �̵�.
			if ((*src & 0x00ffffff) != 0x00ffffff)
			{
				BYTE* tmpDest = (BYTE*)dest;
				BYTE* tmpSrc = (BYTE*)src;

				for (int k = 0; k < bitNum; k++)
				{
					*(tmpDest + k) = (*(tmpDest + k) + *(tmpSrc + k)) / 2;
				}

				dest = (DWORD*)tmpDest;
			}

			dest++;
			src++;
		}

		// ȭ��� ��������Ʈ ��� ���� �ٷ� �̵�.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}

// Ư�� �޸� ��ġ�� ��������Ʈ�� �Ӱ� ����Ѵ�. (�÷�Ű, Ŭ���� ó��) 
void SpriteDib::DrawSpriteRed(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{
	// �ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}

	// ���������� �ʿ� ���� ����
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = sprite[spriteIndex].pitch * drawLen / SCREENDIB_DRAWLEN_MAX;

	int bitNum = sprite[spriteIndex].bitCount / 8;

	// ��� �������� ��ǥ ���
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// ��ܿ� ���� ��������Ʈ ��� ��ġ ���. (��� Ŭ����)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// �ϴ� ��� ��ġ ���. (�ϴ� Ŭ����)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// ���� ��� ��ġ ���. (���� Ŭ����)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// ������ ��� ��ġ ���. (���� Ŭ����)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;

	// ��ü ũ�⸦ ���鼭 �ȼ����� ����� ó���� �ϸ� �׸� ���.
	for (int i = 0; i < srcHeight; i++)
	{
		dest = (DWORD*)destOrigin;
		src = (DWORD*)srcOrigin;

		for (int j = 0; j < srcWidth; j++)
		{
			// �÷�Ű�� �ƴ� �� ȭ�鿡 ��� ���� ĭ���� �̵�.
			if ((*src & 0x00ffffff) != 0x00ffffff)
			{
				BYTE* tmpDest = (BYTE*)dest;
				BYTE* tmpSrc = (BYTE*)src;
				
				*(tmpDest + 0) = *(tmpSrc + 0) / 2;
				*(tmpDest + 1) = *(tmpSrc + 1) / 2;
				*(tmpDest + 2) = *(tmpSrc + 2);

				dest = (DWORD*)tmpDest;
			}

			dest++;
			src++;
		}

		// ȭ��� ��������Ʈ ��� ���� �ٷ� �̵�.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}

// Ư�� �޸� ��ġ�� �̹����� ����Ѵ�. (Ŭ���� ó��)
void SpriteDib::DrawImage(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{
	// �ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}

	// ���������� �ʿ� ���� ����
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = sprite[spriteIndex].pitch * drawLen / SCREENDIB_DRAWLEN_MAX;


	int bitNum = sprite[spriteIndex].bitCount / 8;

	// ��� �������� ��ǥ ���
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// ��ܿ� ���� ��������Ʈ ��� ��ġ ���. (��� Ŭ����)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// �ϴ� ��� ��ġ ���. (�ϴ� Ŭ����)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// ���� ��� ��ġ ���. (���� Ŭ����)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// ������ ��� ��ġ ���. (���� Ŭ����)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// ȭ�鿡 ���� ��ġ�� �̵��Ѵ�.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;

	// ��ü ũ�⸦ ���鼭 �ȼ����� �׸� ���.
	for (int i = 0; i < srcHeight; i++)
	{
		dest = (DWORD*)destOrigin;
		src = (DWORD*)srcOrigin;

		for (int j = 0; j < srcWidth; j++)
		{
			*dest = *src;

			dest++;
			src++;
		}

		// ȭ��� ��������Ʈ ��� ���� �ٷ� �̵�.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}