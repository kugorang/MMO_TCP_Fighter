#pragma once

// DIB�� ����� GDI�� ��ũ�� ����.
// �������� HDC�� DIB�� ����Ͽ� �׸��� ��� �ش�.
class ScreenDib
{
protected:
	BITMAPINFO dibInfo;
	BYTE* byteBuffer;
	int width, height, pitch, colorBit, bufferSize;	// pitch : 4����Ʈ ���ĵ� �� ���� ����Ʈ ��

	void CreateDibBuffer(int width, int height, int colorBit);
	void ReleaseDibBuffer();
public:
	// ������, �ı���
	ScreenDib(int width, int height, int colorBit);
	virtual ~ScreenDib();

	// �޸� ���� ������ ������ DC�� ���.
	void DrawBuffer(HWND hWnd, int x = 0, int y = 0);

	BYTE* GetDibBuffer();	// ���� �޸� ������ ���
	int GetWidth();			// ���� �ȼ� �ʺ�
	int GetHeight();		// ���� �ȼ� ����
	int GetPitch();			// ���� �� ���� �޸� ����
	int GetColorBit();
};