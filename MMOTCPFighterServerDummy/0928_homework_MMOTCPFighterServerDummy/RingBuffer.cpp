#include "RingBuffer.h"

// 생성자, 파괴자.
RingBuffer::RingBuffer()
{
	bufferSize = RINGBUFFER_DEFAULT_SIZE;
	buffer = new BYTE[bufferSize];
	front = 0;
	rear = 0;

	ZeroMemory(buffer, sizeof(BYTE) * (bufferSize));
}

RingBuffer::RingBuffer(int bufferSize)
{
	this->bufferSize = bufferSize;
	buffer = new BYTE[bufferSize];
	front = 0;
	rear = 0;

	ZeroMemory(buffer, sizeof(BYTE) * (bufferSize));
}

RingBuffer::~RingBuffer()
{
	delete[] buffer;
	buffer = nullptr;
}

// 버퍼 전체의 용량 얻음.
int RingBuffer::GetBufferSize()
{
	return sizeof(BYTE) * (bufferSize);
}

// 현재 사용중인 용량 얻기.
int RingBuffer::GetUseSize()
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
int RingBuffer::GetRemainSize()
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

// 버퍼 포인터로 외부에서 한 번에 읽을 수 있는 길이 얻기.
int RingBuffer::GetNotBrokenGetSize()
{
	if (rear >= front)
	{
		return rear - front;
	}
	else // (front > rear)
	{
		return bufferSize - front;
	}
}

// 버퍼 포인터로 외부에서 한 번에 쓸 수 있는 길이 얻기.
int RingBuffer::GetNotBrokenPutSize()
{
	if (rear >= front)
	{
		return bufferSize - rear;
	}
	else // front > rear
	{
		return front - rear - 1;
	}
}

// Rear 위치에 데이터 넣음.
int RingBuffer::Enqueue(BYTE* data, int size)
{
	int remainSize = GetRemainSize();
	int notBrokenPutSize = GetNotBrokenPutSize();

	if (size > remainSize)
	{
		size = remainSize;
	}

	if (size > notBrokenPutSize)
	{
		memcpy_s(buffer + rear, notBrokenPutSize, data, notBrokenPutSize);
		memcpy_s(buffer, size - notBrokenPutSize, data + notBrokenPutSize, size - notBrokenPutSize);
	}
	else
	{
		memcpy_s(buffer + rear, size, data, size);
	}

	MoveRearPos(size);
	return size;
}

// Front 위치에서 데이터 뺌. Front 이동.
int RingBuffer::Dequeue(BYTE* data, int size)
{
	int useSize = GetUseSize();
	int notBrokenGetSize = GetNotBrokenGetSize();

	if (size > useSize)
	{
		size = useSize;
	}

	if (size > notBrokenGetSize)
	{
		memcpy_s(data, notBrokenGetSize, buffer + front, notBrokenGetSize);
		memcpy_s(data + notBrokenGetSize, size - notBrokenGetSize, buffer, size - notBrokenGetSize);
		/*ZeroMemory(buffer + front, notBrokenGetSize);
		ZeroMemory(buffer, size - notBrokenGetSize);*/
	}
	else
	{
		memcpy_s(data, size, buffer + front, size);
		//ZeroMemory(buffer + front, size);
	}

	MoveFrontPos(size);
	return size;
}

// Front 위치에서 데이터 가져 옴. Front 안 변함.
int RingBuffer::Peek(BYTE* dest, int size)
{
	int useSize = GetUseSize();
	int notBrokenGetSize = GetNotBrokenGetSize();

	if (size > useSize)
	{
		size = useSize;
	}

	if (size > notBrokenGetSize)
	{
		memcpy_s(dest, notBrokenGetSize, buffer + front, notBrokenGetSize);
		memcpy_s(dest + notBrokenGetSize, size - notBrokenGetSize, buffer, size - notBrokenGetSize);
	}
	else
	{
		memcpy_s(dest, size, buffer + front, size);
	}

	return size;
}

// 외부에서 Rear의 위치를 이동.
void RingBuffer::MoveRearPos(int size)
{
	rear = (rear + size) % bufferSize;
}

// 외부에서 Front의 위치를 이동.
void RingBuffer::MoveFrontPos(int size)
{
	front = (front + size) % bufferSize;
}

// 버퍼의 모든 데이터 삭제.
void RingBuffer::DeleteAllData()
{
	front = 0;
	rear = 0;
}

// 버퍼의 포인터 얻음.
BYTE* RingBuffer::GetBufferPtr()
{
	return buffer;
}

// 버퍼의 Rear 위치 버퍼 포인터 얻음.
BYTE* RingBuffer::GetRearPosBufferPtr()
{
	if (rear >= bufferSize)
	{
		MoveRearPos(0);
	}

	return buffer + rear;
}

// 버퍼의 Front 위치 버퍼 포인터 얻음.
BYTE* RingBuffer::GetFrontPosBufferPtr()
{
	if (front >= bufferSize)	
	{
		MoveFrontPos(0);
	}

	return buffer + front;
}