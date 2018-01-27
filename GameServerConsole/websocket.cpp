#include <Windows.h>
#include <string.h>
#include "websocket.h"
#include "cserver.h"
#include <iostream>
#include <map>			//使用map存储socket
using namespace std;
//
INTEGER WebSocketServer::encoding(string inMessage, string &outFrame, WS_FrameType frameType) {
	int ret = WS_EMPTY_FRAME;
	const uint32_t messageLength = inMessage.size();
	if (messageLength > 32767)
	{
		// 暂不支持这么长的数据  
		return WS_ERROR_FRAME;
	}

	uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
	// header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节  
	uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
	uint8_t *frameHeader = new uint8_t[frameHeaderSize];
	memset(frameHeader, 0, frameHeaderSize);
	// fin位为1, 扩展位为0, 操作位为frameType  
	frameHeader[0] = static_cast<uint8_t>(0x80 | frameType);

	// 填充数据长度  
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

	// 填充数据  
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
	//长度太小
	if (frameLength < 2)
	{
		ret = WS_ERROR_FRAME;
	}
	//检查扩展位并忽略
	if (frameData[0] & 0x70 != 0x0)
	{
		ret = WS_ERROR_FRAME;
	}
	//fin位：为1表示已接收完整报文，为0表示继续监听后续报文
	ret = (frameData[0] & 0x80);
	if ((frameData[0] & 0x80) != 0x80)
	{
		ret = WS_ERROR_FRAME;
	}
	//操作码
	UINT16 payloadLength = 0;
	UINT8 payloadFieldExtraBytes = 0;
	UINT8 opcode = static_cast<UINT8> (frameData[0] & 0x0f);
	if (opcode == WS_TEXT_FRAME)
	{
		//处理UTF-8编码文本
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
			cout << "数据过长" << endl;
			//数据过长，暂时不支持
			ret = WS_ERROR_FRAME;
		}
	}
	else if (opcode == WS_BINARY_FRAME || opcode == WS_PING_FRAME || opcode == WS_PONG_FRAME)
	{
		//二进制/ping/pong帧暂时不处理
	}
	else if (opcode == WS_CLOSING_FRAME)
	{
		ret = WS_CLOSING_FRAME;
	}
	else
	{
		ret = WS_ERROR_FRAME;
	}
	//数据解码
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
	socketSets.erase(socket);			//删除一个socket
}
void WebSocketServer::ReceiveMessage(SOCKET socket, INT8 buffer[], INTEGER len) {
	//查找该socket是否在之前的socket集合中
	map<SOCKET, SOCKET>::iterator it = socketSets.find(socket);
	//没有找到，那么该条请求是一条websocket请求
	if(it == socketSets.end())
	{
		WebSocketHeader webSocketHeader;
		webSocketHeader.ParseHeader(string(buffer));
		//是WebSocket响应头
		if (webSocketHeader.header.substr(0, 3) == "GET")
		{
			string result = webSocketHeader.ProductResponse();
			//发送websocket握手回应信息
			send(socket, result.c_str(), result.length(), 0);
			//调用子类可用的函数
			this->NewClient(socket);
			//将该socket放入集合中
			socketSets.insert(map<SOCKET, SOCKET>::value_type(socket, socket));
		}
	}
	else
	{
		string message;
		//接收到一条新的消息
		//并且解码
		this->decoding(string(buffer), message);
		//客户端关闭连接
		if (message.size() == 0)
		{
			//客户端关闭
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