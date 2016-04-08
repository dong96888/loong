#include "Socket.h"

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <assert.h>
#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "Address.h"
#include "Logger.h"

using namespace std;

namespace loong
{
	int Socket::createSocket()
	{
		return socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	}

	int Socket::bindAndListen(const Address& addr)
	{
		int err = 0;

		int fd = createSocket();
		if (fd < 0)
		{
			err = errno;
			ERROR("create socket error %s", strerror(err));
			return fd;
		}

		setReuseable(fd, true);

		do
		{
			struct sockaddr_in sockAddr = addr.sockAddr();

			if (bind(fd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
			{
				err = errno;
				ERROR("bind address %s:%d error: %s", addr.ipstr().c_str(), addr.port(), strerror(err));
				break;
			}

			if (listen(fd, SOMAXCONN) < 0)
			{
				err = errno;
				ERROR("listen address %s:%d error: %s", addr.ipstr().c_str(), addr.port(), strerror(err));
				break;
			}

			return fd;
		} while (0);

		close(fd);
		return err;
	}

	int Socket::connect(int sockfd, const Address& addr)
	{
		struct sockaddr_in sockAddr = addr.sockAddr();

		int ret = ::connect(sockfd, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
		if (ret < 0)
		{
			int err = errno;
			return err;
		}
		else
		{
			Socket::setNoDelay(sockfd, true);
			return ret;
		}
	}


	int Socket::setNonBlock(int sockfd)
	{
		int flags;
		int s;
		flags = fcntl(sockfd, F_GETFL, 0);
		if (flags == -1)
		{
			ERROR("setNonBlock error %s", strerror(errno));
			return -1;
		}

		flags |= O_NONBLOCK;
		s = fcntl(sockfd, F_SETFL, flags);
		if (s == -1)
		{
			ERROR("setNonBlock error %s", strerror(errno));
			return -1;
		}
		return 0;
	}

	int Socket::setCloseExec(int sockfd)
	{
		int flags = fcntl(sockfd, F_GETFD, 0);
		flags |= FD_CLOEXEC;
		if (fcntl(sockfd, F_SETFD, flags) < 0)
		{
			return -1;
		}
		return 0;
	}

	int Socket::setNoDelay(int sockfd, bool on)
	{
		int opt = on ? 1 : 0;

		return setsockopt(sockfd, IPPROTO_TCP,
			TCP_NODELAY, &opt,
			static_cast<socklen_t>(sizeof(opt)));
	}

	int Socket::setReuseable(int sockfd, bool on)
	{
		int opt = on ? 1 : 0;
		return setsockopt(sockfd, SOL_SOCKET,
			SO_REUSEADDR, &opt,
			static_cast<socklen_t>(sizeof(opt)));
	}

	int Socket::setKeepAlive(int sockfd, bool on)
	{
		int opt = on ? 1 : 0;
		return setsockopt(sockfd, SOL_SOCKET,
			SO_KEEPALIVE, &opt,
			static_cast<socklen_t>(sizeof(opt)));
	}

	int Socket::getLocalAddr(int sockfd, Address& addr)
	{
		struct sockaddr_in sockAddr;
		socklen_t sockLen = sizeof(sockAddr);
		if (getsockname(sockfd, (struct sockaddr*)&sockAddr, &sockLen) != 0)
		{
			int err = errno;
			return err;
		}

		addr = Address(sockAddr);
		return 0;
	}

	int Socket::getRemoteAddr(int sockfd, Address& addr)
	{
		struct sockaddr_in sockAddr;
		socklen_t sockLen = sizeof(sockAddr);
		if (getpeername(sockfd, (struct sockaddr*)&sockAddr, &sockLen) != 0)
		{
			int err = errno;
			return err;
		}

		addr = Address(sockAddr);
		return 0;
	}

	int Socket::getSockError(int sockfd)
	{
		int opt;
		socklen_t optLen = static_cast<socklen_t>(sizeof(opt));

		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &opt, &optLen) < 0)
		{
			int err = errno;
			return err;
		}
		else
		{
			return opt;
		}
	}


	unsigned int Socket::getHostByName(const string& host)
	{
		struct addrinfo hint;
		struct addrinfo *answer;

		memset(&hint, 0, sizeof(hint));
		hint.ai_family = AF_INET;
		hint.ai_socktype = SOCK_STREAM;

		int ret = getaddrinfo(host.c_str(), nullptr, &hint, &answer);
		if (ret != 0)
		{
			ERROR("getaddrinfo error %s", strerror(errno));
			return uint32_t(-1);
		}

		//we only use first find addr
		for (struct addrinfo* cur = answer; cur != nullptr; cur = cur->ai_next)
		{
			return ((struct sockaddr_in*)(cur->ai_addr))->sin_addr.s_addr;
		}

		ERROR("getHostByName Error");
		return uint32_t(-1);
	}
}
