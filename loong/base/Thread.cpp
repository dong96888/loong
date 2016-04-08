#include "Thread.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>


namespace loong
{
	Thread::Thread()
	{
		m_tid = 0;
	}

	Thread::~Thread()
	{
	}

	void Thread::start()
	{
		if (pthread_create(&m_tid, nullptr, threadfunc, this) != 0)
		{
			printf("create thread error: %s\n", strerror(errno));
		}
		else
		{
			m_running = true;
		}
	}

	void Thread::stop()
	{
		if (m_running)
		{
			m_running = false;
		}
	}

	bool Thread::isRunning()
	{
		return m_running;
	}

	void *Thread::threadfunc(void *arg)
	{
		Thread *thread = static_cast<Thread *>(arg);
		thread->run();
		return thread;
	}

	void Thread::join()
	{
		pthread_join(m_tid, nullptr);
	}

	pthread_t Thread::getTid() const
	{
		return m_tid;
	}
}