#include "EnochServer.h"

Socket::Socket()
{
	_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
}

Socket::Socket(SOCKET fd)
{
	_sock = fd;
}

Socket::~Socket()
{
	printf("deltet\n");
}

bool Socket::Listen()
{
	//_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (_sock == INVALID_SOCKET)
		return false;

	//nagle 등 욥션 처리 필요??


	if (listen(_sock, SOMAXCONN) == SOCKET_ERROR)
		return false;

	return true;
}

bool Socket::Bind(int port)
{
	ZeroMemory(&_sockAddr, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_sockAddr.sin_port = htons(port);
	if (bind(_sock, (sockaddr *)&_sockAddr, sizeof(sockaddr)) < 0)
	{
		return false;
	}

	return true;
}

bool Socket::Connect()
{
	if (connect(_sock, (sockaddr*)&_sockAddr, sizeof(_sockAddr) < 0)) {
		return false;
	}

	return true;
}

bool Socket::Close()
{
	closesocket(_sock);

	return true;
}

Socket *Socket::Accept()
{
	Socket *sock;
	SOCKET fd = accept(_sock, NULL, 0);

	if (fd == SOCKET_ERROR)
		return NULL;
	
	sock = new Socket(fd);
	InetNtop(AF_INET, &_sockAddr.sin_addr, _ip, 16);


	return sock;
}

bool Socket::AcceptEx(Socket &sock)
{
	//unsigned long rip;
	//bool ret = ::AcceptEx(_sock, sock.getSock(), sock.getIp(), 0, 50, 50, &rip, &_readOverlapped);
	//sock.setRemoteIp(rip);
	
	//return ret;
	return false;
}



SOCKET Socket::GetSock()
{
	return _sock;
}

SOCKADDR_IN& Socket::GetSockAddr()
{
	return _sockAddr;
}

#ifdef UNICODE
WCHAR* Socket::getIp()
{
	return _ip;
}
#elif
char* Socket::getIp()
{
	return _ip;
}
#endif

void Socket::SetRemoteIp(unsigned long rip)
{
	_remoteIp = rip;
}

void Socket::SetEndPoint(const char* addr, int port)
{
	ZeroMemory(&_sockAddr, sizeof(_sockAddr));

	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_sockAddr.sin_port = htons(port);
}
