#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include "define.h"
#include "cserver.h"

bool FServer::InitWsa() {
	if (WSAStartup(MAKEWORD(2, 2), &this->wss) != 0)
		return false;
	return true;
}

Server::Server(INT32 port) {
	this->port = port;
	this->init();
}
Server::Server() {
	this->init();
}
void Server::init() {
	this->socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->socketServer == INVALID_SOCKET)
	{
		std::cout << "SOCKET创建失败！" << std::endl;
	}
}
//开始服务
void Server::StartServer(){
	if (this->socketServer == INVALID_SOCKET)
	{
		std::cout << "无法开启服务!" << std::endl;
		return;
	}
	this->localAddr.sin_family = AF_INET;
	this->localAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	this->localAddr.sin_port = htons(this->port);
	memset(this->localAddr.sin_zero, 0x00, 8);
	INT32 ret = bind(this->socketServer, (struct sockaddr*)&this->localAddr, sizeof(this->localAddr));
	if (ret != 0)
	{
		std::cout << "bind socket 出错！" << std::endl;
		return;
	}
	ret = listen(this->socketServer, 10);
	if (ret != 0)
	{
		std::cout << "listen socket 出错！" << std::endl;
		return;
	}
#ifdef INFO_PRINT
	std::cout << "服务已开启：" << std::endl;
#endif
	/*
	while (true) {
		INT32 addrLen = sizeof(this->clientAddr);
		SOCKET socketClient = accept(this->socketServer, (struct sockaddr*)&this->clientAddr, &addrLen);
		
		this->socketClients[this->socketClientIndex++] = socketClient;
	}*/
	//开始运行
	this->Run();
}

void Server::Run() {
	fd_set fdSockets;
	INT8 buffer[2048];
	FD_ZERO(&fdSockets);
	FD_SET(this->socketServer, &fdSockets);
	while (true) {
		fd_set fdReadSockets;
		FD_ZERO(&fdReadSockets);
		FD_SET(this->socketServer, &fdReadSockets);
		for (INT32 i = 0; i < this->socketClientIndex; i++)
		{
			FD_SET(this->socketClients[i], &fdReadSockets);
		}
		//选择socket
		INT32 iResult = select(0, &fdReadSockets, NULL, NULL, NULL);
		if (iResult > 0)
		{
			//有客户端新连接
			if (FD_ISSET(this->socketServer, &fdReadSockets))
			{
				struct sockaddr_in addrClient;
				INTEGER len = sizeof(sockaddr_in);
				SOCKET socketClient = accept(this->socketServer, (struct sockaddr*)&addrClient, &len);
				this->socketClients[this->socketClientIndex++] = socketClient;
			}
			else
			{
				for (INTEGER i = 0; i < this->socketClientIndex; i++)
				{
					if (FD_ISSET(this->socketClients[i], &fdReadSockets))
					{
						INTEGER messageLen = recv(this->socketClients[i], buffer, sizeof(buffer), 0);
						if (messageLen > 0)
						{
#ifdef INFO_PRINT
							std::cout << "receive message:" << buffer << std::endl;
#endif
						}
						else						//客户端关闭了链接
						{
							SOCKET closeSocket = this->socketClients[i];
							INTEGER index = i;
							while (index < this->socketClientIndex - 1)
							{
								this->socketClients[index] = this->socketClients[index + 1];
								index++;
							}
							this->socketClientIndex--;
							closesocket(closeSocket);
#ifdef INFO_PRINT
							printf("close socket!socket handle:%d\n", closeSocket);
#endif
						}
					}
				}
			}

		}
	}
}
