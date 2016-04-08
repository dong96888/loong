/*
*   File name: RingBuff.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#include "ThreadPool.h"

#include "Logger.h"
#include <unistd.h>
#include <thread>

using namespace std;

namespace loong
{
	ThreadPool::ThreadPool(uint16_t threadNum)
	:m_threadNum(threadNum),m_loopIdx(0)
	{
	}

	ThreadPool::~ThreadPool()
	{
		stop();
	}

	EventLoop *ThreadPool::getEventLoop()
	{
		EventLoop *loop = m_loops[m_loopIdx];
		++m_loopIdx;
		if (m_loopIdx>=m_loops.size())
		{
			m_loopIdx=0;
		}
		return loop;
	}

	void ThreadPool::start()
	{
		//创建工作线程池
		for (int i = 0; i < m_threadNum; i++)
		{
			EventLoop *loop = new EventLoop();
			thread *t=new thread(&EventLoop::loop,loop);
			m_threads.push_back(t);
			m_loops.push_back(loop);
		}
		printf("thread pool start success!\n");
	}

	void ThreadPool::stop()
	{
		for (auto it=m_loops.begin(); it != m_loops.end(); ++it)
		{
			(*it)->quit();
		}

		for (auto it=m_threads.begin(); it != m_threads.end(); ++it)
		{
			(*it)->join();
			delete *it;
		}

		for (auto it=m_loops.begin(); it != m_loops.end(); ++it)
		{
			delete *it;
		}
	}


}
