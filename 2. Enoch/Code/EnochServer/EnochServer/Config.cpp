#include "EnochServer.h"

bool Config::init()
{
	ConfigParser parser;
	parser.init("config.txt"); // 설정파일 임시이름

	////////////
	// 파일 설정값 가져오기
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