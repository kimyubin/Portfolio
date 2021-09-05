#include "EnochServer.h"

//메인
int main()
{
	Server* server = new Server();

	if (!server->Start()) 
	{
		std::cout << "Server Start Failed" << std::endl;
		return -1;
	}

	std::cout << "Server Start Success" << std::endl;

	for (;;) 
	{
		//모니터링할 경우 모니터링 내용을 시간에 맞춰 출력해주는 메인 스레드 -> 모니터랑 안하게 될 경우 AcceptThread나 다른 용도를 여기서? worker 중 하나??
	}

	return 0;
}