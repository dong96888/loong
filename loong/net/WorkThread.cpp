#include "WorkThread.h"

#include "ThreadPool.h"
#include "ServiceFactoryBase.h"
#include "Socket.h"
#include "BufferPool.h"
#include "AsynLog.h"
#include "TcpConn.h"

#include <iostream>
#include <map>

#include <string.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <stdio.h>



using namespace std;

namespace loong
{
	//unordered_map<int, TcpConn*> g_tcpConnMap;
	WorkThread::WorkThread()
	{
	}

	WorkThread::~WorkThread()
	{
	}

	void WorkThread::run()
	{
		DEBUG("begin workthread run");
		deque<TcpConn*> localTaskQue;
		TcpConn *connTask = nullptr;

		while (true)
		{
			//交换共享队列到临时任务队列，减少线程切换次数	
			m_threadPool->getTaskQue(localTaskQue);
			INFO("Get task queue success!");

			while (!localTaskQue.empty())
			{
				connTask = localTaskQue.front();
				execTask(connTask);
				localTaskQue.pop_front();
				DEBUG("work thread procing");
			}
			localTaskQue.clear();
			DEBUG("end enter work thread proc queue");
		}
		DEBUG("end workthread run");
	}


	void WorkThread::execTask(TcpConn *task)
	{
		task->onRecv();
	}

	void WorkThread::registerThreadPool(ThreadPool *threadPool)
	{
		m_threadPool = threadPool;
	}
}
