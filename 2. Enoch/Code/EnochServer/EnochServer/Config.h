#ifndef __CONFIG__
#define __CONFIG__

class Config
{
public:
	bool init();
	int getPort() { return port; };
	int getThreadCnt() { return threadCnt; }
	int getRunningCnt() { return runningCnt; };
private:
	int port = 6000;
	int threadCnt = 6;
	int runningCnt = 3;
	std::string dbId = "root";
	std::string dbPwd = "1234";
};

#endif // !__CONFIG__

