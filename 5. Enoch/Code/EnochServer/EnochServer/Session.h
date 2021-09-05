#ifndef __SESSION_H__
#define __SESSION_H__

/// <summary>
/// 잠금 체크
/// </summary>
struct LockChecker {
	LONG64 useFlag;//사용중인 세션체크용
	LONG64 IOCount;//진행중인 IO의 수 -> 항상 recv를 진행중이므로 0이 되면 세션종료(recv포기)
};

/// <summary>
/// 사용자 세션
/// </summary>
class Session {
public:

	Session();
	~Session();

	void Init();

	Socket* GetSock() { return _sock; }
	RingBuffer& GetRecvQ() { return _recvQ; }
	void SetSock(Socket* sock) { _sock = sock; }

	/// <summary>
	/// 세션 연결종료
	/// </summary>
	void Disconnect();

	WSAOVERLAPPED& GetRecvOverlapped();
	WSAOVERLAPPED& GetSendOverlapped();

	/// <summary>
	/// 현재 IO카운트를 증가
	/// </summary>
	void IncreaseIoCount();

	/// <summary>
	/// 현재 IO카운트를 감소
	/// </summary>
	/// <returns>IO카운트의 zero체크</returns>
	bool DecreaseIoCount();

	/// <summary>
	/// 리시브 대기 걸기
	/// </summary>
	/// <returns> 성공 여부</returns>
	bool RecvPost();

	/// <summary>
	/// 센드 작업 걸기
	/// </summary>
	/// <returns></returns>
	bool SendPost();

	int GetIdx() { return idx; }
	void SetIdx(int idx) { this->idx = idx; }

	LockFreeStack<int>* GetStack() { return _sessionStack; }
	void SetStack(LockFreeStack<int>* sessionStack) { this->_sessionStack =sessionStack; }
private:
	int idx;
	Socket *_sock;
	RingBuffer _recvQ;
	LockChecker *_lockChecker;
	LockFreeStack<int>* _sessionStack;

	bool _disconnectFlag;

	WSAOVERLAPPED _recvOverlapped;
	WSAOVERLAPPED _sendOverlapped;
};

#endif // !__SESSION_H__
