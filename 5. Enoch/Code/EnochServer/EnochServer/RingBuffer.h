#ifndef __RING_BUFFER__
#define __RING_BUFFER__

#define RINGBUF_DEFAULT_SIZE 50000

/// <summary>
/// 원형 큐 형태의 버퍼
/// </summary>
class RingBuffer
{
public:
	RingBuffer();
	RingBuffer(int iBufferSize);
	~RingBuffer();

private:
	int _size;
	int _front;
	int _rear;
	int _capacity;
	char* _buf;
	SRWLOCK srwLock;

private:
	void Initial(int iBufferSize);
public:

	bool isFull();
	bool isEmpty();

	/// <summary>
	/// 버퍼의 크기 구하기
	/// </summary>
	/// <returns>버퍼의 크기</returns>
	int GetBufferSize() const;

	/// <summary>
	/// 초기화
	/// </summary>
	void Reset();

	/// <summary>
	/// 현재 사용중인 용량 얻기.
	/// </summary>
	/// <returns>사용중인 용량.</returns>
	int GetUseSize() const;

	/// <summary>
	/// 현재 버퍼에 남은 용량 얻기. 
	/// </summary>
	/// <returns>남은용량</returns>
	int GetFreeSize() const;

	/// <summary>
	/// 버퍼 포인터에서 1번에 쓸 수 있는 길이
	/// </summary>
	/// <returns>사용가능 용량</returns>
	int DirectEnqueueSize() const;

	/// <summary>
	/// 버퍼 포인터에서 1번에 읽을 수 있는 길이
	/// </summary>
	/// <returns>사용가능 용량</returns>
	int DirectDequeueSize(void) const;

	/// <summary>
	/// 버퍼에 삽입
	/// </summary>
	/// <param name="chpData">삽입 데이터</param>
	/// <param name="iSize">삽입 데이터의 크기</param>
	/// <returns>삽입된 데이터의 크기</returns>
	int Enqueue(char* chpData, int iSize);

	/// <summary>
	/// 버퍼에서 빼냄
	/// </summary>
	/// <param name="chpData">빼낸 데이터</param>
	/// <param name="iSize">빼낸 데이터의 크기</param>
	/// <returns>빼낸 데이터의 크기</returns>
	int Dequeue(char* chpData, int size);
#ifdef _XSTRING_
	int Enqueue(std::wstring* str, int iSize);
	int Dequeue(std::wstring* str, int iSize);
#endif

	/// <summary>
	/// 데이터 복제(front변경 x)
	/// </summary>
	/// <param name="chpData">복제할 데이터 배열</param>
	/// <param name="size">복제된 데이터 크기</param>
	/// <returns></returns>
	int Peek(char* chpData, int size);

	/// <summary>
	/// Front 이동
	/// </summary>
	/// <param name="size">이동할 크기</param>
	/// <returns>성공 여부</returns>
	bool MoveFront(int size);

	/// <summary>
	/// Rear 이동
	/// </summary>
	/// <param name="size">이동할 크기</param>
	/// <returns>성공 여부</returns>
	bool MoveRear(int size);

	/// <summary>
	/// 쓰기 위치 이동
	/// </summary>
	/// <param name="size">이동할 크기</param>
	/// <returns>성공 여부</returns>
	bool MoveWritePos(int size) { return MoveRear(size); }

	/// <summary>
	/// 읽기 위치 이동
	/// </summary>
	/// <param name="size">이동할 크기</param>
	/// <returns>성공 여부</returns>
	bool MoveReadPos(int size) { return MoveFront(size); }

	/// <summary>
	/// 읽기 위치 구하기
	/// </summary>
	/// <returns>읽기 위치</returns>
	char* GetWritePos() const;

	/// <summary>
	/// 쓰기 위치 구하기
	/// </summary>
	/// <returns>쓰기 위치</returns>
	char* GetReadPos() const;

	/// <summary>
	/// 버퍼의 시작 위치
	/// </summary>
	/// <returns>버퍼 포인터</returns>
	char* GetBufPtr() const;

	/// <summary>
	/// 버퍼 락
	/// </summary>
	void Lock();

	/// <summary>
	/// 버퍼 언락
	/// </summary>
	void UnLock();
};

#endif // !__RING_BUFFER__