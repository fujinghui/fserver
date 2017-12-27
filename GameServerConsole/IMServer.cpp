#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "cserver.h"
#include "define.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
//����ʵ����
class MyServer :public Server {
public:
	void NewRequest(SOCKET socket) {
		cout << "new clinet:" <<socket<< endl;
	}
	void CloseRequest(SOCKET socket) {
		cout << "close client:" << socket << endl;
	}
	void ReceiveMessage(SOCKET socket, INT8 buffer[], INTEGER len) {
		//cout << "new message and message len is "<<len<<" :" << buffer << endl;
		WebSocketHeader socketWeb;// (string(buffer));
		
		socketWeb.ParseHeader(string(buffer));
		string result = socketWeb.ProductResponse();
		if (socketWeb.header.substr(0, 3) == "GET")
		{
			cout << "���յ���key:" << socketWeb.header << endl;
			send(socket, result.c_str(), result.length(), 0);
			cout << "���͵����ݣ�\n" << socketWeb.ProductResponse() << endl;

			cout << endl << endl;
		}
		else
		{

			//cout << "���յ����ݣ�\n" << buffer << endl;
			printf("���յ����ݣ�");
			for (int i = 0; i < len; i++)
			{
				printf("%d ", (int)buffer[i]);
			}
			printf("\n");
			cout << "���ȣ�" << len << endl;
			//send(socket, "hello", 5, 0);
		}
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