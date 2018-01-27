#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "cserver.h"
#include "websocket.h"
#include "define.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
//具体实现类
class MyServer :public WebSocketServer {
public:
	void OnMessage(SOCKET socket, const INT8 buffer[], int len) {
		cout << "message:" << buffer << endl;

		this->SendMessage(socket, string(buffer));
	}
	void NewClient(SOCKET socket) {
		cout << "new client" << endl;
	}
	void CloseClient(SOCKET socket) {
		cout << "close client" << endl;
	}
};
int main(void) {
	FServer server;

	Server *mainService = NULL;
	
	int flag = server.InitWsa();
	std::cout << "flag:" << flag << endl;
	if (flag)
	{
		mainService = new MyServer();
		mainService->StartServer();
	}
	return 0;
}