#ifndef __LOCK_FREE_STACK_H__
#define __LOCK_FREE_STACK_H__

#include "MemoryPool.h"

template<class T>
class LockFreeStack
{
private:
	struct Node
	{
		Node(T data)
		{
			next = NULL;
			item = data;
		}

		Node()
		{
			next = NULL;
		}

		T item;
		Node* next;
	};

	struct TopNode
	{
		TopNode()
		{
			node = NULL;
			check = 0;
		}

		Node* node;
		long long check;
	};

public:
	LockFreeStack();
	~LockFreeStack() {};
	T Top();
	bool Push(T data);
	bool Pop(T* data = NULL);
	bool IsEmpty();
	int GetUseCount() { return _useCount; }

private:
	TopNode* volatile _TopNode;
	unsigned long long _checkNum;
	MemoryPool<Node> _stackPool;

	int _useCount;
};

template<class T>
LockFreeStack<T>::LockFreeStack()
	:_checkNum(0), _useCount(0)
{
	_TopNode = new TopNode();
}

template<class T>
T LockFreeStack<T>::Top()
{
	return _TopNode->node->item;
}

template<class T>
bool LockFreeStack<T>::IsEmpty()
{
	if (_useCount == 0)
	{
		//_useCount가 0으로 확인 후 Push가 들어왔을 경우가 있을 수 있으므로 체크
		if (_TopNode->node == NULL)
			return true;
	}
	return false;
}

template<class T>
bool LockFreeStack<T>::Push(T data)
{
	Node* newNode = _stackPool.Alloc();

	if (newNode == NULL)
		return false;

	newNode->item = data;

	TopNode t;

	unsigned long long checkNum = InterlockedIncrement64((LONG64*)&_checkNum);//이 push행위의 checkNum은 함수 시작 시에 결정

	//interlockedCompareExchange128이 실패할 시 비교 
	//피연산자인 t값에 대상의 값인 _TopNode가 덮어씌워지므로 do while방식과 같게 돌아가게 된다
	while (!InterlockedCompareExchange128((LONG64*)_TopNode, (LONG64)checkNum, (LONG64)newNode, (LONG64*)&t))
	{
		newNode->next = t.node;

	}
	InterlockedIncrement((LONG*)&_useCount);

	return true;
}


template<class T>
bool LockFreeStack<T>::Pop(T* data)
{
	//isEmpty를 atomic할 수 있게
	if (InterlockedDecrement((LONG*)&_useCount) < 0)
	{
		InterlockedIncrement((LONG*)&_useCount);
		return false;
	}

	TopNode t;
	Node* newTop = NULL;
	T popData = _TopNode->node->item;
	unsigned long long checkNum = InterlockedIncrement64((LONG64*)&_checkNum);//이 pop행위의 checkNum은 함수 시작 시에 결정

	//interlockedCompareExchange128이 실패할 시 비교 
	//피연산자인 t값에 대상의 값인 _TopNode가 덮어씌워지므로 do while방식과 같게 돌아가게 된다
	while (!InterlockedCompareExchange128((LONG64*)_TopNode, (LONG64)checkNum, (LONG64)newTop, (LONG64*)&t))
	{
		newTop = (Node*)_TopNode->node->next;

		popData = _TopNode->node->item;

	}

	_stackPool.Free(t.node);

	//top -> next가 자기 자신인 경우는 최초 생성시 만들어진 top뿐이다.

	if (data != NULL)
		*data = popData;

	return true;
}

#endif // !__LOCK_FREE_STACK_H__
