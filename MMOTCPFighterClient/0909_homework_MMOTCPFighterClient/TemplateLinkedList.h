#ifndef __LINKEDLIST__
#define __LINKEDLIST__

template <typename T>
class TemplateLinkedList
{
private:
	class Node					// 리스트 노드 구조체
	{
	public:
		Node *prev;				// 이전 노드를 가리키는 포인터
		Node *next;				// 다음 노드를 가리키는 포인터
		T* data;				// 데이터 포인터
	};

	Node* head;					// 리스트 처음
	Node* tail;					// 리스트 마지막
	Node* freeNode;				// 노드 기억시 사용하는 변수
	unsigned int listCount;		// 링크된 리스트의 개수

	// pFreeNode에 현재 리스트가 기억되므로 계속해서 연결해 사용 가능하다.
protected:
	void Inital();		// 변수 초기화
	void AfreshList();	// 새롭게 Head, Tail을 생성. 있을 경우 삭제.
public:
	TemplateLinkedList();
	~TemplateLinkedList();

	void InsertHead(T* addData);	// 머리에 추가
	void InsertTail(T* addData);	// 꼬리에 추가
	void InsertNext(T* addData);	// FreeNode 다음에 추가
	void InsertPrev(T* addData);	// FreeNode 다음에 추가

	void MoveHead();	// FreeNode를 head로 변경
	void MoveTail();	// FreeNode를 tail로 변경
	bool MoveNext();	// FreeNode를 다음 리스트로 변경
	bool MovePrev();	// FreeNode를 이전 리스트로 변경
	bool MoveNum(int num);	// FreeNode를 num 번호 리스트로 변경
	void MoveFreeNode(T* data);	// FreeNode를 data로 변경

	T* GetHead();		// Head의 다음 리스트 데이터를 얻고 FreeNode를 그 리스트로 변경
	T* GetTail();		// Tail의 이전 리스트 데이터를 얻고 FreeNode를 그 리스트로 변경
	T* GetAt();			// FreeNode를 얻음.
	T* GetNext();		// FreeNode 다음 리스트를 얻음.
	T* GetPrev();		// FreeNode 이전 리스트를 얻음.
	T* GetNum(int num);	// num 번호의 리스트를 얻음.

	int GetCount();	// 리스트 안 모든 Node 개수를 얻음.

	void SetAt(T* data);	// FreeNode 자리에 데이터를 넣음. (SWAP 용도로 사용.)
	void SetNum(int num, T* data);	// num 번호의 리스트에 데이터를 넣음. (SWAP 용도로 사용.)

	T* PeekHead();		// Head의 다음 리스트 데이터를 얻지만 FreeNode 변경 없음.
	T* PeekTail();		// Tail의 이전 리스트 데이터를 얻지만 FreeNode 변경 없음.

	/* 여기서 지워지는 것은 리스트 Node 데이터 뿐이므로
	실제 포인터의 자료는 직접 지워야 한다.*/
	T* RemoveAt();		// FreeNode를 지움.
	T* RemoveNext();	// FreeNode 다음을 지움.
	T* RemovePrev();	// FreeNode 이전을 지움.
	bool RemoveAll();	// 모두 지움.
};

template <typename T>
TemplateLinkedList<T>::TemplateLinkedList()
{
	Inital();
	AfreshList();
}

template <typename T>
TemplateLinkedList<T>::~TemplateLinkedList()
{
	RemoveAll();

	if (head != nullptr)
	{
		delete head;
	}

	if (tail != nullptr)
	{
		delete tail;
	}
}

template <typename T>
void TemplateLinkedList<T>::Inital()		// 변수 초기화
{
	head = nullptr;
	tail = nullptr;
	freeNode = nullptr;
	listCount = 0;
}

template <typename T>
void TemplateLinkedList<T>::AfreshList()	// 새롭게 Head, Tail을 생성. 있을 경우 삭제.
{
	if (listCount != 0)
	{
		RemoveAll();
	}

	head = new Node();
	tail = new Node();

	head->prev = head;
	head->next = tail;
	tail->prev = head;
	tail->next = tail;
}

