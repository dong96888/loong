#include "Lock.h"

namespace loong
{
	Lock::Lock()
	{
		pthread_mutex_init(&m_mtx, nullptr);
	}
	Lock::~Lock()
	{
		pthread_mutex_destroy(&m_mtx);
	}
	int Lock::lock()
	{
		return pthread_mutex_lock(&m_mtx);
	}
	int Lock::unlock()
	{
		return pthread_mutex_unlock(&m_mtx);
	}
	int Lock::trylock()
	{
		return pthread_mutex_trylock(&m_mtx);
	}
}

