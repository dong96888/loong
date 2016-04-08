#include "Acceptor.h"

#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <memory>
#include <functional>

#include "Address.h"
#include "Socket.h"
#include "Logger.h"
#include "EventLoop.h"

using namespace std;


namespace loong
{
	int createDummyFd()
	{
		return open("/dev/nullptr", O_RDONLY | O_CLOEXEC);
	}

	Acceptor::Acceptor(EventLoop *loop)
		: m_sockfd(0)
		, m_dummyFd(createDummyFd()),m_loop(loop)
	{

	}

	Acceptor::~Acceptor()
	{
		if (m_sockfd > 0)
		{
			close(m_sockfd);
		}
		close(m_dummyFd);
	}

	int Acceptor::listen(const Address& addr)
	{
		int fd = Socket::bindAndListen(addr);
		if (fd < 0)
		{
			return fd;
		}

		m_sockfd = fd;

		return m_sockfd;
	}

	void Acceptor::setNewConnCallback(function<void()> cb)
	{
        m_newConnCallback=cb;
	}

	void Acceptor::start()
	{
        m_chan.setSockFd(m_sockfd);
        m_chan.setReadCallback(m_newConnCallback);
        m_loop->addEvent(&m_chan,EPOLLET|EPOLLIN);
	}

	int Acceptor::accept(struct sockaddr_in* addr)
	{
		socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);

#if VALGRIND
		int connfd = accept(m_sockfd, sockaddr_cast(addr), &addrlen);
		Socket::setNonBlock(connfd);
		Socket::setCloseExec(connfd);
#else
		int connfd = accept4(m_sockfd, (struct sockaddr*)(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
		if (connfd < 0)
		{
			int err = errno;
			if (err == EMFILE || err == ENFILE)
			{
				ERROR("accept error %s", strerror(err));
				close(m_dummyFd);
				connfd = ::accept(m_sockfd, nullptr, nullptr);
				close(connfd);

				m_dummyFd = createDummyFd();
			}
			return -1;
		}
		else
		{
			INFO("onAccept %d", connfd);

			//设置TCP缓冲数据立即发送
			Socket::setNoDelay(connfd, true);

			return connfd;
		}
	}
}
