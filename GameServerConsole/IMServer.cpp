#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "cserver.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
class MyServer :public Server {


public:
	void NewRequest(SOCKET socket) {

	}
	void CloseRequest(SOCKET socket) {

	}
	void ReceiveMessage(SOCKET socket) {

	}
};
int main(void) {
	FServer server;

	MyServer *mainService = NULL;

	int flag = server.InitWsa();
	std::cout << "flag:" << flag << endl;
	if (flag)
	{
		mainService = new MyServer();
		mainService->StartServer();
	}
	return 0;
}