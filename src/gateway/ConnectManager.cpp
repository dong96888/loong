#include "ConnectManager.h"

#include "TcpServer.h"
#include "WorkThread.h"
#include "Connection.h"

#include <unistd.h>



const int EVENTS_SIZE = 1024;
ConnectManager::ConnectManager(TcpServer *tcpSer)
{
	m_tcpSer = tcpSer;
}


ConnectManager::~ConnectManager()
{
}


int ConnectManager::connect(string ip, uint16_t port)
{
	int sockfd = Socket::createSocket();

	//设置为非阻塞
	Socket::setNonBlock(sockfd);

	int ret = Socket::connect(sockfd, Address(ip, port));

	if (ret != 0 && errno != EINPROGRESS)
	{
		close(sockfd);
		return -1;
	}

	//将连接加入监听队列
	m_tcpSer->addEpoll(sockfd, EPOLLET | EPOLLIN | EPOLLOUT);

	return sockfd;
}


//重连socket
bool ConnectManager::reconnect(string ip, uint16_t port)
{
	return connect(ip, port);
}

/*
bool ConnectManager::createConnectPool(string ip, uint16_t port, uint16_t connNum)
{
	for (uint16_t i = 0; i < connNum; i++)
	{
		SmartLock lock(m_lock);
		if (!connect(ip, port))
		{
			ERROR("connect server error!");
			return false;
		}
	}

	return true;
}
*/

bool ConnectManager::connTreadeServer(string ip, uint16_t port, uint16_t connNum)
{
	for (uint16_t i = 0; i < connNum; i++)
	{
		SmartLock lock(m_lock);
		int sockfd = connect(ip, port);
		if (sockfd == -1)
		{
			ERROR("connect server error!");
			return false;
		}
		else
		{
			m_tradeConnVec.push_back(sockfd);
		}
	}

	return true;
}

bool ConnectManager::connTreadeServer(string ip, uint16_t port)
{
	SmartLock lock(m_lock);
	m_tradeSock = connect(ip, port);
	if (m_tradeSock == -1)
	{
		ERROR("connect server error!");
		return false;
	}

	return true;
}

int ConnectManager::getTradeSock()
{
	return m_tradeSock;
}





