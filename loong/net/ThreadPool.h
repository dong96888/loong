/*
*   File name: Address.cpp
*	 Describe: 工作线程池类，创建数个工作线程
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "EventLoop.h"
#include "Lock.h"
#include "Cond.h"

#include <deque>
#include <vector>
#include <thread>

#include <stdint.h>


using namespace std;

namespace loong
{
	class ThreadPool
	{
	public:
		ThreadPool(uint16_t threadNum = 6);
		~ThreadPool();

		void start();
		void stop();
		EventLoop *getEventLoop();


	private:
		//线程的数目
		uint16_t m_threadNum;
		uint32_t m_loopIdx;

		//用来存储线程
		vector<thread*> m_threads;
		vector<EventLoop*> m_loops;

	};
}
#endif /* THREADPOOL_H_ */
