#pragma once

//일단 홀수 REQ 짝수 RES로?
enum class MessageType {
	DEFAULT = 0, //테스트용

	REQ_ECHO = 1,
	RES_ECHO = 2,

	RES_CLIENT_JOIN = 4,
};