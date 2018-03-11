#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include "cserver.h"
#include "websocket.h"
#include "define.h"
#include "jsonla.h"
#include "FSQL.h"
#include "message.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "odbc32.lib")

using namespace std;
using namespace ggicci;

CSQL *csql = NULL;
Server *mainService = NULL;

//���������û�������״̬
void UpdateOnlineStatus(map<SOCKET, INT32> &mapUser) {
	csql->execute("update users set is_login=0;");
	//����map�������е��ѵ�¼���ֶ��޸ĳ�1
	map<SOCKET, INT32>::iterator it;
	it = mapUser.begin();
	char sqlstr[128];
	while (it != mapUser.end()) {
		sprintf(sqlstr, "update users set is_login=1 where id=%d;\0", it->second);
		if (csql->execute(sqlstr)) {

		}
		it++;
	}
	cout << "���������û��ĵ�¼״̬��ϣ�" << endl;
}
SOCKET FindSocketByUserId(map<SOCKET, INT32> &mapUser, int user_id) {
	SOCKET socket = -1;
	map<SOCKET, INT32>::iterator it = mapUser.begin();
	while (it != mapUser.end()) {
		if (it->second == user_id) {
			return it->first;
		}
		it++;
	}
	return socket;
}
//��������Ϣ
class SendMessageType : public Message {
private:
	//Ҫִ�е�sql
	char sqlstr[1024];			
public:
	void DealMessage(Json &json) {
	}
	void DealMessage(Json &json, map<SOCKET, INT32> &mapUser) {
		int user_id = json["user_id"].AsInt();
		int friend_id = json["friend_id"].AsInt();
		int is_send = 0;																//�Ƿ�Ҫ����
		string message = json["message"].AsString();
		time_t zeitstempel;
		time(&zeitstempel);

		SOCKET socketFriend = FindSocketByUserId(mapUser, friend_id);					//���Һ����Ƿ��Ѿ���¼
		if (socketFriend != -1) {
			is_send = 1;																//�����Ѿ����յ���Ϣ
			json["date"] = (INT32)zeitstempel*1000;
			//��ú��ѷ�����Ϣ
			((WebSocketServer*)mainService)->SendMessage(socketFriend, json.ToString());

			cout << "message:" << message << endl;
		}

		

		sprintf(sqlstr, "insert into message(send_id,receive_id,message,is_send,date) values(%d,%d,'%s',%d,%d);",
			user_id,friend_id,message.c_str(), is_send, (INT32)zeitstempel*1000);
		//cout << sqlstr << endl;
		csql->execute(sqlstr);
	}
	void DealMessage(Json &json, SOCKET socket, map<SOCKET, INT32> &mapUser) {

	}
};
//�����¼��Ϣ����
class LoginMessageType : public Message {
public:
	void DealMessage(Json &json) {

	}
	void DealMessage(Json &json, map<SOCKET, INT32> &mapUser) {

	}
	void DealMessage(Json &json, SOCKET socket, map<SOCKET, INT32> &mapUser) {
		int user_id = json["user_id"].AsInt();		//��ȡ��ǰ��¼���û���id
		mapUser[socket] = user_id;
		UpdateOnlineStatus(mapUser);				
		//�����ݿ��л�ȡ�Ƿ���û�ж�ȡ����Ϣ������У���ô��ȡ����������
	}
};
class LogoutMessageType : public Message {
public:
	void DealMessage(Json &json) {

	}
	void DealMessage(Json &json, map<SOCKET, INT32> &mapUser) {
		int user_id = json["user_id"].AsInt();
		map<SOCKET, INT32>::iterator it = mapUser.begin();
		SOCKET socketDel = -1;
		char sqlstr[128];
		while (it != mapUser.end()) {
			if (it->second == user_id) {
				socketDel = it->first;
				sprintf(sqlstr, "update users set is_login=0 where id=%d;\0", user_id);
				csql->execute(sqlstr);
			}
			it++;
		}
		if (socketDel != -1) {
			mapUser.erase(socketDel);
		}
	}
	void DealMessage(Json &json, SOCKET socket, map<SOCKET, INT32> &mapUser) {
		int user_id = mapUser[socket];
		char sqlstr[128];
		sprintf(sqlstr, "update users set is_login=0 where id=%d;\0", user_id);
		csql->execute(sqlstr);
		mapUser.erase(socket);
		cout << "���û��˳���ʣ���û��У�" << endl;
		map<SOCKET, INT32>::iterator it = mapUser.begin();
		while (it != mapUser.end()) {
			cout << it->first << ":" << it->second << endl;
			it++;
		}
	}
};
//����ʵ����
class MyServer :public WebSocketServer {
private:
	map<SOCKET, INT32> mapUser;					//socket��id�Ķ�Ӧ
public:
	static const INT32 MESSAGE_TYPE = 1;		//������Ϣ
	static const INT32 LOGIN_TYPE = 2;
	static const INT32 LOGOUT_TYPE = 3;
	void OnMessage(SOCKET socket, const INT8 buffer[], int len) {
		try {
			Json json = Json::Parse(buffer);			//ת����json����
			Message *message = NULL;// new SendMessageType();
			if (json["flag"].AsInt() == MESSAGE_TYPE) {
				message = new SendMessageType();
			}
			else if (json["flag"].AsInt() == LOGIN_TYPE) {
				message = new LoginMessageType();
			}
			else if (json["flag"].AsInt() == LOGOUT_TYPE) {
				message = new LogoutMessageType();
			}
			//�����Ϊ��������Ϣ
			if (message != NULL) {
				message->DealMessage(json);
				message->DealMessage(json, this->mapUser);
				message->DealMessage(json, socket, this->mapUser);
			}
		}
		catch (const char *msg) {
			cout << msg << endl;
		}
	}
	void NewClient(SOCKET socket) {
	}
	void CloseClient(SOCKET socket) {
		Json json;
		(new LogoutMessageType())->DealMessage(json, socket, this->mapUser);
	}
};
int main(void) {
	string jsonstr = "{\"a\":\"fujinghui\"}";
	FServer server;
	int flag = server.InitWsa();
	//cout << json["a"].AsString();
	csql = new CSQL();				//��ʼ��
	if (flag)
	{
		mainService = new MyServer();
		mainService->StartServer();
	}
	return 0;
}