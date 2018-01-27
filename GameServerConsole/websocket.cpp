#include <Windows.h>
#include <string.h>
#include "websocket.h"
#include "cserver.h"
#include <iostream>
#include <map>			//ʹ��map�洢socket
using namespace std;
//
INTEGER WebSocketServer::encoding(string inMessage, string &outFrame, WS_FrameType frameType) {
	int ret = WS_EMPTY_FRAME;
	const uint32_t messageLength = inMessage.size();
	if (messageLength > 32767)
	{
		// �ݲ�֧����ô��������  
		return WS_ERROR_FRAME;
	}

	uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
	// header: 2�ֽ�, maskλ����Ϊ0(������), ������masking key������д, ʡ��4�ֽ�  
	uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
	uint8_t *frameHeader = new uint8_t[frameHeaderSize];
	memset(frameHeader, 0, frameHeaderSize);
	// finλΪ1, ��չλΪ0, ����λΪframeType  
	frameHeader[0] = static_cast<uint8_t>(0x80 | frameType);

	// ������ݳ���  
	if (messageLength <= 0x7d)
	{
		frameHeader[1] = static_cast<uint8_t>(messageLength);
	}
	else
	{
		frameHeader[1] = 0x7e;
		uint16_t len = htons(messageLength);
		memcpy(&frameHeader[2], &len, payloadFieldExtraBytes);
	}

	// �������  
	uint32_t frameSize = frameHeaderSize + messageLength;
	char *frame = new char[frameSize + 1];
	memcpy(frame, frameHeader, frameHeaderSize);
	memcpy(frame + frameHeaderSize, inMessage.c_str(), messageLength);
	frame[frameSize] = '\0';
	outFrame = frame;

	delete[] frame;
	delete[] frameHeader;
	return ret;
}
//
void WebSocketServer::decoding(string frame, string &outMessage) {
	int ret = WS_OPENING_FRAME;
	const char *frameData = frame.c_str();
	const int frameLength = frame.size();
	//����̫С
	if (frameLength < 2)
	{
		ret = WS_ERROR_FRAME;
	}
	//�����չλ������
	if (frameData[0] & 0x70 != 0x0)
	{
		ret = WS_ERROR_FRAME;
	}
	//finλ��Ϊ1��ʾ�ѽ����������ģ�Ϊ0��ʾ����������������
	ret = (frameData[0] & 0x80);
	if ((frameData[0] & 0x80) != 0x80)
	{
		ret = WS_ERROR_FRAME;
	}
	//������
	UINT16 payloadLength = 0;
	UINT8 payloadFieldExtraBytes = 0;
	UINT8 opcode = static_cast<UINT8> (frameData[0] & 0x0f);
	if (opcode == WS_TEXT_FRAME)
	{
		//����UTF-8�����ı�
		payloadLength = static_cast<UINT16>(frameData[1] & 0x7f);
		if (payloadLength == 0x7e)
		{
			INT16 payloadLength16b = 0;
			payloadFieldExtraBytes = 2;
			memcpy(&payloadLength16b, &frameData[2], payloadFieldExtraBytes);
			payloadLength = ntohs(payloadLength16b);
		}
		else if (payloadLength == 0x7f)
		{
			cout << "���ݹ���" << endl;
			//���ݹ�������ʱ��֧��
			ret = WS_ERROR_FRAME;
		}
	}
	else if (opcode == WS_BINARY_FRAME || opcode == WS_PING_FRAME || opcode == WS_PONG_FRAME)
	{
		//������/ping/pong֡��ʱ������
	}
	else if (opcode == WS_CLOSING_FRAME)
	{
		ret = WS_CLOSING_FRAME;
	}
	else
	{
		ret = WS_ERROR_FRAME;
	}
	//���ݽ���
	if ((ret != WS_ERROR_FRAME) && (payloadLength > 0))
	{
		const char * maskingKey = &frameData[2 + payloadFieldExtraBytes];
		char *payloadData = new char[payloadLength + 1];
		memset(payloadData, 0, payloadLength + 1);
		memcpy(payloadData, &frameData[2 + payloadFieldExtraBytes + 4], payloadLength);
		for (int i = 0; i < payloadLength; i++)
		{
			payloadData[i] = payloadData[i] ^ maskingKey[i % 4];
		}
		outMessage = payloadData;
		delete[] payloadData;

	}
}
void WebSocketServer::NewRequest(SOCKET socket) {
		//socketSets[socket] = socket;
		//socketSets.insert(map<SOCKET, SOCKET>::value_type(socket, socket));
}
void WebSocketServer::CloseRequest(SOCKET socket) {
	socketSets.erase(socket);			//ɾ��һ��socket
}
void WebSocketServer::ReceiveMessage(SOCKET socket, INT8 buffer[], INTEGER len) {
	//���Ҹ�socket�Ƿ���֮ǰ��socket������
	map<SOCKET, SOCKET>::iterator it = socketSets.find(socket);
	//û���ҵ�����ô����������һ��websocket����
	if(it == socketSets.end())
	{
		WebSocketHeader webSocketHeader;
		webSocketHeader.ParseHeader(string(buffer));
		//��WebSocket��Ӧͷ
		if (webSocketHeader.header.substr(0, 3) == "GET")
		{
			string result = webSocketHeader.ProductResponse();
			//����websocket���ֻ�Ӧ��Ϣ
			send(socket, result.c_str(), result.length(), 0);
			//����������õĺ���
			this->NewClient(socket);
			//����socket���뼯����
			socketSets.insert(map<SOCKET, SOCKET>::value_type(socket, socket));
		}
	}
	else
	{
		string message;
		//���յ�һ���µ���Ϣ
		//���ҽ���
		this->decoding(string(buffer), message);
		//�ͻ��˹ر�����
		if (message.size() == 0)
		{
			//�ͻ��˹ر�
			this->CloseClient(socket);
		}
		else
		{
			string outmessage(message);
			string realmessage;
			this->encoding(string(outmessage), realmessage, WS_BINARY_FRAME);
			this->OnMessage(socket, realmessage.c_str(), realmessage.length());
		}
	}
}

map<SOCKET, SOCKET> WebSocketServer::getSocketMap() {
	return this->socketSets;
}
void WebSocketServer::SendMessage(SOCKET socket, string &message) {
	string outMessage;
	this->decoding(message, outMessage);
	send(socket, message.c_str(), message.length(), 0);
}