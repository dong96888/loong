/*
*   File name: Acceptor.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_


#include "Channel.h"

#include <netinet/in.h>

class EventLoop;
namespace loong
{
	class Address;
	class EventLoop;
	class Acceptor
	{
	public:
		Acceptor(EventLoop *loop);
		~Acceptor();

		int listen(const Address& addr);
		int accept(sockaddr_in * addr);
		void start();
		void setNewConnCallback(function<void()> cb);

	private:
		function<void()> m_newConnCallback;
		int m_sockfd;
		int m_dummyFd;
		Channel m_chan;
		EventLoop *m_loop;
	};
}
#endif

