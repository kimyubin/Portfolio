#include "EnochServer.h"

Session::Session()
	:_disconnectFlag(false),_sock(NULL)
{
	//_lockChecker.IOCount = 1;
	_lockChecker = new LockChecker();
	_lockChecker->useFlag = true;
	ZeroMemory(&_recvOverlapped, sizeof(_recvOverlapped));
	ZeroMemory(&_sendOverlapped, sizeof(_sendOverlapped));
}

Session::~Session()
{
	delete _sock;
}

void Session::Init()
{
	_lockChecker->IOCount = 0;
	_lockChecker->useFlag = true;
	ZeroMemory(&_recvOverlapped, sizeof(_recvOverlapped));
	ZeroMemory(&_sendOverlapped, sizeof(_sendOverlapped));
}

void Session::Disconnect()
{
	if (InterlockedExchange8((char*)&_disconnectFlag, true))
		return;
	//임시 체크용 cout
	std::cout << "Session Disconnect" << std::endl;
	_sock->Close();

	//delete하지 않는 방향 필요?
	delete _sock;

	//세션 자원 정리
}

WSAOVERLAPPED& Session::GetRecvOverlapped()
{
	return _recvOverlapped;
}

WSAOVERLAPPED& Session::GetSendOverlapped()
{
	return _sendOverlapped;
}

void Session::IncreaseIoCount()
{
	InterlockedIncrement64(&_lockChecker->IOCount);
}

bool Session::DecreaseIoCount()
{
	if (InterlockedDecrement64(&_lockChecker->IOCount) == 0)
	{
		Disconnect();

		//disconnectflag가 false이면 이미 새로운 세션으로 사용될 수 있음 -> 탈출
		if (InterlockedExchange8((char*)&_disconnectFlag, false) == false) 
		{
			return false;
		}

		LockChecker checker;
		checker.IOCount = 0;
		checker.useFlag = true;

		if (!InterlockedCompareExchange128((LONG64*)_lockChecker, (LONG64)1, (LONG64)false, (LONG64*)&checker))
		{
			return false;
		}

		//종료시 진행해야할 작업들 진행

		//세션정리
		GetStack()->Push(GetIdx());

		return false;
	}

	return true;
}

bool Session::RecvPost()
{
	DWORD flags = 0;
	WSABUF wsabuf[2];
	wsabuf[0].len = _recvQ.DirectEnqueueSize();
	wsabuf[0].buf = _recvQ.GetWritePos();
	wsabuf[1].len = _recvQ.GetFreeSize() - _recvQ.DirectEnqueueSize();
	wsabuf[1].buf = _recvQ.GetBufPtr();

	IncreaseIoCount();

	ZeroMemory(&_recvOverlapped, sizeof(_recvOverlapped));

	int retval = WSARecv(_sock->GetSock(), wsabuf, 2, NULL, &flags, &_recvOverlapped, NULL);

	if (retval == SOCKET_ERROR)
	{
		int err = WSAGetLastError();

		if (err != ERROR_IO_PENDING)
		{
			DecreaseIoCount();

			return false;
		}
	}

	return true;
}
