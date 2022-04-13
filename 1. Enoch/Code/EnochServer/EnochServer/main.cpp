#include "EnochServer.h"

int main()
{
	Server* server = new Server();
	server->initServer();
	server->InitContents();
	//initNetwork();
	//InitContents();

	//여기서while걸지말고 listen쪽 스레드를 메인이 먹도록 수정??
	while (1)
	{
		//system("pause");
	}
}