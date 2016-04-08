/*
*   File name: TcpServer.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "Address.h"
#include "ThreadPool.h"
#include "Acceptor.h"
#include "Socket.h"
#include "Logger.h"
#include "TcpConn.h"
#include "Channel.h"


#include <string>
#include <functional>

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

namespace loong
{
	class TcpServer
	{
	public:
		TcpServer(string ip, uint16_t port);
		TcpServer(uint16_t port);
		virtual ~TcpServer();

		bool start(int threadNum = 0);

		void setCloseCallback(function<void(shared_ptr<TcpConn>)> cb);
		void setConnCallback(function<void(shared_ptr<TcpConn>)> cb);
        void setErrCallback(function<void(shared_ptr<TcpConn>)> cb);
        void setMessageCallback(function<void(shared_ptr<TcpConn>,Buffer *)> cb);

	private:
		bool listen();
		void onNewConn();

	private:
		uint16_t m_threadNum;
		Address m_addr;
		EventLoop m_loop;
		Acceptor m_acceptor;
		ThreadPool *m_threadPool;

		function<void(shared_ptr<TcpConn>)> m_connCallback;
		function<void(shared_ptr<TcpConn>)> m_closeCallback;
        function<void(shared_ptr<TcpConn>,Buffer *)> m_messageCallback;
        function<void(shared_ptr<TcpConn>)> m_errCallback;

	private:
		//服务器监听socket描述符
		int m_sockfd;

	};
}

#endif
