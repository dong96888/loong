/*************************************************************************
*  File Name: UdpClient.cpp
*     Author: dengxiudong
*       Mail: 
* Created on: 2015年09月22日 星期二 11时08分49秒
*   Describe: 
*
**************************************************************************/
#include "UdpClient.h"
#include "Logger.h"

namespace loong
{
	UdpClient::UdpClient(string svrIp, uint16_t svrPort)
	{
		m_svrIp = svrIp;
		m_svrPort = svrPort;
	}

	UdpClient::~UdpClient()
	{
		close(m_sockfd);
	}

	bool UdpClient::openClient()
	{
		m_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
		if (m_sockfd < 0)
		{
			ERROR("error: Create Socket Failed!");
			return false;
		}

		m_sockAddr.sin_family = AF_INET;
		m_sockAddr.sin_port = htons(m_svrPort);
		inet_aton(m_svrIp.c_str(), &m_sockAddr.sin_addr);

		memset(m_sockAddr.sin_zero, 0x00, 8);
		return true;
	}

	bool UdpClient::sendData(const char* data, uint32_t dataSize)
	{
		int toslen = 0;
		uint32_t tolen = sizeof(struct sockaddr);
		if ((toslen = sendto(m_sockfd, data, dataSize, 0, (struct sockaddr *)&m_sockAddr, tolen)) == -1)
		{
			ERROR("error: Sending Failed! %s\n", strerror(errno));
			return false;
		}
		return true;
	}
}


