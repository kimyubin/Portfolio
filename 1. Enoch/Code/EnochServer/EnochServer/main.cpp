#include "EnochServer.h"

int main()
{
	Server* server = new Server();
	server->initServer();
	server->InitContents();
	//initNetwork();
	//InitContents();

	//���⼭while�������� listen�� �����带 ������ �Ե��� ����??
	while (1)
	{
		//system("pause");
	}
}