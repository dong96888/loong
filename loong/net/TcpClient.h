/*
*   File name: Address.cpp
*  Describe: 工作线程池类，创建数个工作线程
*  Created on: 2015-6-12
*      Author: dengxiudong
*/

#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "TcpConn.h"
#include "Thread.h"
#include "Buffer.h"

#include <functional>
#include <thread>

using namespace std;

namespace loong
{
	class Channel;
	class EventLoop;
	class TcpConn;
	class TcpClient
	{
	public:
		TcpClient(EventLoop *loop);
		~TcpClient();

		void setMessageCallback(function<void (shared_ptr<TcpConn>,Buffer*)> cb);
		void setConnCallback(function<void (shared_ptr<TcpConn>)> cb);
		void setCloseCallback(function<void (shared_ptr<TcpConn>)> cb);
		void setErrCallback(function<void (shared_ptr<TcpConn>)> cb);

		void connect(string ip, uint16_t port);
		void handleConn();
		void handleError();
		bool sendData(const void *data,size_t len);
		bool close();
		void onNewConn();
		void start();

	private:
		function<void (shared_ptr<TcpConn>,Buffer*)> m_messageCallback;
		function<void (shared_ptr<TcpConn>)> m_connCallback;
		function<void (shared_ptr<TcpConn>)> m_closeCallback;
		function<void (shared_ptr<TcpConn>)> m_errCallback;


	private:
		enum Status { disconnected, connecting, connected};

		string m_ip;
		uint16_t m_port;

		int m_sockfd;
		Status m_status;
		Channel *m_chan;
		shared_ptr<TcpConn> m_conn;
		EventLoop *m_loop;
		thread *m_th;
	};
}

#endif
