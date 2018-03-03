#ifndef __LINKEDLIST__
#define __LINKEDLIST__

template <typename T>
class TemplateLinkedList
{
private:
	class Node					// ����Ʈ ��� ����ü
	{
	public:
		Node *prev;				// ���� ��带 ����Ű�� ������
		Node *next;				// ���� ��带 ����Ű�� ������
		T* data;				// ������ ������
	};

	Node* head;					// ����Ʈ ó��
	Node* tail;					// ����Ʈ ������
	Node* freeNode;				// ��� ���� ����ϴ� ����
	unsigned int listCount;		// ��ũ�� ����Ʈ�� ����

	// pFreeNode�� ���� ����Ʈ�� ���ǹǷ� ����ؼ� ������ ��� �����ϴ�.
protected:
	void Inital();		// ���� �ʱ�ȭ
	void AfreshList();	// ���Ӱ� Head, Tail�� ����. ���� ��� ����.
public:
	TemplateLinkedList();
	~TemplateLinkedList();

	void InsertHead(T* addData);	// �Ӹ��� �߰�
	void InsertTail(T* addData);	// ������ �߰�
	void InsertNext(T* addData);	// FreeNode ������ �߰�
	void InsertPrev(T* addData);	// FreeNode ������ �߰�

	void MoveHead();	// FreeNode�� head�� ����
	void MoveTail();	// FreeNode�� tail�� ����
	bool MoveNext();	// FreeNode�� ���� ����Ʈ�� ����
	bool MovePrev();	// FreeNode�� ���� ����Ʈ�� ����
	bool MoveNum(int num);	// FreeNode�� num ��ȣ ����Ʈ�� ����
	void MoveFreeNode(T* data);	// FreeNode�� data�� ����

	T* GetHead();		// Head�� ���� ����Ʈ �����͸� ��� FreeNode�� �� ����Ʈ�� ����
	T* GetTail();		// Tail�� ���� ����Ʈ �����͸� ��� FreeNode�� �� ����Ʈ�� ����
	T* GetAt();			// FreeNode�� ����.
	T* GetNext();		// FreeNode ���� ����Ʈ�� ����.
	T* GetPrev();		// FreeNode ���� ����Ʈ�� ����.
	T* GetNum(int num);	// num ��ȣ�� ����Ʈ�� ����.

	int GetCount();	// ����Ʈ �� ��� Node ������ ����.

	void SetAt(T* data);	// FreeNode �ڸ��� �����͸� ����. (SWAP �뵵�� ���.)
	void SetNum(int num, T* data);	// num ��ȣ�� ����Ʈ�� �����͸� ����. (SWAP �뵵�� ���.)

	T* PeekHead();		// Head�� ���� ����Ʈ �����͸� ������ FreeNode ���� ����.
	T* PeekTail();		// Tail�� ���� ����Ʈ �����͸� ������ FreeNode ���� ����.

	/* ���⼭ �������� ���� ����Ʈ Node ������ ���̹Ƿ�
	���� �������� �ڷ�� ���� ������ �Ѵ�.*/
	T* RemoveAt();		// FreeNode�� ����.
	T* RemoveNext();	// FreeNode ������ ����.
	T* RemovePrev();	// FreeNode ������ ����.
	bool RemoveAll();	// ��� ����.
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
void TemplateLinkedList<T>::Inital()		// ���� �ʱ�ȭ
{
	head = nullptr;
	tail = nullptr;
	freeNode = nullptr;
	listCount = 0;
}

template <typename T>
void TemplateLinkedList<T>::AfreshList()	// ���Ӱ� Head, Tail�� ����. ���� ��� ����.
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
void TemplateLinkedList<T>::InsertHead(T* addData)	// �Ӹ��� �߰�
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
void TemplateLinkedList<T>::InsertTail(T* addData)	// ������ �߰�
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
void TemplateLinkedList<T>::InsertNext(T* addData)	// FreeNode ������ �߰�
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
		OutputDebugString(L"���� freeNode�� �� �� ���ų� �����Դϴ�. (InsertNext)\n");
	}
}

template <typename T>
void TemplateLinkedList<T>::InsertPrev(T* addData)	// FreeNode ������ �߰�
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
		OutputDebugString(L"���� freeNode�� �� �� ���ų� �Ӹ��Դϴ�. (InsertPrev)\n");
	}
}

template <typename T>
void TemplateLinkedList<T>::MoveHead()	// FreeNode�� head�� ����
{
	freeNode = head;
}

template <typename T>
void TemplateLinkedList<T>::MoveTail()	// FreeNode�� tail�� ����
{
	freeNode = tail;
}

template <typename T>
bool TemplateLinkedList<T>::MoveNext()	// FreeNode�� ���� ����Ʈ�� ����
{
	if (freeNode == tail || freeNode->next == tail)
	{
		//OutputDebugString(L"���� freeNode�� ������ �����Դϴ�. (MoveNext)\n");
		return false;

	}
	else
	{
		freeNode = freeNode->next;
		return true;
	}
}

