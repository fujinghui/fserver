
#ifndef _FSERVER_H_
#define _FSERVER_H_
#define INFO_PRINT true			//控制是否打印基础信息

#include "define.h"
#include <string>
class FServer {
private:
	WSADATA wss;
public:
	bool InitWsa();
};
//抽象类
class Server {
private:
	static const int MAX_CLIENT_COUNT = 1000;				//最多允许的客户端数量
	struct sockaddr_in localAddr, clientAddr;				//服务
	SOCKET socketServer;									//server
	SOCKET socketClients[MAX_CLIENT_COUNT];					//client的数量
	INT32 socketClientIndex = 0;
	INT32 port = 11111;

	//初始化函数
	void init();
	void Run();
public:
	Server(INT32 port);
	Server();
	void StartServer();

	virtual void NewRequest(SOCKET socket) = 0;				//有一条新的请求
	virtual void CloseRequest(SOCKET socket) = 0;			//关闭了一条请求
	virtual void ReceiveMessage(SOCKET socket, INT8 buffer[], INTEGER len) = 0;			//发送过来新的消息
};


//工具函数
class WebSocketHeader {
public:
	std::string header;
	std::string host;
	std::string upgrade;
	std::string connection;
	std::string sec_websocket_key;
	std::string sec_websocket_protocol;
	std::string sec_websocket_version;
	std::string origin;
public:
	WebSocketHeader();
	WebSocketHeader(std::string &str);					//构造函数
	void ParseHeader(std::string &str);	//解析头部信息
	std::string ProductResponse();
};
#endif