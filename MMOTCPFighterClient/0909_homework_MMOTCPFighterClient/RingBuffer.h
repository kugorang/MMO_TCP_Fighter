#pragma once

#include <windows.h>

#define RINGBUFFER_DEFAULT_SIZE 32768

class RingBuffer
{
protected:
	// 버퍼 포인터
	BYTE* buffer;

	// 버퍼 사이즈, 버퍼의 쓰기 위치, 읽기 위치
	int bufferSize, front, rear;
public:
	// 생성자, 파괴자.
	RingBuffer();
	RingBuffer(int bufferSize);
	virtual ~RingBuffer();
	
	// 버퍼 전체의 용량 얻음.
	int GetBufferSize();

	// 현재 사용중인 용량 얻기.
	int GetUseSize();

	// 현재 버퍼에 남은 용량 얻기.
	int GetRemainSize();

	// 버퍼 포인터로 외부에서 한 번에 읽고, 쓸 수 있는 길이 얻기.
	int GetNotBrokenGetSize();
	int GetNotBrokenPutSize();

	// Rear 위치에 데이터 넣음.
	int Enqueue(BYTE* data, int size);

	// Front 위치에서 데이터 뺌. Front 이동.
	int Dequeue(BYTE* data, int size);
	
	// Front 위치에서 데이터 가져 옴. Front 안 변함.
	int Peek(BYTE* dest, int size);

	// 외부에서 Rear의 위치를 이동.
	void MoveRearPos(int size);

	// 외부에서 Front의 위치를 이동.
	void MoveFrontPos(int size);

	// 버퍼의 모든 데이터 삭제.
	void DeleteAllData();

	// 버퍼의 포인터 얻음.
	BYTE* GetBufferPtr();

	// 버퍼의 Rear 위치 버퍼 포인터 얻음.
	BYTE* GetRearPosBufferPtr();
	
	// 버퍼의 Front 위치 버퍼 포인터 얻음.
	BYTE* GetFrontPosBufferPtr();
};