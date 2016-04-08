#include "Timer.h"
#include "TimerManager.h"


namespace loong
{
	Timer::Timer(long interval, function<void ()> func, TimerType type)
	{
		m_interval = interval;
		handleTimeout = func;
		m_type = type;
		m_next = nullptr;
		m_timeout = 0;
	}

	void Timer::start()
	{
		TimerManager::instance().addTimer(this);
	}

	void Timer::stop()
	{
		TimerManager::instance().delTimer(this);
	}
}

