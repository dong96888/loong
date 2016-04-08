/*
*   File name: ReadWriteLock.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef __ReadWriteLock__
#define __ReadWriteLock__

#include <pthread.h>

namespace loong
{
	class ReadWriteLock
	{
	public:
		ReadWriteLock()
		{
			pthread_rwlock_init(&m_rwlock, nullptr);
		}
		~ReadWriteLock()
		{
			if (m_islock == 1)
			{
				pthread_rwlock_unlock(&m_rwlock)
			}
			pthread_rwlock_destroy(&m_rwlock);
		}
		int readlock()
		{
			if (pthread_rwlock_rdlock(&m_rwlock) < 0)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}

		int writelock()
		{
			if (pthread_rwlock_wrlock(&m_rwlock) < 0)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}

		void unlock()
		{
			pthread_rwlock_unlock(&m_rwlock);
		}

	private:
		pthread_rwlock_t m_rwlock;
	};
}

#endif

