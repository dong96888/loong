#include "Cond.h"

#include <errno.h>
#include <time.h>

namespace loong
{
	bool Cond::waitForSeconds(int seconds)
	{
		struct timespec abstime;
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += seconds;
		return ETIMEDOUT == pthread_cond_timedwait(&m_cond, &m_mutex, &abstime);
	}
}
