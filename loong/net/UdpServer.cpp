/*************************************************************************
*  File Name: UdpServer.cpp
*     Author: dengxiudong
*       Mail:
* Created on: 2015年09月16日 星期三 16时33分42秒
*   Describe:
*
**************************************************************************/
#include "UdpServer.h"
#include "Logger.h"
#include "Socket.h"
#include <netinet/in.h>    
#include <sys/types.h>    
#include <sys/socket.h>   
#include <unistd.h>    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

namespace loong
{
#define RECV_BUFF_SIZE 256
	UdpServer::UdpServer(uint16_t port)
	{
		m_port = port;
	}
	UdpServer::~UdpServer()
	{
	}

	void UdpServer::setMsgCallback(function<void(struct sockaddr_in*, char *, size_t)> cb)
	{
		msgCallback = cb;
	}

	void UdpServer::run()
	{
		if (!openServer())
		{
			ERROR("start udp server failed!");
		}

		try
		{
			char recvs[RECV_BUFF_SIZE];
			int recvslen;
			struct sockaddr_in from;
			uint32_t fromlen;
			while (!isRunning())
			{
				memset(recvs, 0, sizeof(recvs));
				INFO("Waiting for data...\n");
				fromlen = sizeof(struct sockaddr);
				if ((recvslen = recvfrom(m_sockfd, recvs, RECV_BUFF_SIZE, 0, (struct sockaddr *)&from, &fromlen)) == -1)
				{
					ERROR("Receiving Failed!");
				}
				else
				{
					if (recvslen < RECV_BUFF_SIZE)
					{
						*(recvs + recvslen) = '\0';
					}
					else
					{
						*(recvs + RECV_BUFF_SIZE - 1) = '\0';
					}
					DEBUG("Received data:\n[%s]\n", recvs);
					msgCallback(&from, recvs, recvslen);
				}
			}
		}
		catch (...)
		{
			ERROR("error catched~");
		}

		close(m_sockfd);
	}

	bool UdpServer::openServer()
	{
		int m_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
		if (m_sockfd < 0)
		{
			ERROR("Create Socket Failed!");
			return false;
		}

		if (Socket::setReuseable(m_sockfd, true) != 0)
		{
			ERROR("Set port reuse error!");
			return false;
		}

		struct sockaddr_in svrAddr;
		svrAddr.sin_family = AF_INET;
		svrAddr.sin_port = htons(m_port);
		svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		memset(svrAddr.sin_zero, 0x00, 8);

		if (bind(m_sockfd, (struct sockaddr *)&svrAddr, sizeof(struct sockaddr)) != 0)
		{
			ERROR("Bind Socket Failed:%s", strerror(errno));
			return false;
		}

		return true;
	}

	size_t UdpServer::sendData(struct sockaddr_in *sockAddr, const char* data, uint32_t dataSize)
	{
		memset(sockAddr->sin_zero, 0x00, 8);
		int toslen = 0;
		try
		{
			char *tos = (char*)&data;
			uint32_t tolen = sizeof(struct sockaddr);
			if ((toslen = sendto(m_sockfd, tos, dataSize, 0, (struct sockaddr *)&sockAddr, tolen)) == -1)
			{
				ERROR("Send data error:%s", strerror(errno));
			}
		}
		catch (...)
		{
			ERROR("Catch exception");
		}
		return toslen;
	}
}

