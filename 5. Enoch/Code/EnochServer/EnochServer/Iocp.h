#ifndef __IOCP_H__
#define __IOCP_H__

#define MAX_EVENT_COUNT 100

/// <summary>
/// Completion Port

/// </summary>
class Iocp 
{
public:
	Iocp(int threadCnt, int activeCnt = 0);

	/// <summary>
	/// CompletionPort에 등록
	/// </summary>
	/// <param name="socket">등록할 소켓</param>
	/// <param name="sessionPtr">등록할 세션의 주소</param>
	/// <returns>성공 여부</returns>
	bool Add(Socket *socket, void *sessionPtr);

	/// <summary>
	/// 대기중인 이벤트 처리
	/// </summary>
	/// <param name="events">완료된 이벤트들</param>
	/// <returns>완료된 이벤트의 count</returns>
	int Wait(OVERLAPPED_ENTRY *events);
private:
	HANDLE _cp = NULL; // completionPort
	int _threadCount;
};

#endif // !__IOCP_H__
