#pragma once

//�ϴ� Ȧ�� REQ ¦�� RES��?
enum class MessageType {
	DEFAULT = 0, //�׽�Ʈ��

	REQ_ECHO = 1,
	RES_ECHO = 2,

	RES_CLIENT_JOIN = 4,
};