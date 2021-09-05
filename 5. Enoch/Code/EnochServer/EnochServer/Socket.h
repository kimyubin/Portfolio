#ifndef __SOCKET_H__
#define __SOCKET_H__


/// <summary>
/// 소켓
/// </summary>
class Socket 
{
public:
	Socket();
	Socket(SOCKET fd);
	~Socket();

	/// <summary>
	/// 소켓 Listen
	/// </summary>
	/// <returns>성공여부</returns>
	bool Listen();

	/// <summary>
	/// 소켓 Bind
	/// </summary>
	/// <param name="port">Bind할 포트</param>
	/// <returns>성공여부</returns>
	bool Bind(int port);

	/// <summary>
	/// 소켓 connect
	/// </summary>
	/// <returns>성공여부</returns>
	bool Connect();

	/// <summary>
	/// 소켓 종료
	/// </summary>
	/// <returns>성공 여부</returns>
	bool Close();

	/// <summary>
	/// 소켓 accept
	/// </summary>
	/// <returns>연결된 소켓</returns>
	Socket *Accept();
	
	//작업 필요(현재 사용 금지)
	bool AcceptEx(Socket &sock); 

	SOCKET GetSock();
	SOCKADDR_IN& GetSockAddr();
#ifdef UNICODE
	WCHAR* getIp();
#elif
	char* getIp();
#endif
	
	void SetRemoteIp(unsigned long rip);
	void SetEndPoint(const char* addr, int port);

public:
private:
#ifdef UNICODE
	WCHAR _ip[16];
#elif
	char _ip[16];
#endif
	unsigned long _remoteIp;
	SOCKET _sock;
	SOCKADDR_IN _sockAddr;

	
};
#endif // !__SOCKET_H__
