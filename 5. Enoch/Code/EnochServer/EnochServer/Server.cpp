#include "EnochServer.h"

bool Server::Init() {

	//TODO 임시로 config파일 방식 추가전에는 고정값으로 지정(config 추가 시 제거 필요)
	std::cout << "Server Init" << std::endl;
	_port = 8080;
	_activeCnt = 4;
	_workerCnt = 8;

	_maxSessionCnt = 100;

	//세션풀,  스레드 풀 등의 자원이 추가되면 해당 위치에서 자원 초기화
	//_sessionPool = new MemoryPoolTLS<Session>(_maxSessionCnt, true);
	_sessionAry = new Session[_maxSessionCnt];

	for (int i = _maxSessionCnt-1; i >= 0; i--)
	{
		_sessionIndexStack.Push(i);
	}

	std::cout << "Pool Init" << std::endl;

	std::cout << "WinSock Init" << std::endl;
	//WinSock 초기화
	if (WSAStartup(MAKEWORD(2, 2), &_wsa) != 0)
		return false;

	return true;
}

bool Server::Uninit() {
	//세션풀,  스레드 풀 등의 자원이 추가되면 해당 위치에서 자원 반납

	WSACleanup();

	return true;
}


unsigned int WINAPI Server::AcceptThread(void* lpParam)
{
	Server* _this = (Server*)lpParam;

	std::cout << "acceptThread Active" << std::endl;

	return _this->AcceptThreadInner();
}

unsigned int __stdcall Server::AcceptThreadInner()
{
	//Session* session = new Session(); //세션풀로 바꿔야함
	Session* session = NULL;
	Socket *sock;
	SOCKADDR_IN sockAddr;
	int addrLen;

	int retval;
	DWORD idCount = 0;
	DWORD uniqueID = 0;

	WCHAR IP[16];
	int sessionIdx;

	for (;;)
	{
		addrLen = sizeof(sockAddr);
		sock = _listenSock->Accept();

		_sessionIndexStack.Pop(&sessionIdx);
		session = &_sessionAry[sessionIdx];
		session->Init();
		session->SetStack(&_sessionIndexStack);
		//session = _sessionPool->Alloc();

		//세션 초기화 작업
		session->SetIdx(sessionIdx);
		session->SetSock(sock);

		//세션 설정 등록

		_iocp->Add(sock, (void*)session);
		session->RecvPost();

		//테스트용 cout
		std::cout << sessionIdx << std::endl;

	}

	return 0;
}

unsigned int WINAPI Server::WorkerThread(void* lpParam)
{
	Server* _this = (Server*)lpParam;

	std::cout << "WorkerThread Active" << std::endl;

	return _this -> WorkerThreadInner();
}

unsigned int WINAPI Server::WorkerThreadInner()
{
	//while(1)
	for (;;)
	{

		//TODO 임시임 100사이즈를 option으로 크기 조절? -> MAX_EVENT_COUNT도 조절 가능해야??
		OVERLAPPED_ENTRY events[100];

		
		int eventCount = _iocp->Wait(events);

		for (int i = 0; i < eventCount; i++) {
			OVERLAPPED_ENTRY& event = events[i];
			Session *session = (Session*)event.lpCompletionKey;

			if (event.dwNumberOfBytesTransferred == 0 && event.lpCompletionKey == 0 && event.lpOverlapped == 0) 
			{
				//workerThread 종료
				break;
			}

			if (event.dwNumberOfBytesTransferred == 0) 
			{
				//zero바이트 이벤트 -> 끊김 처리

				//세션 끊기 추가 필요
				//세션을 세션풀로 돌려줌
				session->Disconnect();
			}
			else if (event.lpOverlapped == &session->GetRecvOverlapped())
			{
				//수신받은 패킷 처리부 -> 패킷구조 정해야함 
				//-> 임시로 정해두면 그걸로 처리부 작성 필요

				//FIXME 테스트용 cout 지울 필요 있음
				std::cout<< session->GetIdx() << ": Recv" << std::endl;

				//수신받은 메시지 처리부 추가

				session->RecvPost();
			}
			else if (event.lpOverlapped == &session->GetSendOverlapped())
			{
				//보낼 패킷 처리부 -> 패킷구조 정해야함
				//-> 임시로 정해두면 그걸로 처리부 작성 필요

				//FIXME 테스트용 cout 지울 필요 있음
				std::cout << session->GetIdx() << ": Send" << std::endl;
			}

			session->DecreaseIoCount();

			//Listen Socket일 경우(옵션으로 처리??)
			//if (event.lpCompletionKey == (ULONG_PTR)_listenSock) 
			//{
			//}
		}
	}

	return 0;
}

bool Server::Start() {
	

	if (!Init())
		return false;

	_iocp = new Iocp(_workerCnt, _activeCnt);
	_listenSock = new Socket();

	if (!_listenSock->Bind(_port))
		return false;

	if (!_listenSock -> Listen())
		return false;

	_iocp->Add(_listenSock, _listenSock);

	//thread 활성화

	_acceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, this, 0, (unsigned int*)&_acceptThreadId);

	_workerIds = new DWORD[_workerCnt];
	_workers = new HANDLE[_workerCnt];

	for (int i = 0; i < _workerCnt; i++)
	{
		_workers[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, (unsigned int*)&_workerIds[i]);
	}

	return true;
}
