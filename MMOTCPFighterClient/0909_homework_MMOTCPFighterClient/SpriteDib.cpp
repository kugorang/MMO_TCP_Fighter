#include "stdafx.h"
#include "SpriteDib.h"

SpriteDib::SpriteDib(int maxSprite, DWORD colorKey)
{
	// 최대로 읽어올 개수 만큼 미리 할당받는다.
	sprite = new Sprite[maxSprite];
	
	memset(sprite, NULL, sizeof(Sprite) * maxSprite);

	this->maxSprite = maxSprite;
	this->colorKeyColor = colorKey;
}

SpriteDib::~SpriteDib()
{
	// 전체를 돌면서 모두 지워준다.
	for (int i = 0; i < maxSprite; i++)
	{
		ReleaseSprite(i);
	}

	delete[] sprite;
}

// BMP 파일을 읽어서 하나의 프레임으로 저장한다.
// Parameters : (int)spriteIndex, (char*)fileName, (int)centerPointX, (int)centerPointY
// Return : (BOOL)true, false
BOOL SpriteDib::LoadDibSprite(int spriteIndex, WCHAR* fileName, int centerPointX, int centerPointY)
{
	HANDLE file;
	DWORD read;
	int pitch, imageSize;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	// 비트맵 헤더를 열어 BMP 파일 확인
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

	// 한 줄, 한 줄의 피치값을 구한다.
	pitch = infoHeader.biWidth * infoHeader.biBitCount / 8;

	// 스프라이트 구조체에 크기 저장.
	sprite[spriteIndex].width = infoHeader.biWidth;
	sprite[spriteIndex].height = infoHeader.biHeight;
	sprite[spriteIndex].pitch = pitch;
	sprite[spriteIndex].bitCount = infoHeader.biBitCount;
	sprite[spriteIndex].centerPointX = centerPointX;
	sprite[spriteIndex].centerPointY = centerPointY;

	// 이미지에 대한 전체 크기를 구하고, 메모리 할당.
	imageSize = pitch * infoHeader.biHeight;
	sprite[spriteIndex].byteImage = new BYTE[imageSize];
	memset(sprite[spriteIndex].byteImage, 0, imageSize);

	// 이미지 부분은 스프라이트 버퍼로 읽어온다.
	// DIB는 뒤집어져 있으므로 이를 다시 뒤집자.
	// 임시 버퍼에 읽은 뒤에 뒤집으면서 복사한다.
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

// 해당 스프라이트 해제.
// Parameters : (int)spriteIndex
// Return : (BOOL)true, false
void SpriteDib::ReleaseSprite(int spriteIndex)
{
	// 최대 할당된 스프라이트를 넘어서면 안 된다.
	if (spriteIndex >= maxSprite)
	{
		return;
	}

	// == 비교문에서 상수를 왼쪽에 뒀을 때, = 기호를 하나 안 쓴 것을 컴파일러가 잡아준다.
	if (NULL != sprite[spriteIndex].byteImage)
	{
		// 삭제 후 초기화
		delete[] sprite[spriteIndex].byteImage; 
		memset(&sprite[spriteIndex], 0, sizeof(Sprite));
	}
}

// 특정 메모리 위치에 스프라이트를 출력한다. (컬러키, 클리핑 처리)
void SpriteDib::DrawSprite(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{	
	// 최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}
	
	// 지역변수로 필요 정보 세팅
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	
	int bitNum = sprite[spriteIndex].bitCount / 8;

	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = srcWidth * bitNum;
	
	// 출력 중점으로 좌표 계산
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// 상단에 대한 스프라이트 출력 위치 계산. (상단 클리핑)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// 하단 출력 위치 계산. (하단 클리핑)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// 왼쪽 출력 위치 계산. (좌측 클리핑)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// 오른쪽 출력 위치 계산. (우측 클리핑)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// 화면에 찍을 위치로 이동한다.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;
	
	// 전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (int i = 0; i < srcHeight; i++)
	{
		dest = (DWORD*)destOrigin;
		src = (DWORD*)srcOrigin;

		for (int j = 0; j < srcWidth; j++)
		{
			// 컬러키가 아닐 때 화면에 찍고 다음 칸으로 이동.
			if ((*src & 0x00ffffff) != 0x00ffffff)
			{
				*dest = *src;
			}
			
			dest++;
			src++;
		}

		// 화면과 스프라이트 모두 다음 줄로 이동.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}

// 특정 메모리 위치에 스프라이트를 반투명으로 출력한다. (컬러키, 클리핑 처리)
void SpriteDib::DrawSpriteTranslucence(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{
	// 최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}

	// 지역변수로 필요 정보 세팅
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = sprite[spriteIndex].pitch * drawLen / SCREENDIB_DRAWLEN_MAX;

	int bitNum = sprite[spriteIndex].bitCount / 8;

	// 출력 중점으로 좌표 계산
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// 상단에 대한 스프라이트 출력 위치 계산. (상단 클리핑)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// 하단 출력 위치 계산. (하단 클리핑)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// 왼쪽 출력 위치 계산. (좌측 클리핑)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// 오른쪽 출력 위치 계산. (우측 클리핑)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// 화면에 찍을 위치로 이동한다.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;

	// 전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (int i = 0; i < srcHeight; i++)
	{
		dest = (DWORD*)destOrigin;
		src = (DWORD*)srcOrigin;

		for (int j = 0; j < srcWidth; j++)
		{
			// 컬러키가 아닐 때 화면에 찍고 다음 칸으로 이동.
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

		// 화면과 스프라이트 모두 다음 줄로 이동.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}

// 특정 메모리 위치에 스프라이트를 붉게 출력한다. (컬러키, 클리핑 처리) 
void SpriteDib::DrawSpriteRed(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{
	// 최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}

	// 지역변수로 필요 정보 세팅
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = sprite[spriteIndex].pitch * drawLen / SCREENDIB_DRAWLEN_MAX;

	int bitNum = sprite[spriteIndex].bitCount / 8;

	// 출력 중점으로 좌표 계산
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// 상단에 대한 스프라이트 출력 위치 계산. (상단 클리핑)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// 하단 출력 위치 계산. (하단 클리핑)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// 왼쪽 출력 위치 계산. (좌측 클리핑)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// 오른쪽 출력 위치 계산. (우측 클리핑)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// 화면에 찍을 위치로 이동한다.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;

	// 전체 크기를 돌면서 픽셀마다 투명색 처리를 하며 그림 출력.
	for (int i = 0; i < srcHeight; i++)
	{
		dest = (DWORD*)destOrigin;
		src = (DWORD*)srcOrigin;

		for (int j = 0; j < srcWidth; j++)
		{
			// 컬러키가 아닐 때 화면에 찍고 다음 칸으로 이동.
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

		// 화면과 스프라이트 모두 다음 줄로 이동.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}

// 특정 메모리 위치에 이미지를 출력한다. (클리핑 처리)
void SpriteDib::DrawImage(int spriteIndex, int drawX, int drawY, BYTE* byteDest,
	int destWidth, int destHeight, int destPitch, int drawLen)
{
	// 최대 스프라이트 개수를 초과하거나, 로드되지 않는 스프라이트라면 무시
	if (spriteIndex >= maxSprite || sprite[spriteIndex].pitch == NULL)
	{
		return;
	}

	// 지역변수로 필요 정보 세팅
	DWORD* src = (DWORD*)(sprite[spriteIndex].byteImage);
	DWORD* dest;
	BYTE* srcOrigin;
	BYTE* destOrigin;
	int srcWidth = sprite[spriteIndex].width * drawLen / SCREENDIB_DRAWLEN_MAX;
	int srcHeight = sprite[spriteIndex].height;
	int srcPitch = sprite[spriteIndex].pitch * drawLen / SCREENDIB_DRAWLEN_MAX;


	int bitNum = sprite[spriteIndex].bitCount / 8;

	// 출력 중점으로 좌표 계산
	drawX -= sprite[spriteIndex].centerPointX;
	drawY -= sprite[spriteIndex].centerPointY;

	// 상단에 대한 스프라이트 출력 위치 계산. (상단 클리핑)
	if (drawY < 0)
	{
		src += srcWidth * (-drawY);
		srcHeight += drawY;
		drawY = 0;
	}

	// 하단 출력 위치 계산. (하단 클리핑)
	if (drawY + srcHeight > destHeight)
	{
		srcHeight = destHeight - drawY;
	}

	// 왼쪽 출력 위치 계산. (좌측 클리핑)
	if (drawX < 0)
	{
		src -= drawX;
		srcWidth += drawX;
		drawX = 0;
	}

	// 오른쪽 출력 위치 계산. (우측 클리핑)
	if (drawX + srcWidth > destWidth)
	{
		srcWidth = destWidth - drawX;
	}

	// 화면에 찍을 위치로 이동한다.
	dest = (DWORD*)(byteDest + drawX * bitNum + (drawY * destWidth * bitNum));
	srcOrigin = (BYTE*)src;
	destOrigin = (BYTE*)dest;

	// 전체 크기를 돌면서 픽셀마다 그림 출력.
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

		// 화면과 스프라이트 모두 다음 줄로 이동.
		destOrigin += destPitch;
		srcOrigin += sprite[spriteIndex].pitch;
	}
}