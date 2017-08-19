
#ifndef _FSERVER_H_
#define _FSERVER_H_
void print();
class FServer {
private:
	WSADATA wss;
public:
	bool InitWsa();
};
#endif