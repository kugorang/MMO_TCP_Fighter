#include "stdafx.h"
#include "SerializationBuffer.h"

// ������.
SerializationBuffer::SerializationBuffer()
{
	bufferSize = SERIALIZATIONBUFFER_DEFAULT_SIZE;
	buffer = new BYTE[bufferSize];
	front = 0;
	rear = 0;

	//ZeroMemory(buffer, sizeof(BYTE) * bufferSize);
}

// ������.
SerializationBuffer::SerializationBuffer(int bufferSize)
{
	this->bufferSize = bufferSize;
	buffer = new BYTE[bufferSize];
	front = 0;
	rear = 0;

	//ZeroMemory(buffer, sizeof(BYTE) * (bufferSize));
}

// �ı���.
SerializationBuffer::~SerializationBuffer()
{
	delete[] buffer;
	buffer = nullptr;
}

// ���� ��ü�� �뷮 ����.
int SerializationBuffer::GetBufferSize()
{
	return sizeof(BYTE) * (bufferSize);
}

// ���� ������� �뷮 ���.
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

// ���� ���ۿ� ���� �뷮 ���.
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

// Rear ��ġ�� ������ ����.
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

// Front ��ġ���� ������ ��. Front �̵�.
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

// Front ��ġ���� ������ ���� ��. Front �� ����.
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

// �ܺο��� Rear�� ��ġ�� �̵�.
void SerializationBuffer::MoveRearPos(int size)
{
	rear = (rear + size) % bufferSize;
}

// �ܺο��� Front�� ��ġ�� �̵�.
void SerializationBuffer::MoveFrontPos(int size)
{
	front = (front + size) % bufferSize;
}

// ������ ��� ������ ����.
void SerializationBuffer::DeleteAllData()
{
	front = 0;
	rear = 0;
}

// ������ ������ ����.
BYTE* SerializationBuffer::GetBufferPtr()
{
	return buffer;
}

// ������ Rear ��ġ ���� ������ ����.
BYTE* SerializationBuffer::GetRearPosBufferPtr()
{
	return buffer + rear;
}

// ������ Front ��ġ ���� ������ ����.
BYTE* SerializationBuffer::GetFrontPosBufferPtr()
{
	return buffer + front;
}