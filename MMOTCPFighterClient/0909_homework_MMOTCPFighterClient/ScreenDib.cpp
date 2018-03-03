#include "stdafx.h"
#include "ScreenDib.h"


// 전체 사이즈와 컬러 비트를 받는다.
ScreenDib::ScreenDib(int width, int height, int colorBit)
{
	// 각종 멤버변수 초기화.
	this->width = width;
	this->height = height;
	this->colorBit = colorBit;
	pitch = width * colorBit / 8;

	// 스크린 버퍼 생성 함수 호출.
	CreateDibBuffer(width, height, colorBit);
}

ScreenDib::~ScreenDib()
{
	ReleaseDibBuffer();
}

// 메모리 버퍼와 DIB를 생성한다. 생성자에서 호출된다.
// Parameters : (int)width, (int)height, (int)colorBit
void ScreenDib::CreateDibBuffer(int width, int height, int colorBit)
{
	// 입력받은 인자의 정보로 멤버 변수 정보 세팅.
	// BITMAPINFO(BITMAPINFOHEADER)를 세팅한다.
	dibInfo.bmiHeader.biSize = sizeof(dibInfo);
	dibInfo.bmiHeader.biWidth = width;
	dibInfo.bmiHeader.biHeight = -height;
	dibInfo.bmiHeader.biPlanes = 1;
	dibInfo.bmiHeader.biBitCount = colorBit;
	// 이미지 사이즈를 계산하여 버퍼용 이미지 동적 할당.
	bufferSize = pitch * height;
	byteBuffer = (BYTE*)malloc(bufferSize);
}

// 생성된 메모리 버퍼를 삭제한다. 파괴자에서 호출된다.
// Parameters : 없음.
void ScreenDib::ReleaseDibBuffer()
{
	// 메모리 해제
	delete byteBuffer;
}

// 메모리 버퍼 내용을 윈도우 DC에 출력한다. (Flip)
// Parameters : (HWND)윈도우 핸들, (int)int 위치
void ScreenDib::DrawBuffer(HWND hWnd, int x, int y)
{
	HDC hdc;
	
	hdc = GetDC(hWnd);

	// 입력받은 hWnd의 핸들의 DC를 얻어서 DC의 x, y 위치에 스크린 버퍼 DIB를 출력한다.
	//StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, byteBuffer, &dibInfo, DIB_RGB_COLORS, SRCCOPY);
	SetDIBitsToDevice(hdc, x, y, width, height, 0, 0, 0, height, byteBuffer, &dibInfo, DIB_RGB_COLORS);
	
	ReleaseDC(hWnd, hdc);
}

// 버퍼 메모리 포인터 얻기
BYTE* ScreenDib::GetDibBuffer()
{
	return byteBuffer;
}

// 버퍼 픽셀 너비
int ScreenDib::GetWidth()
{
	return width;
}

// 버퍼 픽셀 높이
int ScreenDib::GetHeight()
{
	return height;
}

// 버퍼 한 줄의 메모리 길이
int ScreenDib::GetPitch()
{
	return pitch;
}

int ScreenDib::GetColorBit()
{
	return colorBit;
}