#include "EnochServer.h"

Packet *Echo(LONGLONG sessionID, Packet& p)
{
	AcquireSRWLockExclusive(&playerListLock);
	Player* player = (*playerList.find(sessionID)).second;
	ReleaseSRWLockExclusive(&playerListLock);

	Packet* packet = new Packet();
	*packet = p;

	//헤더 타입을 packet에 추가??
	/*Header header;
	header.messageType = MessageType::RES_ECHO;*/

	//SendPacket(sessionID, header, p);

	return packet;
}

