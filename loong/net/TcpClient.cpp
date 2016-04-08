#include "TcpClient.h"

#include "Socket.h"
#include "Address.h"
#include "Logger.h"
#include "TcpServer.h"

#include <stdio.h>
#include <string.h>

#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <thread>

#include "Channel.h"

using namespace std;
namespace loong
{
	TcpClient::TcpClient(EventLoop *loop)
	:m_ip(""),m_port(0),m_chan(new Channel),
	m_loop(loop)
	{
		
	}

	TcpClient::~TcpClient()
	{
		if (m_chan!=nullptr)
		{
			delete m_chan;
		}
		m_th->join();
		delete m_th;
	}

	void TcpClient::connect(string ip, uint16_t port)
	{
		m_ip=ip;
		m_port=port;
		
		m_sockfd = Socket::createSocket();

		//设置为非阻塞
		Socket::setNonBlock(m_sockfd);

		int keepAlive = 1; // 开启keepalive属性
		int keepIdle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测
		int keepInterval = 5; // 探测时发包的时间间隔为5 秒
		int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

		setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
		setsockopt(m_sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
		setsockopt(m_sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
		setsockopt(m_sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));

		int ret = Socket::connect(m_sockfd, Address(m_ip, m_port));
		if(ret < 0)
		{
			if(ret == EINPROGRESS)
			{
				m_status = connecting;
				m_chan->setSockFd(m_sockfd);
				m_chan->setWriteCallback(bind(&TcpClient::handleConn,this));
				m_chan->setErrCallback(bind(&TcpClient::handleError,this));
				m_loop->addEvent(m_chan,EPOLLET|EPOLLOUT|EPOLLERR);
			}
			else
			{
				handleError();
				return;
			}
		}
		else
		{
			m_status=connected;
			onNewConn();
		}

		return;
	}

	void TcpClient::handleConn()
	{
		if(m_status == connecting)
		{
			if(Socket::getSockError(m_sockfd) != 0)
			{
				handleError();
			}
			else
			{
				m_status=connected;
				onNewConn();
			}
		}
	}

	void TcpClient::onNewConn()
	{
		//删除原来检测连接的监听事件
		m_loop->delEvent(m_chan, EPOLLET | EPOLLOUT | EPOLLERR);

		m_conn=make_shared<TcpConn>(m_loop,m_sockfd);
		m_conn->setMessageCallback(m_messageCallback);
		m_conn->setCloseCallback(m_closeCallback);
		m_conn->setErrCallback(m_errCallback);

		printf("client connneted!\n");
		m_conn->onEstablished();
		
		if (m_connCallback!=nullptr)
		{
			m_connCallback(m_conn);
		}
	}

	bool TcpClient::sendData(const void *data,size_t len)
	{
		if (m_status == connected)
		{
			m_conn->sendData(data,len);
			return true;
		}
		else
		{
			printf("not connect server\n");
			return false;
		}
	}

	void TcpClient::handleError()
	{
		if (m_status == connecting
				|| m_status == disconnected)
		{
			m_loop->delEvent(m_chan,EPOLLET|EPOLLOUT|EPOLLERR);
			::close(m_sockfd);
			//connect();//重连
		}
		if (m_status == connected)
		{
			close();
			//connect();
			m_status=disconnected;
		}
	}

	bool TcpClient::close()
	{
		if (m_status==disconnected)
		{
			return false;
		}
		else
		{
			m_conn->close();
			m_conn=nullptr;
			m_status=disconnected;
			return true;
		}
	}


	void TcpClient::setMessageCallback(function<void (shared_ptr<TcpConn>,Buffer*)> cb)
	{
		m_messageCallback=cb;
	}
	void TcpClient::setConnCallback(function<void (shared_ptr<TcpConn>)> cb)
	{
		m_connCallback=cb;
	}
	void TcpClient::setCloseCallback(function<void (shared_ptr<TcpConn>)> cb)
	{
		m_closeCallback=cb;
	}
	void TcpClient::setErrCallback(function<void (shared_ptr<TcpConn>)> cb)
	{
		m_errCallback=cb;
	}

}
