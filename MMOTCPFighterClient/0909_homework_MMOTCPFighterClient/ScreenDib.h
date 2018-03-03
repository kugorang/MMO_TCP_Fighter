#pragma once

// DIB를 사용한 GDI용 스크린 버퍼.
// 윈도우의 HDC에 DIB를 사용하여 그림을 찍어 준다.
class ScreenDib
{
protected:
	BITMAPINFO dibInfo;
	BYTE* byteBuffer;
	int width, height, pitch, colorBit, bufferSize;	// pitch : 4바이트 정렬된 한 줄의 바이트 수

	void CreateDibBuffer(int width, int height, int colorBit);
	void ReleaseDibBuffer();
public:
	// 생성자, 파괴자
	ScreenDib(int width, int height, int colorBit);
	virtual ~ScreenDib();

	// 메모리 버퍼 내용을 윈도우 DC에 출력.
	void DrawBuffer(HWND hWnd, int x = 0, int y = 0);

	BYTE* GetDibBuffer();	// 버퍼 메모리 포인터 얻기
	int GetWidth();			// 버퍼 픽셀 너비
	int GetHeight();		// 버퍼 픽셀 높이
	int GetPitch();			// 버퍼 한 줄의 메모리 길이
	int GetColorBit();
};