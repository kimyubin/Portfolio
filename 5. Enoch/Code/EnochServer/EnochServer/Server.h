#ifndef __SERVER_H__
#define __SERVER_H__

/// <summary>
/// 게임 서버
/// </summary>
class Server
{
public:
	/// <summary>
	/// 서버 실행
	/// </summary>
	/// <returns>성공 여부</returns>
	bool Start();

	/// <summary>
	/// 세션풀, 스레드풀 등 자원을 초기화 세팅
	/// </summary>
	/// <returns>성공 여부</returns>
	bool Init();

	/// <summary>
	/// 세션풀, 스레드풀 등 자원 반납
	/// </summary>
	/// <returns></returns>
	bool Uninit();

private:
	//acceptThread는 workerThread에서 해당 역할도 가능하도록 수정가능? 옵션으로??
	static unsigned int WINAPI AcceptThread(void* lpParam);
	unsigned int WINAPI AcceptThreadInner();
	static unsigned int WINAPI WorkerThread(void* lpParam);
	unsigned int WINAPI WorkerThreadInner();

private:
	WSADATA _wsa;
	Iocp* _iocp; // completionPort

	int _activeCnt; // active될 수 있는 스레드 수
	int _workerCnt; // 워커스레드 갯수
	int _maxSessionCnt; // 최대 session 수

#ifdef UNICODE
	WCHAR _ip[16];
#elif
	char _ip[16];
#endif
	int _port;
	Socket *_listenSock;
	DWORD _acceptThreadId;
	HANDLE _acceptThread;
	DWORD* _workerIds;
	HANDLE* _workers;

	/// <summary>
	/// 세션은 최대  갯수가지고 미리 생성
	/// 세션을 사용시 array에 직접접근
	/// </summary>
	Session* _sessionAry;

	/// <summary>
	/// 미사용 중인 세션의 index를 쌓아두는 스택
	/// </summary>
	LockFreeStack<int> _sessionIndexStack;
};
#endif // !__SERVER_H__