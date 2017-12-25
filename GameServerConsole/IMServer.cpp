#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "cserver.h"
#include "define.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
//具体实现类
class MyServer :public Server {
public:
	void NewRequest(SOCKET socket) {
		cout << "new clinet:" <<socket<< endl;
	}
	void CloseRequest(SOCKET socket) {
		cout << "close client:" << socket << endl;
		char buffer[1024];
		sscanf(buffer, "HTTP/1.1 101 Switching Protocols\r\n");
		sscanf(buffer, "Upgrade: websocket\r\n");
		sscanf(buffer, "Connection: Upgrade\r\n");
		sscanf(buffer, "Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWK=\r\n");
		sscanf(buffer, "Sec-WebSocket-Protocol: chat\r\n");
		send(socket, buffer, strlen(buffer), 0);
	}
	void ReceiveMessage(SOCKET socket, INT8 buffer[], INTEGER len) {
		cout << "new message and message len is "<<len<<" :" << buffer << endl;
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