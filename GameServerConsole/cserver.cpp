
#include <stdio.h>
#include <WinSock2.h>
#include "cserver.h"

bool FServer::InitWsa() {
	if (WSAStartup(MAKEWORD(2, 2), &this->wss) != 0)
		return false;
	return true;
}
void print() {
	printf("hello world");
} 