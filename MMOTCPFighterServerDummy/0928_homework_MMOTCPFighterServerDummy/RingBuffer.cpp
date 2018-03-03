#include "RingBuffer.h"

// ������, �ı���.
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

// ���� ��ü�� �뷮 ����.
int RingBuffer::GetBufferSize()
{
	return sizeof(BYTE) * (bufferSize);
}

// ���� ������� �뷮 ���.
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

// ���� ���ۿ� ���� �뷮 ���.
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

// ���� �����ͷ� �ܺο��� �� ���� ���� �� �ִ� ���� ���.
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

// ���� �����ͷ� �ܺο��� �� ���� �� �� �ִ� ���� ���.
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

// Rear ��ġ�� ������ ����.
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

// Front ��ġ���� ������ ��. Front �̵�.
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

// Front ��ġ���� ������ ���� ��. Front �� ����.
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

// �ܺο��� Rear�� ��ġ�� �̵�.
void RingBuffer::MoveRearPos(int size)
{
	rear = (rear + size) % bufferSize;
}

// �ܺο��� Front�� ��ġ�� �̵�.
void RingBuffer::MoveFrontPos(int size)
{
	front = (front + size) % bufferSize;
}

// ������ ��� ������ ����.
void RingBuffer::DeleteAllData()
{
	front = 0;
	rear = 0;
}

// ������ ������ ����.
BYTE* RingBuffer::GetBufferPtr()
{
	return buffer;
}

// ������ Rear ��ġ ���� ������ ����.
BYTE* RingBuffer::GetRearPosBufferPtr()
{
	if (rear >= bufferSize)
	{
		MoveRearPos(0);
	}

	return buffer + rear;
}

// ������ Front ��ġ ���� ������ ����.
BYTE* RingBuffer::GetFrontPosBufferPtr()
{
	if (front >= bufferSize)	
	{
		MoveFrontPos(0);
	}

	return buffer + front;
}