/*
*   File name: Address.cpp
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _ADRESS_H_
#define _ADRESS_H_

#include <string>
#include <stdint.h>
#include <netinet/in.h>

namespace loong
{
	class Address
	{
	public:
		Address(unsigned short port);
		Address(const std::string& ip, unsigned short port);
		Address(const struct sockaddr_in& addr);

		//host byte order
		uint32_t ip() const;

		//host byte order
		unsigned short port() const;

		const struct sockaddr_in& sockAddr() const { return m_addr; }

		std::string ipstr() const;

	private:
		struct sockaddr_in m_addr;
	};

}
#endif


