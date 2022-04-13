#ifndef __ENOCH_SERVER__
#define __ENOCH_SERVER__
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <process.h>
#include <map>
#include "Packet.h"
#include "RingBuffer.h"
#include "Session.h"

//사용할 세션매니저로 변경해서 사용필요??
#include "ProtoSessionManager.h"

#include "MessageType.h"
#include "define.h"
#include "Server.h"
#include "PacketProc.h"
#include "Player.h"


#endif //__ENOCH_SERVER__