/*
*   File name: RecvMessage.h
*	 Describe: �����̣߳�ר�Ŵ�����д�¼�������ʱ�򣬷��ͻ�����û�з��͵�����
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _SEND_MESSAGE_H_
#define _SEND_MESSAGE_H_

#include "Thread.h"
#include "WorkThread.h"
#include "Cond.h"
#include "Socket.h"
#include "AsynLog.h"

#include <deque>
#include <stdint.h>

using namespace std;

namespace loong
{
	class SendMessage :
		public Thread
	{
	public:
		SendMessage();
		~SendMessage();

		static void pushSock(TcpConn *m_tcpConn);

		virtual void run();

	private:
		static deque<TcpConn *> m_sendSockQue;
		//��������
		static deque<TcpConn *> m_tmpSockQue;

		static Cond m_cond;
		TcpConn *m_tcpConn;
	};
}

#endif

