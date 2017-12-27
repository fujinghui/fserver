
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

#include <WinSock2.h>
#include "define.h"

#include "sha1.h"
#include "cserver.h"
#include "base64.h"

using namespace std;
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
		std::cout << "SOCKET����ʧ�ܣ�" << std::endl;
	}
}
//��ʼ����
void Server::StartServer(){
	if (this->socketServer == INVALID_SOCKET)
	{
		std::cout << "�޷���������!" << std::endl;
		return;
	}
	this->localAddr.sin_family = AF_INET;
	this->localAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	this->localAddr.sin_port = htons(this->port);
	memset(this->localAddr.sin_zero, 0x00, 8);
	INT32 ret = bind(this->socketServer, (struct sockaddr*)&this->localAddr, sizeof(this->localAddr));
	if (ret != 0)
	{
		std::cout << "bind socket ����" << std::endl;
		return;
	}
	ret = listen(this->socketServer, 10);
	if (ret != 0)
	{
		std::cout << "listen socket ����" << std::endl;
		return;
	}
#ifdef INFO_PRINT
	std::cout << "�����ѿ�����" << std::endl;
#endif
	/*
	while (true) {
		INT32 addrLen = sizeof(this->clientAddr);
		SOCKET socketClient = accept(this->socketServer, (struct sockaddr*)&this->clientAddr, &addrLen);
		
		this->socketClients[this->socketClientIndex++] = socketClient;
	}*/
	//��ʼ����
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
		//ѡ��socket
		INT32 iResult = select(0, &fdReadSockets, NULL, NULL, NULL);
		if (iResult > 0)
		{
			//�пͻ���������
			if (FD_ISSET(this->socketServer, &fdReadSockets))
			{
				struct sockaddr_in addrClient;
				INTEGER len = sizeof(sockaddr_in);
				SOCKET socketClient = accept(this->socketServer, (struct sockaddr*)&addrClient, &len);
				this->socketClients[this->socketClientIndex++] = socketClient;
				this->NewRequest(socketClient);			//���ó����������µĿͻ���
			}
			else
			{
				for (INTEGER i = 0; i < this->socketClientIndex; i++)
				{
					if (FD_ISSET(this->socketClients[i], &fdReadSockets))
					{
						INTEGER messageLen = recv(this->socketClients[i], buffer, sizeof(buffer), 0);
						buffer[messageLen] = '\0';
						if (messageLen > 0)
						{
#ifdef INFO_PRINT
							//std::cout << "receive message:" << buffer << std::endl;
#endif
							//���ó����������µ���Ϣ����
							this->ReceiveMessage(this->socketClients[i], buffer, messageLen);			//
						}
						else						//�ͻ��˹ر�������
						{
							SOCKET closeSocket = this->socketClients[i];
							INTEGER index = i;
							while (index < this->socketClientIndex - 1)
							{
								this->socketClients[index] = this->socketClients[index + 1];
								index++;
							}
							this->socketClientIndex--;
							//�رտͻ���֮���øú���
							this->CloseRequest(closeSocket);
							closesocket(closeSocket);
#ifdef INFO_PRINT
							//printf("close socket!socket handle:%d\n", closeSocket);
#endif
						}
					}
				}
			}

		}
	}
}


//WebSocketͷ����Ϣ������
/*
	������Ϣ��
	GET /chat HTTP/1.1
	Host: server.example.com
	Upgrade: websocket
	Connection: Upgrade
	Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==
	Sec-WebSocket-Protocol: chat, superchat
	Sec-WebSocket-Version: 13
	Origin: http://example.com
*/
WebSocketHeader::WebSocketHeader() {

}
//���캯��
WebSocketHeader::WebSocketHeader(string &str) {
	this->ParseHeader(str);
}
//��������
void WebSocketHeader::ParseHeader(string &str) {
	std::istringstream stream(str);
	string reqType;
	getline(stream, reqType);			//��ȡͷ����Ϣ��һ�м�¼
	if (reqType.substr(0, 3) != "GET")	//websocketͷ����Ϣ��ǰ�ĸ��ֽڱ�����GET
		return;
	this->header = reqType;				//����ͷ����Ϣ
	string header;
	string::size_type pos = 0;
	string websocketKey;
	while (getline(stream, header) && header != "\r")
	{
		header.erase(header.end() - 1);	//ȥ�����һ�����з�"\n"
		pos = header.find(": ", 0);
		if (pos != string::npos)
		{
			string key = header.substr(0, pos);
			string value = header.substr(pos + 2);
			if (key == string("Host"))
			{
				this->host = value;
			}
			else if(key == string("Upgrade"))
			{
				this->upgrade = value;
			}
			else if (key == string("Connection"))
			{
				this->connection = value;
			}
			else if (key == string("Sec-WebSocket-Key"))
			{
				this->sec_websocket_key = value;
			}
			else if (key == string("Sec-WebSocket-Protocol"))
			{
				this->sec_websocket_protocol = value;
			}
			else if (key == string("Sec-WebSocket-Version"))
			{
				this->sec_websocket_version = value;
			}
			else if (key == string("Origin"))
			{
				this->origin = value;
			}
		}
	}
}
string WebSocketHeader::ProductResponse() {
	string magicKey("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	string serverKey = this->sec_websocket_key + magicKey;
	char shaHash[32];
	memset(shaHash, 0, sizeof(shaHash));

	SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << serverKey.c_str();
	sha.Result(message_digest);
	for (int i = 0; i < 5; i++)
		message_digest[i] = htonl(message_digest[i]);
	serverKey = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
	serverKey += "\r\n";
	

	//char buffer[1024];
	string buffer;
	buffer.append("HTTP/1.1 101 Switching Protocols\r\n");
	buffer.append("Upgrade: websocket\r\n");
	buffer.append("Connection: Upgrade\r\n");
	buffer.append("Sec-WebSocket-Accept: ");
	buffer.append(serverKey);
	//buffer.append("Sec-WebSocket-Protocol: chat");
	buffer.append("\r\n\0");
	return buffer;
}