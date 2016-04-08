/*
*   File name: WorkThread.h
*	 Describe: 工作线程，接收到消息解包后分配给具体的业务线程处理
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_

#include "Thread.h"
#include "BaseService.h"
#include "Lock.h"
#include "TcpConn.h"

#include <pthread.h>
#include <map>

#include <stdint.h>


using namespace std;

namespace loong
{
	//extern unordered_map<int, shard_ptr<TcpConn*>> g_tcpConnMap;
	class ThreadPool;
	class WorkThread : public Thread
	{
	public:
		WorkThread();
		~WorkThread();
		void run();

		//实际的工作函数，由run来调用
		void execTask(TcpConn * task);
		//注册线程池
		void registerThreadPool(ThreadPool *threadPool);

	private:
		//线程池的指针，用来指定线程去哪里获取任务
		ThreadPool *m_threadPool;
	};
}

#endif /* WORKTHREAD_H_ */
