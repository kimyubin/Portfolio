#include "EnochServer.h"

std::map <LONGLONG, Player*> playerList;
SRWLOCK playerListLock;

Player::Player(LONGLONG id)
	:sessionID(id)
{
	InitializeSRWLock(&playerLock);
}

void Player::Lock()
{
	AcquireSRWLockExclusive(&playerLock);
}

void Player::UnLock()
{
	ReleaseSRWLockExclusive(&playerLock);
}