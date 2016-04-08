#include "Address.h"

#include <string.h>
#include <arpa/inet.h>

#include "Socket.h"
#include "Logger.h"
#include "BaseType.h"

using namespace std;

namespace loong
{
	Address::Address(unsigned short port)
	{
		bzero(&m_addr, sizeof(m_addr));

		m_addr.sin_family = AF_INET;
		m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		m_addr.sin_port = htons(port);
	}

	Address::Address(const string& ip, unsigned short port)
	{
		bzero(&m_addr, sizeof(m_addr));

		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(port);

		if (inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) <= 0)
		{
			//may be domain
			unsigned int ret = Socket::getHostByName(ip);
			if (ret != uint32_t(-1))
			{
				m_addr.sin_addr.s_addr = ret;
				return;
			}

			ERROR("invalid ip %s, use 0.0.0.0 instead", ip.c_str());
			//error, may log later, now use INADDR_ANY
			m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
	}

	Address::Address(const struct sockaddr_in& addr)
	{
		memcpy(&m_addr, &addr, sizeof(addr));
	}

	unsigned int Address::ip() const
	{
		return ntohl(m_addr.sin_addr.s_addr);
	}

	string Address::ipstr() const
	{
		char buf[32] = { '\0' };
		inet_ntop(AF_INET, &m_addr.sin_addr, buf, static_cast<socklen_t> (sizeof(buf)));

		return string(buf, 32);
	}

	unsigned short Address::port() const
	{
		return ntohs(m_addr.sin_port);
	}
}
