#include "EnochServer.h"

bool Config::init()
{
	ConfigParser parser;
	parser.init("config.txt"); // �������� �ӽ��̸�

	////////////
	// ���� ������ ��������
	////////////
	if (parser.containsKey("port"))
	{
		port = parser.intValue("port");
	}

	if (parser.containsKey("threadCnt"))
	{
		threadCnt = parser.intValue("threadCnt");
	}

	if (parser.containsKey("runningCnt"))
	{
		runningCnt = parser.intValue("runningCnt");
	}

	if (parser.containsKey("dbId"))
	{
		dbId = parser.stringValue("dbId");
	}

	if (parser.containsKey("dbPwd"))
	{
		dbPwd = parser.stringValue("dbPwd");
	}

	return true;
}