#pragma once
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "cserver.h"
#include "websocket.h"
#include "define.h"
#include "jsonla.h"

using namespace ggicci;
class Message {
public:
	virtual void DealMessage(Json &json) = 0;
	virtual void DealMessage(Json &json, map<SOCKET, INT32> &mapUser) = 0;
	virtual void DealMessage(Json &json, SOCKET socket, map<SOCKET, INT32> &mapUser) = 0;
};