template <typename T>
bool TemplateLinkedList<T>::MovePrev()	// FreeNode�� ���� ����Ʈ�� ����
{
	if (freeNode == head || freeNode->prev == head)
	{
		OutputDebugString(L"���� freeNode�� ������ �Ӹ��Դϴ�. (MovePrev)\n");
		return false;
	}
	else
	{
		freeNode = freeNode->prev;
		return true;
	}
}

template <typename T>
bool TemplateLinkedList<T>::MoveNum(int num)	// FreeNode�� num ��ȣ ����Ʈ�� ����
{
	MoveHead();

	for (int i = 0; i < num; i++)
	{
		if (!MoveNext())
		{
			OutputDebugString(L"����Ʈ ���ٿ� ������ �߻��߽��ϴ�. (MoveNum)\n");
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
T* TemplateLinkedList<T>::GetHead()		// Head�� ���� ����Ʈ �����͸� ��� FreeNode�� �� ����Ʈ�� ����
{
	T* data = nullptr;

	MoveHead();

	data = GetNext();

	if (data == nullptr)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetHead)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetTail()		// Tail�� ���� ����Ʈ �����͸� ��� FreeNode�� �� ����Ʈ�� ����
{
	T* data = nullptr;

	MoveTail();

	data = GetPrev();

	if (data == nullptr)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetTail)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetAt()	// FreeNode�� ����.
{
	if (freeNode == nullptr || freeNode == head || freeNode == tail)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetAt)\n");
		return nullptr;
	}

	return freeNode->data;
}

template <typename T>
T* TemplateLinkedList<T>::GetNext()		// FreeNode ���� ����Ʈ�� ����.
{
	T* data = nullptr;

	if (!MoveNext())
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetNext)\n");
		return nullptr;
	}

	data = GetAt();

	if (data == nullptr)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetNext)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetPrev()		// FreeNode ���� ����Ʈ�� ����.
{
	T* data = nullptr;

	if (!MovePrev())
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetPrev)\n");
		return nullptr;
	}

	data = GetAt();

	if (data == nullptr)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetPrev)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
T* TemplateLinkedList<T>::GetNum(int num)	// num ��ȣ�� ����Ʈ�� ����.
{
	T* data = nullptr;

	if (!MoveNum(num))
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetNum)\n");
		return nullptr;
	}

	data = GetAt();

	if (data == nullptr)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetNum)\n");
		return nullptr;
	}

	return data;
}

template <typename T>
int TemplateLinkedList<T>::GetCount()	// ����Ʈ ���� Node ������ ����.
{
	return listCount;
}

template <typename T>
void TemplateLinkedList<T>::SetAt(T* data)
{
	if (freeNode == nullptr || freeNode == head || freeNode == tail)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (GetAt)\n");
		return nullptr;
	}

	freeNode->data = data;
}

template <typename T>
void TemplateLinkedList<T>::SetNum(int num, T* data)
{
	if (!MoveNum(num))
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (SetNum)\n");
	}

	freeNode->data = data;

	if (data == nullptr)
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (SetNum)\n");
	}
}

template <typename T>
T* TemplateLinkedList<T>::PeekHead()	// Head�� ���� ����Ʈ �����͸� ������ FreeNode ���� ����.
{
	return head->next->data;
}

template <typename T>
T* TemplateLinkedList<T>::PeekTail()	// Tail�� ���� ����Ʈ �����͸� ������ FreeNode ���� ����.
{
	return tail->prev->data;
}

template <typename T>
T* TemplateLinkedList<T>::RemoveAt()	// FreeNode�� ����� ���� ��带 ����Ŵ.
{
	if (freeNode == nullptr || freeNode == head || freeNode == tail)
	{
		OutputDebugString(L"���� freeNode�� �� �� ���ų�, �Ӹ� Ȥ�� �����Դϴ�. (RemoveAt)\n");
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
		// �����ʹ� �� �ۿ��� delete �� �� ��!!!!
		// ������ ���� �ۿ��� �ϹǷ� �ı��� �ۿ��� �� �ش�.
	}
}

template <typename T>
T* TemplateLinkedList<T>::RemoveNext()	// FreeNode ������ ����.
{
	T* returnNode = nullptr;

	if (!MoveNext())
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (RemoveNext)\n");
		return nullptr;
	}

	returnNode = RemoveAt();

	return returnNode;
}

template <typename T>
T* TemplateLinkedList<T>::RemovePrev()	// FreeNode ������ ����.
{
	T* returnNode = nullptr;

	if (!MovePrev())
	{
		OutputDebugString(L"�����Ͱ� �������� �ʽ��ϴ�. (RemovePrev)\n");
		return nullptr;
	}

	returnNode = RemoveAt();

	return returnNode;
}

template <typename T>
bool TemplateLinkedList<T>::RemoveAll()	// ��� ����.
{
	int tmplistCount = listCount;

	if (tmplistCount == 0)
	{
		OutputDebugString(L"����Ʈ�� �������� �ʽ��ϴ�. (RemoveAll)\n");
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