/*
*   File name: Cond.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef __Cond__
#define __Cond__

#include <pthread.h>

namespace loong
{
	class Cond
	{
	public:
		explicit Cond()
		{
			pthread_mutex_init(&m_mutex, nullptr);
			pthread_cond_init(&m_cond, nullptr);
		}

		~Cond()
		{
			pthread_mutex_destroy(&m_mutex);
			pthread_cond_destroy(&m_cond);
		}

		void lock()
		{
			pthread_mutex_lock(&m_mutex);
		}

		void unlock()
		{
			pthread_mutex_unlock(&m_mutex);
		}

		void wait()
		{
			pthread_cond_wait(&m_cond, &m_mutex);
		}

		// returns true if time out, false otherwise.
		bool waitForSeconds(int seconds);

		void signal()
		{
			pthread_cond_signal(&m_cond);
			pthread_mutex_unlock(&m_mutex);
		}

		void broadcast()
		{
			pthread_cond_broadcast(&m_cond);
		}

	private:
		pthread_mutex_t m_mutex;
		pthread_cond_t m_cond;
	};
}
#endif  //
