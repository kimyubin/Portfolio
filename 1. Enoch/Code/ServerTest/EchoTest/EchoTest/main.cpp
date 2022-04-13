#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "..\..\..\EnochServer\EnochServer\MessageType.h"
#include "..\..\..\EnochServer\EnochServer\define.h"
#include "..\..\..\EnochServer\EnochServer\Packet.h"
#include "..\..\..\EnochServer\EnochServer\RingBuffer.h"

//���� �׽�Ʈ���� ���� �׽�Ʈ �ڵ���
//���� �׽�Ʈ�� �����ϵ��� �ۼ�

int main() 
{
	WSADATA wsa;
	SOCKET sock;

	int retval;

	char buf[512];

	//���� ��� ��
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

	//�α��� ��Ŷ�� ���� ó��
	ZeroMemory(buf, 512);
	recv(sock, buf, 512, 0);

	//���� ������ ���������� ������ �� �߰��ؼ� ����ȭ �׽�Ʈ�� �����ϵ��� �����ʿ�
	//�߰� �߰� ������ ���� ���� ������ ���� ���� ���� �׽�Ʈ�� �߰� �ʿ�
	//������� ���� ������ ���� �׽�Ʈ�� �ݺ��ϴ� �� �߰� �ʿ�
	//������ ������ ���� �� �ƴ� send�� recv ��Ʈ�� ���ؼ� �����ϴ� ��� �߰� �ʿ�

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