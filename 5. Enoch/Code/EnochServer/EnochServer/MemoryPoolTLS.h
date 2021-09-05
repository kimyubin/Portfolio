#ifndef __MEMORY_POOL_TLS_H__
#define __MEMORY_POOL_TLS_H__

#include "MemoryPool.h"

#define CHUNK_SIZE 200
#define MEMORY_POOL_TLS_BUMP 0xffeeffee


// 사용 금지 -> 지금 터질 때가 많음
template <class T>
class MemoryPoolTLS
{
private:
	struct Chunk;
	//#pragma pack(push,1)
	struct ChunkBlock
	{
		Chunk* pChunk;
		T item;
		int _bump;
		ChunkBlock() : _bump(MEMORY_POOL_TLS_BUMP)
		{
		}
	};
	//#pragma pack(pop)

	struct Chunk
	{
		int AllocIndex;
		int FreeCount;
		ChunkBlock block[CHUNK_SIZE];

		Chunk() :AllocIndex(0),FreeCount(0)
		{
			/*for (int i = 0; i < CHUNK_SIZE; i++)
			{
				block[i].pChunk = this;
			}
			AllocIndex = 0;
			FreeCount = 0;*/
		}

	};


public:
	MemoryPoolTLS(int ChunkNum = 0, bool placement = true);
	~MemoryPoolTLS();
	T* Alloc();
	bool Free(T* data);
	int GetChunkCount() { return _useChunkCount; }
	int GetCount() { return _useCount; }
private:
	DWORD _tlsIndex;
	MemoryPool<Chunk>* _pool;
	int _useChunkCount;
	bool _placementNew;
	int _useCount;
};

template<class T>
MemoryPoolTLS<T>::MemoryPoolTLS(int ChunkNum, bool placement)
	:_useChunkCount(0), _placementNew(placement)
{
	_tlsIndex = TlsAlloc();
	_pool = new MemoryPool<Chunk>(ChunkNum);//chunk 갯수 지정
}

template<class T>
MemoryPoolTLS<T>::~MemoryPoolTLS()
{
	delete _pool;
}

template<class T>
T* MemoryPoolTLS<T>::Alloc()
{

	Chunk* chunk = (Chunk*)TlsGetValue(_tlsIndex);
	ChunkBlock* ret;

	if (chunk == NULL)
	{
		chunk = _pool->Alloc(false);//chunk는 반드시 placement new

		if (chunk == NULL)
		{
			return NULL;
		}
		chunk->AllocIndex = 0;
		chunk->FreeCount = 0;
		TlsSetValue(_tlsIndex, chunk);
	}
	int temp = chunk->AllocIndex;


	ret = &chunk->block[chunk->AllocIndex++];
	ret->pChunk = chunk;

	if (_placementNew)
	{
		new (&(ret->item)) T();
	}

	if (chunk->AllocIndex == CHUNK_SIZE)
	{
		chunk = _pool->Alloc();//chunk는 반드시 placement new

		if (chunk == NULL)
		{
			return NULL;
		}
		TlsSetValue(_tlsIndex, chunk);
	}
	InterlockedIncrement((LONG*)&_useCount);
	return &ret->item;
}

template<class T>
bool MemoryPoolTLS<T>::Free(T* data)
{
	ChunkBlock* chunkBlock = (ChunkBlock*)((LONG64)data - sizeof(ChunkBlock*));
	Chunk* chunk;

	if (chunkBlock->_bump != MEMORY_POOL_TLS_BUMP)
	{
		return false;
	}

	if (_placementNew)
	{
		data->~T();
	}


	chunk = chunkBlock->pChunk;

	int tempFreeCount = InterlockedIncrement((LONG*)&chunk->FreeCount);

	if (tempFreeCount == CHUNK_SIZE)
	{
		if (!_pool->Free(chunk))
		{
			return false;
		}
	}
	InterlockedDecrement((LONG*)&_useCount);
	return true;
}

#endif // !__MEMORY_POOL_TLS_H__
