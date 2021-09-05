#pragma once
#define _WINSOCKAPI_
#include <Windows.h>
#include <string>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <mswsock.h>
#include <iostream>
#include <process.h>

#include "MemoryPool.h"
#include "MemoryPoolTLS.h"
#include "LockFreeStack.h"

#include "RingBuffer.h"
#include "Socket.h"
#include "Session.h"
#include "Iocp.h"
#include "Server.h"


#pragma comment(lib, "ws2_32")