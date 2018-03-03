#include "stdafx.h"
#include "SerializationBuffer.h"

// 생성자.
SerializationBuffer::SerializationBuffer()
{
	bufferSize = SERIALIZATIONBUFFER_DEFAULT_SIZE;
	buffer = new BYTE[bufferSize];
	front = 0;
	rear = 0;

	//ZeroMemory(buffer, sizeof(BYTE) * bufferSize);
}

// 생성자.
SerializationBuffer::SerializationBuffer(int bufferSize)
{
	this->bufferSize = bufferSize;
	buffer = new BYTE[bufferSize];
	front = 0;
	rear = 0;

	//ZeroMemory(buffer, sizeof(BYTE) * (bufferSize));
}

// 파괴자.
SerializationBuffer::~SerializationBuffer()
{
	delete[] buffer;
	buffer = nullptr;
}

// 버퍼 전체의 용량 얻음.
int SerializationBuffer::GetBufferSize()
{
	return sizeof(BYTE) * (bufferSize);
}

// 현재 사용중인 용량 얻기.
int SerializationBuffer::GetUseSize()
{
	if (rear >= front)
	{
		return rear - front;
	}
	else // (front > rear)
	{
		return bufferSize - front + rear;
	}
}

// 현재 버퍼에 남은 용량 얻기.
int SerializationBuffer::GetRemainSize()
{
	if (rear >= front)
	{
		return bufferSize - rear + front - 1;
	}
	else // (front > rear)
	{
		return front - rear - 1;
	}
}

// Rear 위치에 데이터 넣음.
int SerializationBuffer::Enqueue(BYTE* data, int size)
{
	int remainSize = GetRemainSize();

	if (size > remainSize)
	{
		size = remainSize;
	}
		
	memcpy_s(buffer + rear, size, data, size);
	rear += size;

	return size;
}

// Front 위치에서 데이터 뺌. Front 이동.
int SerializationBuffer::Dequeue(BYTE* data, int size)
{
	int useSize = this->GetUseSize();
	
	if (size > useSize)
	{
		size = useSize;
	}

	memcpy_s(data, size, buffer + front, size);
	//ZeroMemory(buffer + front, size);

	front += size;

	return size;
}

// Front 위치에서 데이터 가져 옴. Front 안 변함.
int SerializationBuffer::Peek(BYTE* dest, int size)
{
	int useSize = GetUseSize();

	if (size > useSize)
	{
		size = useSize;
	}
		
	memcpy_s(dest, size, buffer + front, size);
	return size;
}

// 외부에서 Rear의 위치를 이동.
void SerializationBuffer::MoveRearPos(int size)
{
	rear = (rear + size) % bufferSize;
}

// 외부에서 Front의 위치를 이동.
void SerializationBuffer::MoveFrontPos(int size)
{
	front = (front + size) % bufferSize;
}

// 버퍼의 모든 데이터 삭제.
void SerializationBuffer::DeleteAllData()
{
	front = 0;
	rear = 0;
}

// 버퍼의 포인터 얻음.
BYTE* SerializationBuffer::GetBufferPtr()
{
	return buffer;
}

// 버퍼의 Rear 위치 버퍼 포인터 얻음.
BYTE* SerializationBuffer::GetRearPosBufferPtr()
{
	return buffer + rear;
}

// 버퍼의 Front 위치 버퍼 포인터 얻음.
BYTE* SerializationBuffer::GetFrontPosBufferPtr()
{
	return buffer + front;
}