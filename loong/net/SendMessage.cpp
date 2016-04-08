#include "SendMessage.h"

#include "BufferPool.h"
#include "TcpConn.h"

#include <netinet/in.h>
#include <string.h>

using namespace std;

namespace loong
{
	Cond SendMessage::m_cond;
	deque<TcpConn *> SendMessage::m_sendSockQue;
	deque<TcpConn *> SendMessage::m_tmpSockQue;

	SendMessage::SendMessage()
	{
		m_tcpConn = nullptr;
	}


	SendMessage::~SendMessage()
	{
	}


	void SendMessage::pushSock(TcpConn *conn)
	{
		m_cond.lock();
		m_sendSockQue.push_back(conn);
		m_cond.signal();
	}

	void SendMessage::run()
	{
		while (true)
		{
			m_cond.lock();
			while (m_sendSockQue.empty())
			{
				m_cond.wait();
			}
			//����������к���ʱ���е�����
			m_sendSockQue.swap(m_tmpSockQue);
			m_sendSockQue.clear();
			m_cond.unlock();

			while (!m_tmpSockQue.empty())
			{
				m_tcpConn = m_tmpSockQue.front();
				m_tmpSockQue.pop_front();

				if (m_tcpConn->m_sendBuf.size == 0)
				{
					continue;
				}
				//������ͻ�����������û�з��ͣ���ִ�з��Ͳ���
				else
				{
					m_tcpConn->sendMsg();
				}
			}
		}
	}
}
