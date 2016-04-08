/*
*   File name: Thread.h
*	 Describe: �̴߳����࣬���Ҫ����һ���̣߳��̳�����࣬ʵ��run�ӿھͿ���
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

namespace loong
{
	class Thread
	{
	public:
		Thread();
		virtual ~Thread();
		void start();
		virtual void stop();
		virtual bool isRunning();
		virtual void join();
		pthread_t getTid() const;

	protected:
		virtual void run() = 0;
		pthread_t m_tid;

	private:
		static void *threadfunc(void *arg);
		bool m_running;

	};
}

#endif
