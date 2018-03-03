#include "stdafx.h"
#include "ScreenDib.h"


// ��ü ������� �÷� ��Ʈ�� �޴´�.
ScreenDib::ScreenDib(int width, int height, int colorBit)
{
	// ���� ������� �ʱ�ȭ.
	this->width = width;
	this->height = height;
	this->colorBit = colorBit;
	pitch = width * colorBit / 8;

	// ��ũ�� ���� ���� �Լ� ȣ��.
	CreateDibBuffer(width, height, colorBit);
}

ScreenDib::~ScreenDib()
{
	ReleaseDibBuffer();
}

// �޸� ���ۿ� DIB�� �����Ѵ�. �����ڿ��� ȣ��ȴ�.
// Parameters : (int)width, (int)height, (int)colorBit
void ScreenDib::CreateDibBuffer(int width, int height, int colorBit)
{
	// �Է¹��� ������ ������ ��� ���� ���� ����.
	// BITMAPINFO(BITMAPINFOHEADER)�� �����Ѵ�.
	dibInfo.bmiHeader.biSize = sizeof(dibInfo);
	dibInfo.bmiHeader.biWidth = width;
	dibInfo.bmiHeader.biHeight = -height;
	dibInfo.bmiHeader.biPlanes = 1;
	dibInfo.bmiHeader.biBitCount = colorBit;
	// �̹��� ����� ����Ͽ� ���ۿ� �̹��� ���� �Ҵ�.
	bufferSize = pitch * height;
	byteBuffer = (BYTE*)malloc(bufferSize);
}

// ������ �޸� ���۸� �����Ѵ�. �ı��ڿ��� ȣ��ȴ�.
// Parameters : ����.
void ScreenDib::ReleaseDibBuffer()
{
	// �޸� ����
	delete byteBuffer;
}

// �޸� ���� ������ ������ DC�� ����Ѵ�. (Flip)
// Parameters : (HWND)������ �ڵ�, (int)int ��ġ
void ScreenDib::DrawBuffer(HWND hWnd, int x, int y)
{
	HDC hdc;
	
	hdc = GetDC(hWnd);

	// �Է¹��� hWnd�� �ڵ��� DC�� �� DC�� x, y ��ġ�� ��ũ�� ���� DIB�� ����Ѵ�.
	//StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, byteBuffer, &dibInfo, DIB_RGB_COLORS, SRCCOPY);
	SetDIBitsToDevice(hdc, x, y, width, height, 0, 0, 0, height, byteBuffer, &dibInfo, DIB_RGB_COLORS);
	
	ReleaseDC(hWnd, hdc);
}

// ���� �޸� ������ ���
BYTE* ScreenDib::GetDibBuffer()
{
	return byteBuffer;
}

// ���� �ȼ� �ʺ�
int ScreenDib::GetWidth()
{
	return width;
}

// ���� �ȼ� ����
int ScreenDib::GetHeight()
{
	return height;
}

// ���� �� ���� �޸� ����
int ScreenDib::GetPitch()
{
	return pitch;
}

int ScreenDib::GetColorBit()
{
	return colorBit;
}