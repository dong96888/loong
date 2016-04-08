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

		//����Ϊ������
		Socket::setNonBlock(m_sockfd);

		int keepAlive = 1; // ����keepalive����
		int keepIdle = 60; // ���������60����û���κ���������,�����̽��
		int keepInterval = 5; // ̽��ʱ������ʱ����Ϊ5 ��
		int keepCount = 3; // ̽�Ⳣ�ԵĴ���.�����1��̽������յ���Ӧ��,���2�εĲ��ٷ�.

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
		//ɾ��ԭ��������ӵļ����¼�
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
			//connect();//����
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
