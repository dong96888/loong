/*************************************************************************
*  File Name: UdpServer.h
*     Author: dengxiudong
*       Mail:
* Created on: 2015年09月16日 星期三 16时33分57秒
*   Describe:
*
**************************************************************************/
#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

#include "Thread.h"
#include <string>
#include <functional>
#include <stdint.h>
#include <netinet/in.h>

using namespace std;

namespace loong
{
	struct RecvData
	{
		uint16_t port;
		uint32_t connNum;
	};

	class UdpServer :public Thread
	{
	public:
		UdpServer(uint16_t port);
		~UdpServer();

		size_t sendData(struct sockaddr_in *sockAddr, const char * data, uint32_t dataSize);
		void setMsgCallback(function<void(struct sockaddr_in*, char *, size_t)> cb);

	protected:
		void run();

	private:
		bool openServer();
		function<void(struct sockaddr_in*, char *, size_t)> msgCallback;

		uint16_t m_port;
		int m_sockfd;
	};
}

#endif
