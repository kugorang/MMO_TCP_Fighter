#pragma once

#include <windows.h>

#define SERIALIZATIONBUFFER_DEFAULT_SIZE 65536

class SerializationBuffer
{
protected:
	// ���� ������
	BYTE* buffer;

	// ���� ������, ������ ���� ��ġ, �б� ��ġ
	int bufferSize, front, rear;
public:
	// ������, �ı���.
	SerializationBuffer();
	SerializationBuffer(int bufferSize);
	virtual ~SerializationBuffer();

	// ���� ��ü�� �뷮 ����.
	int GetBufferSize();

	// ���� ������� �뷮 ���.
	int GetUseSize();

	// ���� ���ۿ� ���� �뷮 ���.
	int GetRemainSize();

	// �� ĭ �̵� �� Rear ��ġ�� ������ ����.
	int Enqueue(BYTE* data, int size);

	// �� ĭ �̵� �� Front ��ġ���� ������ ��. Front �̵�.
	int Dequeue(BYTE* data, int size);

	// �� ĭ �̵� �� Front ��ġ���� ������ ���� ��. Front �� ����.
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

	template <typename T>
	SerializationBuffer& operator << (T const& data)
	{
		Enqueue((BYTE*)&data, sizeof(data));

		return *this;
	}

	template <typename T>
	SerializationBuffer& operator >> (T const& data)
	{
		Dequeue((BYTE*)&data, sizeof(data));

		return *this;
	}
};