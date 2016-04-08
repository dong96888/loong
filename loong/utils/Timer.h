#ifndef _TIMER_H_
#define _TIMER_H_


#include <sys/time.h>
#include <functional>

using namespace std;

namespace loong
{
	class Timer
	{
		friend class TimerManager;
	public:
		enum TimerType { TIMER_ONCE, TIMER_CIRC };

		Timer(long interval, function<void ()> func, TimerType type);
		void start();
		void stop();

	private:
		Timer *m_next;

		function<void ()> handleTimeout;

		TimerType m_type;

		long m_interval;
		long m_timeout;
	};
}


#endif



