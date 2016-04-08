#include "TcpServer.h"

#include <string.h>
#include <unistd.h>
#include <memory>

#include <sys/epoll.h>
#include <netinet/tcp.h>

namespace loong
{

	TcpServer::TcpServer(string ip, uint16_t port)
	:m_addr(ip, port),m_loop(),m_acceptor(&m_loop),m_threadNum(0)
	{
	}


	TcpServer::TcpServer(uint16_t port)
	:m_addr(port),m_loop(),m_acceptor(&m_loop),m_threadNum(0),m_threadPool(nullptr)
	{
	}


	TcpServer::~TcpServer()
	{
		if (m_sockfd > 0)
		{
			close(m_sockfd);
		}
	}


	bool TcpServer::start(int threadNum)
	{
		if (threadNum != 0)
		{
			m_threadNum = threadNum;
			//创建线程池
			m_threadPool = new ThreadPool(threadNum);
			if (m_threadPool == nullptr)
			{
				printf("create thread pool failed!\n");
				return false;
			}
			m_threadPool->start();
			DEBUG("Start thread pool");
		}

		//打开服务器监听，循环等待事件触发
		if (!listen())
		{
			printf("listen socket error\n");
			return false;
		}

		printf("TcpServer success start!\n");
		m_loop.loop();
		return true;
	}

	void TcpServer::setConnCallback(function<void(shared_ptr<TcpConn>)> cb)
	{
		m_connCallback = cb;
	}

	void TcpServer::setCloseCallback(function<void(shared_ptr<TcpConn>)> cb)
	{
		m_closeCallback = cb;
	}

	void TcpServer::setErrCallback(function<void(shared_ptr<TcpConn>)> cb)
	{
		m_errCallback = cb;
	}

	void TcpServer::setMessageCallback(function<void(shared_ptr<TcpConn>,Buffer*)> cb)
	{
		m_messageCallback = cb;
	}

	bool TcpServer::listen()
	{
		//监听端口
		m_acceptor.setNewConnCallback(std::bind(&TcpServer::onNewConn,this));

		m_sockfd = m_acceptor.listen(m_addr);
		if (m_sockfd < 0)
		{
			ERROR("listen error %s", strerror(errno));
			return false;
		}

		m_acceptor.start();
		DEBUG("start server listen");
		return true;
	}

	void TcpServer::onNewConn()
	{
		//printf("have new client connect!\n");
		int connfd = 0;
		struct sockaddr_in addr;
		//循环接受连接
		while ((connfd = m_acceptor.accept(&addr)) > 0)
		{
			//printf("begin accept client connect!\n");
			INFO("accept one client connect!");
			Socket::setNonBlock(connfd);

			int keepAlive = 1; // 开启keepalive属性
			int keepIdle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测
			int keepInterval = 5; // 探测时发包的时间间隔为5 秒
			int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

			setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
			setsockopt(connfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
			setsockopt(connfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
			setsockopt(connfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));

			EventLoop *loop=nullptr;
			if (m_threadNum==0)
			{
				loop=&m_loop;
			}
			else
			{
				loop=m_threadPool->getEventLoop();
			}

			shared_ptr<TcpConn> conn(new TcpConn(loop,connfd));
			//printf("create on tcpconn object!\n");
			conn->setMessageCallback(m_messageCallback);
			conn->setErrCallback(m_errCallback);
			conn->setCloseCallback(m_closeCallback);

			if (m_connCallback != nullptr)
			{
				m_connCallback(conn);
			}

			conn->onEstablished();
		}
	}
}


