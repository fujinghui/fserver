
#ifndef _FSERVER_H_
#define _FSERVER_H_
#define INFO_PRINT true			//�����Ƿ��ӡ������Ϣ

#include "define.h"
#include <string>
class FServer {
private:
	WSADATA wss;
public:
	bool InitWsa();
};
//������
class Server {
private:
	static const int MAX_CLIENT_COUNT = 1000;				//�������Ŀͻ�������
	struct sockaddr_in localAddr, clientAddr;				//����
	SOCKET socketServer;									//server
	SOCKET socketClients[MAX_CLIENT_COUNT];					//client������
	INT32 socketClientIndex = 0;
	INT32 port = 11111;

	//��ʼ������
	void init();
	void Run();
public:
	Server(INT32 port);
	Server();
	void StartServer();

	virtual void NewRequest(SOCKET socket) = 0;				//��һ���µ�����
	virtual void CloseRequest(SOCKET socket) = 0;			//�ر���һ������
	virtual void ReceiveMessage(SOCKET socket, INT8 buffer[], INTEGER len) = 0;			//���͹����µ���Ϣ
};


//���ߺ���
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
	WebSocketHeader(std::string &str);					//���캯��
	void ParseHeader(std::string &str);	//����ͷ����Ϣ
	std::string ProductResponse();
};
#endif