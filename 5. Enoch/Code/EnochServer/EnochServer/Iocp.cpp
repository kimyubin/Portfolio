#include "EnochServer.h"

Iocp::Iocp(int threadCnt, int activeCnt)
	:_threadCount(threadCnt)
{
	if (activeCnt <= 0)
		activeCnt = threadCnt;

	_cp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, activeCnt);
}

bool Iocp::Add(Socket *socket, void* sessionPtr)
{
	if (!CreateIoCompletionPort((HANDLE)socket -> GetSock(), _cp, (ULONG_PTR)sessionPtr, 0))
		return false;
	return true;
}

int Iocp::Wait(OVERLAPPED_ENTRY* events)
{
	int count;

	//TODO Ex가 성능은 좋은데 한번에 여러개의 이벤트를 하나의 스레드가 처리하게될 수 있음
	//대부분의 상황에서는 상관없겠지만 필요에 따라서 1개씩 읽게하는게 나을 수도? Ex사용 여부를 option으로?
	bool result = GetQueuedCompletionStatusEx(_cp, events, MAX_EVENT_COUNT, (ULONG*)&count, INFINITE, false);

	if (!result)
	{
		count = 0;
	}

	return count;
}