template <typename T>
void TemplateLinkedList<T>::InsertHead(T* addData)	// 머리에 추가
{
	Node* newNode = new Node();

	newNode->data = addData;
	newNode->prev = head;
	newNode->next = head->next;
	head->next->prev = newNode;
	head->next = newNode;

	listCount++;
}

template <typename T>
void TemplateLinkedList<T>::InsertTail(T* addData)	// 꼬리에 추가
{
	Node* newNode = new Node();

	newNode->data = addData;
	newNode->next = tail;
	newNode->prev = tail->prev;
	tail->prev->next = newNode;
	tail->prev = newNode;

	listCount++;
}

template <typename T>
void TemplateLinkedList<T>::InsertNext(T* addData)	// FreeNode 다음에 추가
{
	if (freeNode != nullptr || freeNode != tail)
	{
		Node* newNode = new Node();

		newNode->data = addData;
		newNode->next = freeNode->next;
		newNode->prev = freeNode;
		freeNode->next->prev = newNode;
		freeNode->next = newNode;

		listCount++;
	}
	else
	{
		OutputDebugString(L"현재 freeNode는 알 수 없거나 꼬리입니다. (InsertNext)\n");
	}
}

template <typename T>
void TemplateLinkedList<T>::InsertPrev(T* addData)	// FreeNode 이전에 추가
{
	if (freeNode != nullptr || freeNode != head)
	{
		Node* newNode = new Node();

		newNode->data = addData;
		newNode->next = freeNode;
		newNode->prev = freeNode->prev;
		freeNode->prev->next = newNode;
		freeNode->prev = newNode;

		listCount++;
	}
	else
	{
		OutputDebugString(L"현재 freeNode는 알 수 없거나 머리입니다. (InsertPrev)\n");
	}
}

template <typename T>
void TemplateLinkedList<T>::MoveHead()	// FreeNode를 head로 변경
{
	freeNode = head;
}

template <typename T>
void TemplateLinkedList<T>::MoveTail()	// FreeNode를 tail로 변경
{
	freeNode = tail;
}

template <typename T>
bool TemplateLinkedList<T>::MoveNext()	// FreeNode를 다음 리스트로 변경
{
	if (freeNode == tail || freeNode->next == tail)
	{
		//OutputDebugString(L"현재 freeNode의 다음은 꼬리입니다. (MoveNext)\n");
		return false;

	}
	else
	{
		freeNode = freeNode->next;
		return true;
	}
}

template <typename T>
bool TemplateLinkedList<T>::MovePrev()	// FreeNode를 이전 리스트로 변경
{
	if (freeNode == head || freeNode->prev == head)
	{
		OutputDebugString(L"현재 freeNode의 이전은 머리입니다. (MovePrev)\n");
		return false;
	}
	else
	{
		freeNode = freeNode->prev;
		return true;
	}
}

template <typename T>
bool TemplateLinkedList<T>::MoveNum(int num)	// FreeNode를 num 번호 리스트로 변경
{
	MoveHead();

	for (int i = 0; i < num; i++)
	{
		if (!MoveNext())
		{
			OutputDebugString(L"리스트 접근에 오류가 발생했습니다. (MoveNum)\n");
			return false;
		}
	}

	return true;
}

template <typename T>
void TemplateLinkedList<T>::MoveFreeNode(T* data)
{
	MoveHead();

	while (MoveNext())
	{
		if (freeNode->data == data)
		{
			break;
		}
	}
}

template <typename T>
T* TemplateLinkedList<T>::GetHead()		// Head의 다음 리스트 데이터를 얻고 FreeNode를 그 리스트로 변경
{
	T* data = nullptr;

	MoveHead();

	data = GetNext();

	if (data == nullptr)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetHead)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetTail()		// Tail의 이전 리스트 데이터를 얻고 FreeNode를 그 리스트로 변경
{
	T* data = nullptr;

	MoveTail();

	data = GetPrev();

	if (data == nullptr)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetTail)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetAt()	// FreeNode를 얻음.
{
	if (freeNode == nullptr || freeNode == head || freeNode == tail)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetAt)\n");
		return nullptr;
	}

	return freeNode->data;
}

