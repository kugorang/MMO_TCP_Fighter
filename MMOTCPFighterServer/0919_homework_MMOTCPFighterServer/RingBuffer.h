#pragma once

#include <windows.h>

#define RINGBUFFER_DEFAULT_SIZE 32768

class RingBuffer
{
protected:
	// ���� ������
	BYTE* buffer;

	// ���� ������, ������ ���� ��ġ, �б� ��ġ
	int bufferSize, front, rear;
public:
	// ������, �ı���.
	RingBuffer();
	RingBuffer(int bufferSize);
	virtual ~RingBuffer();
	
	// ���� ��ü�� �뷮 ����.
	int GetBufferSize();

	// ���� ������� �뷮 ���.
	int GetUseSize();

	// ���� ���ۿ� ���� �뷮 ���.
	int GetRemainSize();

	// ���� �����ͷ� �ܺο��� �� ���� �а�, �� �� �ִ� ���� ���.
	int GetNotBrokenGetSize();
	int GetNotBrokenPutSize();

	// Rear ��ġ�� ������ ����.
	int Enqueue(BYTE* data, int size);

	// Front ��ġ���� ������ ��. Front �̵�.
	int Dequeue(BYTE* data, int size);
	
	// Front ��ġ���� ������ ���� ��. Front �� ����.
	int Peek(BYTE* dest, int size);

	// �ܺο��� Rear�� ��ġ�� �̵�.
	void MoveRearPos(int size);

	// �ܺο��� Front�� ��ġ�� �̵�.
	void MoveFrontPos(int size);

	// ������ ��� ������ ����.
	void DeleteAllData();

	// ������ ������ ����.
	BYTE* GetBufferPtr();

	// ������ Rear ��ġ ���� ������ ����.
	BYTE* GetRearPosBufferPtr();
	
	// ������ Front ��ġ ���� ������ ����.
	BYTE* GetFrontPosBufferPtr();
};