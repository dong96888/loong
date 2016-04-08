/*
*   File name: Socket.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _SOCK_TOOL_H_
#define _SOCK_TOOL_H_

#include <string>
#include <stdint.h>
#include <unistd.h>

namespace loong
{
	class Address;
	class Socket
	{
	public:
		static int createSocket();
		static int bindAndListen(const Address& addr);

	public:
		static int connect(int sockfd, const Address& addr);

	public:
		static int setNonBlock(int sockfd);
		static int setCloseExec(int sockfd);

		static int setNoDelay(int sockfd, bool on);

		static int setReuseable(int sockfd, bool on);
		static int setKeepAlive(int sockfd, bool on);

		static int getLocalAddr(int sockfd, Address& addr);
		static int getRemoteAddr(int sockfd, Address& addr);

		static int getSockError(int sockfd);

		static unsigned int getHostByName(const std::string& host);
	};
}

#endif

