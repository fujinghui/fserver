// GameServerConsole.cpp: 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <windows.h>
#include "cserver.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main222()
{
	FServer server;///new FServer;
	if (server.InitWsa())
		MessageBox(NULL, "create success!", "success", MB_OK);
	else
		MessageBox(NULL, "create faild!", "faild", MB_OK);
    return 0;
}

