/*
*   File name: MutexLock.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _LOCK_H_
#define _LOCK_H_

#include "pthread.h"

namespace loong
{
	class Lock
	{
	public:
		Lock();
		~Lock();
		int lock();
		int unlock();
		int trylock();

	private:
		pthread_mutex_t m_mtx;
	};

	class SmartLock
	{
	public:
		SmartLock(Lock &lock) :m_lock(lock)
		{
			m_lock.lock();
		}

		~SmartLock()
		{
			m_lock.unlock();
		}
	private:
		Lock &m_lock;
	};
}



#endif