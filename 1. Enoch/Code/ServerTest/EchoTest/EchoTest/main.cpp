#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "..\..\..\EnochServer\EnochServer\MessageType.h"
#include "..\..\..\EnochServer\EnochServer\define.h"
#include "..\..\..\EnochServer\EnochServer\Packet.h"
#include "..\..\..\EnochServer\EnochServer\RingBuffer.h"

//에코 테스트만을 위한 테스트 코드임
//대충 테스트만 가능하도록 작성

int main() 
{
	WSADATA wsa;
	SOCKET sock;

	int retval;

	char buf[512];

	//소켓 통신 부
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(PORT);

	sock = socket(AF_INET, SOCK_STREAM, 0);


	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR)
	{
		printf("error\n");
		return 0;
	}
	Header header;

	header.messageType = MessageType::REQ_ECHO;

	//로그인 패킷에 대한 처리
	ZeroMemory(buf, 512);
	recv(sock, buf, 512, 0);

	//연속 데이터 무지성으로 날리는 걸 추가해서 동기화 테스트도 가능하도록 수정필요
	//중간 중간 재접속 등을 통해 나갔다 들어올 때의 상태 테스트도 추가 필요
	//쓰레드로 여러 계정이 위의 테스트를 반복하는 것 추가 필요
	//응답을 눈으로 보는 게 아닌 send와 recv 스트링 비교해서 검증하는 기능 추가 필요

	while (1)
	{
		printf("send msg : ");
		scanf("%s", buf + sizeof(header));
		int len = strlen(buf + sizeof(header));
		header.len = len;
		memcpy(buf, &header, sizeof(header));

		send(sock, buf, len + sizeof(header), 0);

		ZeroMemory(buf, 512);
		recv(sock, buf, len + sizeof(header), 0);

		buf[len + sizeof(header)] = '\0';

		printf("recv msg : %s\n", buf + sizeof(header));
		ZeroMemory(buf, 512);
	}
}