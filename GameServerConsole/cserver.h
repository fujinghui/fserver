
#ifndef _FSERVER_H_
#define _FSERVER_H_
#define INFO_PRINT true			//�����Ƿ��ӡ������Ϣ


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
	virtual void ReceiveMessage(SOCKET socket) = 0;			//���͹����µ���Ϣ
};

#endif