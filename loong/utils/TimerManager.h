#ifndef _TIMER_MANAGER_H_
#define _TIMER_MANAGER_H_

#include "Timer.h"
#include "Thread.h"


namespace loong
{
	class TimerManager:Thread
	{
	public:
		typedef enum
		{
			TIMER_MANAGER_STOP = 0,
			TIMER_MANAGER_START
		}ManagerState;

		static TimerManager &instance();
		void run();
		void addTimer(Timer *timer);
		void delTimer(Timer *timer);
	public:
		ManagerState  m_state;
	private:
		Timer *m_headTimer;
		static TimerManager *m_instance;
		static pthread_mutex_t m_mutex;
		pthread_mutex_t m_lstMutex;
		TimerManager()
		{
			m_headTimer = nullptr;
			m_state == TIMER_MANAGER_START;
		}
		~TimerManager()
		{
		}
        void updateTimer();
	};
}
#endif
