/*************************************************************************
*  File Name: UdpClient.h
*     Author: dengxiudong
*       Mail: 
* Created on: 2015年09月22日 星期二 11时08分57秒
*   Describe: 
*
**************************************************************************/
#ifndef _UDPCLIENT_H_
#define _UDPCLIENT_H_

#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <unistd.h>    
#include <stdio.h>      
#include <stdlib.h>       
#include <string.h>  
#include <stdint.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
using namespace std;

namespace loong
{
	class UdpClient
	{
	public:
		UdpClient(string svrIp, uint16_t svrPort);
		~UdpClient();

		bool openClient();
		bool sendData(const char* data, uint32_t dataSize);
	private:
		string m_svrIp;
		uint16_t m_svrPort;
		int m_sockfd;
		struct sockaddr_in m_sockAddr;
	};
}

#endif