template <typename T>
T* TemplateLinkedList<T>::GetNext()		// FreeNode 다음 리스트를 얻음.
{
	T* data = nullptr;

	if (!MoveNext())
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetNext)\n");
		return nullptr;
	}

	data = GetAt();

	if (data == nullptr)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetNext)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetPrev()		// FreeNode 이전 리스트를 얻음.
{
	T* data = nullptr;

	if (!MovePrev())
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetPrev)\n");
		return nullptr;
	}

	data = GetAt();

	if (data == nullptr)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetPrev)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetNum(int num)	// num 번호의 리스트를 얻음.
{
	T* data = nullptr;

	if (!MoveNum(num))
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetNum)\n");
		return nullptr;
	}

	data = GetAt();

	if (data == nullptr)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetNum)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
int TemplateLinkedList<T>::GetCount()	// 리스트 안의 Node 개수를 얻음.
{
	return listCount;
}

template <typename T>
void TemplateLinkedList<T>::SetAt(T* data)
{
	if (freeNode == nullptr || freeNode == head || freeNode == tail)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (GetAt)\n");
		return nullptr;
	}

	freeNode->data = data;
}

template <typename T>
void TemplateLinkedList<T>::SetNum(int num, T* data)
{
	if (!MoveNum(num))
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (SetNum)\n");
	}

	freeNode->data = data;

	if (data == nullptr)
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (SetNum)\n");
	}
}

template <typename T>
T* TemplateLinkedList<T>::PeekHead()	// Head의 다음 리스트 데이터를 얻지만 FreeNode 변경 없음.
{
	return head->next->data;
}

template <typename T>
T* TemplateLinkedList<T>::PeekTail()	// Tail의 이전 리스트 데이터를 얻지만 FreeNode 변경 없음.
{
	return tail->prev->data;
}

template <typename T>
T* TemplateLinkedList<T>::RemoveAt()	// FreeNode를 지우고 다음 노드를 가리킴.
{
	if (freeNode == nullptr || freeNode == head || freeNode == tail)
	{
		OutputDebugString(L"현재 freeNode를 알 수 없거나, 머리 혹은 꼬리입니다. (RemoveAt)\n");
		return nullptr;
	}
	else
	{
		Node* tmpNode = freeNode->next;
		T* returnNode = nullptr;

		freeNode->prev->next = freeNode->next;
		freeNode->next->prev = freeNode->prev;

		returnNode = freeNode->data;

		delete freeNode;

		freeNode = tmpNode;

		listCount--;
		return returnNode;
		// 데이터는 꼭 밖에서 delete 해 줄 것!!!!
		// 생성을 보통 밖에서 하므로 파괴도 밖에서 해 준다.
	}
}

template <typename T>
T* TemplateLinkedList<T>::RemoveNext()	// FreeNode 다음을 지움.
{
	T* returnNode = nullptr;

	if (!MoveNext())
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (RemoveNext)\n");
		return nullptr;
	}

	returnNode = RemoveAt();

	return returnNode;
}

template <typename T>
T* TemplateLinkedList<T>::RemovePrev()	// FreeNode 이전을 지움.
{
	T* returnNode = nullptr;

	if (!MovePrev())
	{
		OutputDebugString(L"데이터가 존재하지 않습니다. (RemovePrev)\n");
		return nullptr;
	}

	returnNode = RemoveAt();

	return returnNode;
}

template <typename T>
bool TemplateLinkedList<T>::RemoveAll()	// 모두 지움.
{
	int tmplistCount = listCount;

	if (tmplistCount == 0)
	{
		OutputDebugString(L"리스트가 존재하지 않습니다. (RemoveAll)\n");
		return false;
	}

	MoveHead();
	MoveNext();

	while (tmplistCount--)
	{
		RemoveAt();
	}

	MoveHead();

	return true;
}
#endif // !__LINKEDLIST__