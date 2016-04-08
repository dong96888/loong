#ifndef _CONNECT_MANAGER_H_
#define _CONNECT_MANAGER_H_


#include <Lock.h>


#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

class TcpServer;
enum ConnectStatus
{
	CONNECTING,
	CONNECTED,
	DISCONNECTED
};

class ConnectManager
{
public:
	ConnectManager(TcpServer * tcpSer);
	~ConnectManager();

	int connect(string ip, uint16_t port);
	bool reconnect(string ip, uint16_t port);
	bool connTreadeServer(string ip, uint16_t port, uint16_t connNum);
	bool connTreadeServer(string ip, uint16_t port);

	void sendMsg(char * buf, uint32_t size);

	int getTradeSock();
private:
	int m_tradeSock;
	int m_status;

	vector<int> m_tradeConnVec;

	Lock m_lock;
	Lock m_sockLst;

	TcpServer *m_tcpSer;
};


#endif

