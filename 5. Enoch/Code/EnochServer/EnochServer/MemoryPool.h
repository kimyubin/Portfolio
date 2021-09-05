#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__
#include <Windows.h>
#include <new.h>

#define MEMORY_POOL_BUMP 0xfefefefe

///해당 메모리 풀은 항상 메모리 누수 없는지 체크해보는 게 필요함
///현재 테스트상에서는 없는 것처럼 보이지만 있을 수도 있으니 체크필요??


template <class T>
class MemoryPool
{
public:
	MemoryPool(int blockNum = 0, bool maxLimit = false);
	~MemoryPool();

	T* Alloc(bool placement = true);
	bool Free(T* data);
	int GetUseCount() { return _useCount; }
	int GetFreeCount() { return _maxCapacity == 0 ? 0 : _maxCapacity - _useCount; }
private:
	/// <summary>
	/// 메모리풀을 담고있는 노드
	/// </summary>
	struct Node
	{
		Node() :next(NULL), bump(MEMORY_POOL_BUMP), isPlacement(false)
		{
		}
		Node* next;
		T item;
		bool isPlacement;

		//메모리 영역 침범 체크용 -> 필요없을 수도?
		int bump;
	};

	/// <summary>
	/// top 노드
	/// </summary>
	struct Top
	{
		Top() : node(NULL), cas(0)
		{
		}

		Node* node;
		unsigned long long cas;
	};

	Node* NewNode(bool placement = true)
	{
		Node* newNode = (Node*)malloc(sizeof(Node));
		newNode->next = NULL;
		newNode->bump = MEMORY_POOL_BUMP;
		//Node* temp = new Node();

		if (placement)
		{
			new (&(newNode->item)) T();
			newNode->isPlacement = true;
		}

		return newNode;
	}

private:
	Top* _topNode;
	int _useCount;
	int _maxCapacity;
	bool _maxLimit;
	unsigned long long _checkNum;
};

template<class T>
inline MemoryPool<T>::MemoryPool(int blockNum, bool maxLimit)
	:_maxCapacity(blockNum), _checkNum(0), _maxLimit(maxLimit), _useCount(0)
{
	//blockNum이 default면 반드시 maxLimit는 존재하지 않음
	if (blockNum == 0)
		_maxLimit = false;

	_topNode = new Top();
	Node* cur = _topNode->node;
	for (int i = 0; i < blockNum; i++)
	{
		Node* temp = NewNode(false);

		if (_topNode->node == NULL)
		{
			_topNode->node = temp;
			cur = temp;
		}
		else
		{
			cur->next = temp;
			cur = temp;
		}

	}
}

template<class T>
inline MemoryPool<T>::~MemoryPool()
{
	Node* cur = _topNode->node;
	Node* temp;

	//사용중인 노드가 남아있을 경우 메모리풀의 파괴는 위험할 수 있음
	//다만 무한 루프에 빠질 수도 있으니 체크를 해보고 이득이 나는 쪽으로 수정필요
	while (_useCount != 0);

	while (cur != NULL)
	{
		temp = cur;
		cur = cur->next;

		if (temp->isPlacement)
		{
			//placement new로 item을 생성시 파괴자가 호출되지 않으므로 임의로 호출
			//파괴자의 임의 호출은 원래 권장 x
			temp->item.~T();
		}

		delete temp;
	}
	delete _topNode;
}

template<class T>
inline T* MemoryPool<T>::Alloc(bool placement)
{
	Node* ret = NULL;
	Node* newTop = NULL;
	Top t;
	unsigned long long checkNum;

	//현재 최대 node보다 많을 경우
	if (InterlockedIncrement((LONG*)&_useCount) > _maxCapacity)
	{
		//최대값 제한 걸려있을 경우 실패
		if (_maxLimit)
		{
			InterlockedDecrement((LONG*)&_useCount);
			return NULL;
		}
		else // 최대값 제한이 없을 경우 새로운 node 생성
		{
			InterlockedIncrement((LONG*)&_maxCapacity);

			ret = NewNode(placement);

			return &(ret->item);
		}
	}

	checkNum = InterlockedIncrement64((LONG64*)&_checkNum);//이 pop행위의 checkNum은 함수 시작 시에 결정

	while (!InterlockedCompareExchange128((LONG64*)_topNode, (LONG64)checkNum, (LONG64)newTop, (LONG64*)&t))
	{
		newTop = (Node*)_topNode->node->next;

		ret = _topNode->node;
	}

	if (ret == NULL)
		return NULL;

	if (placement)
	{
		ret->isPlacement = true;
		new (&(ret->item)) T();
	}

	return &(ret->item);
}

template<class T>
inline bool MemoryPool<T>::Free(T* data)
{
	//pointer 이동으로 node의 포인터를 찾음
	Node* temp = (Node*)((LONG64)data - sizeof(Node*));
	Top t;
	unsigned long long checkNum;

	if (temp->isPlacement)
	{
		//placement new로 item을 생성시 파괴자가 호출되지 않으므로 임의로 호출
		//파괴자의 임의 호출은 원래 권장 x
		data->~T();
	}

	//bump침범 문제 없는 것 확인 시 제거?
	if (temp->bump != MEMORY_POOL_BUMP)
	{
		return false;
	}

	checkNum = InterlockedIncrement64((LONG64*)&_checkNum);//이 push행위의 checkNum은 함수 시작 시에 결정

	while (!InterlockedCompareExchange128((LONG64*)_topNode, (LONG64)checkNum, (LONG64)temp, (LONG64*)&t))
	{
		temp->next = t.node;
	}

	InterlockedDecrement((LONG*)&_useCount);

	return false;
}


#endif // !__MEMORY_POOL_H__

