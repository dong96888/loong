/*
*   File name: TcpConn.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _TCP_CONN_
#define _TCP_CONN_

#include "Lock.h"
#include "Buffer.h"
#include "EventLoop.h"
#include <stdint.h>
#include <functional>
#include <memory>

//#include <tr1/unordered_map.h>

using namespace std;

namespace loong
{
	/*
#pragma  pack(1)
	struct MsgHead
	{
		//消息总长度：消息头长度+消息体长度
		uint16_t len;
		//消息编号
		uint16_t cmd;
	};
#pragma  pack()

#define MsgHeadSize sizeof(MsgHead)

	struct Buffer
	{
		uint32_t capacity;
		uint32_t size;

		char *buf;
		Lock lock;
		Buffer()
		{
			capacity = 65535;
			size = 0;
			buf = nullptr;
		}
	};
*/

	class Channel;
	class TcpConn:public enable_shared_from_this<TcpConn>
	{
	public:
		TcpConn(EventLoop *loop,int sockfd);
		virtual ~TcpConn();

		int getSocket();

		void handleRead();
		void handleWrite();
		void handleClose();
		void handleError();

		void sendData(const void *data, size_t len);

		void setMessageCallback(function<void(shared_ptr<TcpConn>, Buffer*)> cb)
		{
			m_messageCallback = cb;
		}

		void setCloseCallback(function<void(shared_ptr<TcpConn>)> cb)
		{
			m_closeCallback = cb;
		}

		void setErrCallback(function<void(shared_ptr<TcpConn>)> cb)
		{
			m_errCallback = cb;
		}

		void onEstablished();
		void close();

	private:
		function<void(shared_ptr<TcpConn>,Buffer *)> m_messageCallback;
		function<void(shared_ptr<TcpConn>)> m_closeCallback;
		function<void(shared_ptr<TcpConn>)> m_errCallback;

	public:
		Buffer m_recvBuf;
		Buffer m_sendBuf;
	private:
		int m_sockfd;
		EventLoop *m_loop;
		Channel *m_chan;
	};
}
//typedef unordered_map<int, TcpConn*> ConnMap_t;

#endif
