#pragma once
#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_
#include <Windows.h>
#include <string.h>
#include "cserver.h"
#include <iostream>
#include <map>
using namespace std;

enum WS_Status {
	WS_STATUS_CONNECT = 0,
	WS_STATUS_UNCONNECT = 1,
};
enum WS_FrameType {
	WS_EMPTY_FRAME = 0xF0,
	WS_ERROR_FRAME = 0xF1,
	WS_TEXT_FRAME = 0x01,
	WS_BINARY_FRAME = 0x02,
	WS_PING_FRAME = 0x09,
	WS_PONG_FRAME = 0x0A,
	WS_OPENING_FRAME = 0xF3,
	WS_CLOSING_FRAME = 0x08
};

class WebSocketServer :public Server {
private :
	map<SOCKET, SOCKET> socketSets;	//socket
	INTEGER encoding(string frame, string &outMessage, WS_FrameType frameType);									//����
	void decoding(string frame, string &outMessage);															//����
public:
	void NewRequest(SOCKET socket);
	void CloseRequest(SOCKET socket);
	void ReceiveMessage(SOCKET socket, INT8 buffer[], INTEGER len);

	/*
		���ܺ���
	*/
	map<SOCKET, SOCKET> getSocketMap();			//�������е�map����
	void SendMessage(SOCKET socket, string &message);			//������Ϣ
	/*
		date	: 2018.1.22
		author	: femy
		describe: �������ദ��ķ���
	*/
	virtual void OnMessage(SOCKET socket, const INT8 buffer[], INTEGER len) = 0;
	virtual void NewClient(SOCKET socket) = 0;		//�µ�websocket�ͻ���
	virtual void CloseClient(SOCKET socket) = 0;	//�ر�websocket�ͻ���
};


#endif