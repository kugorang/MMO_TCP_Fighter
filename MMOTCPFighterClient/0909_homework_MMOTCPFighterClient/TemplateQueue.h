#pragma once

#include <windows.h>

#define TEMPLATEQUEUE_ARRAY_LENGTH 100

template <typename T>
class TemplateQueue
{
private:
	int front;
	int rear;
	T arrData[TEMPLATEQUEUE_ARRAY_LENGTH];
	int arrCount;

	int NextPosCheck(int pos);
	bool IsQueueEmpty();
public:
	TemplateQueue();
	~TemplateQueue();

	void Enqueue(T data);
	T Dequeue();
	T PeekAtFront();
	T PeekAtNumPos(int pos);
	T PeekAtRear();

	int GetQueueNum();
};

template <typename T>
TemplateQueue<T>::TemplateQueue()
{
	front = 0;
	rear = 0;
	arrCount = 0;

	memset(arrData, NULL, sizeof(T) * TEMPLATEQUEUE_ARRAY_LENGTH);
}

template <typename T>
TemplateQueue<T>::~TemplateQueue()
{
}

template <typename T>
void TemplateQueue<T>::Enqueue(T data)
{
	if (NextPosCheck(rear) == front)
	{
		OutputDebugString(L"큐가 가득 찼습니다. (Enqueue(T data))\n");
		exit(2001);
	}
	else
	{
		rear = NextPosCheck(rear);
		arrCount++;
		arrData[rear] = data;
	}
}

template <typename T>
T TemplateQueue<T>::Dequeue()
{
	if (IsQueueEmpty())
	{
		OutputDebugString(L"큐가 비어 있습니다. (Dequeue())\n");
		exit(2002);
	}
	else
	{
		front = NextPosCheck(front);
		arrCount--;
		return arrData[front];
	}
}

template <typename T>
int TemplateQueue<T>::NextPosCheck(int pos)
{
	if (pos + 1 >= TEMPLATEQUEUE_ARRAY_LENGTH)
	{
		return (pos + 1) % TEMPLATEQUEUE_ARRAY_LENGTH;
	}
	else
	{
		return pos + 1;
	}
}

template <typename T>
bool TemplateQueue<T>::IsQueueEmpty()
{
	if (front == rear)
	{
		return true;
	}
	else
	{
		return false;
	}

}

template <typename T>
T TemplateQueue<T>::PeekAtFront()
{
	if (IsQueueEmpty())
	{
		OutputDebugString(L"큐가 비어 있습니다. (PeekAtFront())\n");
		exit(2003);
	}
	else
	{
		return arrData[NextPosCheck(front)];
	}
}

template <typename T>
T TemplateQueue<T>::PeekAtNumPos(int pos)
{
	if (IsQueueEmpty())
	{
		OutputDebugString(L"큐가 비어 있습니다. (PeekAtNumPos(int pos))\n");
		exit(2004);
	}
	else
	{
		return arrData[NextPosCheck(front + pos)];
	}
}

template <typename T>
T TemplateQueue<T>::PeekAtRear()
{
	if (IsQueueEmpty())
	{
		OutputDebugString(L"큐가 비어 있습니다. (PeekAtRear())\n");
		exit(2005);
	}
	else
	{
		return arrData[rear];
	}
}

template <typename T>
int TemplateQueue<T>::GetQueueNum()
{
	return arrCount;
}