#include "TimerManager.h"

#include <iostream>
#include <unistd.h>

using namespace std;

namespace loong
{
	pthread_mutex_t TimerManager::m_mutex = PTHREAD_MUTEX_INITIALIZER;
	//pthread_mutex_t TimerManager::m_lstMutex = PTHREAD_MUTEX_INITIALIZER;
	TimerManager *TimerManager::m_instance = nullptr;

	TimerManager &TimerManager::instance()
	{
		if (m_instance == nullptr)
		{
			pthread_mutex_lock(&m_mutex);
			if (m_instance == nullptr)
			{
				m_instance = new TimerManager();
			}
			pthread_mutex_unlock(&m_mutex);
		}

		return *m_instance;
	}

	void TimerManager::addTimer(Timer *timer)
	{
		cout<<"&&&&&&&&Add timer&&&&&&&&&&"<<endl;
		timer->m_timeout = time(nullptr) + timer->m_interval;
		pthread_mutex_lock(&m_lstMutex);
		if (m_headTimer == nullptr)
		{
			m_headTimer = timer;
			timer->m_next = nullptr;
		}
		else if (m_headTimer != nullptr && m_headTimer->m_next == nullptr)
		{
			if (m_headTimer->m_timeout>timer->m_timeout)
			{
				timer->m_next = m_headTimer;
				m_headTimer->m_next = nullptr;
				m_headTimer = timer;
			}
			else
			{
				m_headTimer->m_next = timer;
				timer->m_next = nullptr;
			}
		}
		else
		{
			if(m_headTimer->m_timeout<timer->m_timeout)
			{
				timer->m_next=m_headTimer;
				m_headTimer=timer;
			}
			else
			{
				Timer *u = m_headTimer;
				Timer *p = u->m_next;
				char flag = '0';
				while (p != nullptr)
				{
					if (timer->m_timeout > p->m_timeout)
					{
						u = p;
						p = p->m_next;
					}
					else
					{
						u->m_next=timer;
						timer->m_next=p;
						flag = '1';
						break;
					}
				}

				if (flag == 0)
				{
					u->m_next = timer;
					timer->m_next=nullptr;
				}
			}
		}
		cout << "add timer:" << m_headTimer->m_timeout << endl;
		pthread_mutex_unlock(&m_lstMutex);
	}

	void TimerManager::delTimer(Timer *timer)
	{
		pthread_mutex_lock(&m_lstMutex);
		if (m_headTimer == timer && m_headTimer->m_next == nullptr)
		{
			m_headTimer = nullptr;
			delete timer;
			pthread_mutex_unlock(&m_lstMutex);
			return;
		}
		else if (m_headTimer == timer && m_headTimer->m_next != nullptr)
		{
			m_headTimer = m_headTimer->m_next;
			delete timer;
			pthread_mutex_unlock(&m_lstMutex);
			return;
		}

		Timer *u = m_headTimer;
		Timer *p = u->m_next;
		while (p != nullptr)
		{
			if (p == timer)
			{
				u->m_next=p->m_next;
				delete timer;
				pthread_mutex_unlock(&m_lstMutex);
				return;
			}
			else
			{
				u = p;
				p = p->m_next;
			}
		}
		pthread_mutex_unlock(&m_lstMutex);
	}

	void TimerManager::updateTimer()
	{
		if(m_headTimer==nullptr)
		{
			return;
		}

		m_headTimer->m_timeout = time(nullptr) + m_headTimer->m_interval;
		if (m_headTimer->m_next==nullptr)
		{
			return ;
		}

		//更新定时器的时间的时候，必然是更新头指针，将头指针往后移
		Timer *u=m_headTimer;
		Timer *p=u->m_next;
		char flag='0';
		while (p != nullptr)
		{
			if (m_headTimer->m_timeout > p->m_timeout)
			{
				u=p;
				p=p->m_next;
			}
			else
			{
				flag='1';
		        u->m_next=m_headTimer;
				Timer *tmp=m_headTimer->m_next;
				m_headTimer->m_next=p;
				m_headTimer=tmp;
				break;
			}
		}

		if(flag=='0')
		{
			u->m_next=m_headTimer;
			m_headTimer=m_headTimer->m_next;
			u->m_next->m_next=nullptr;
		}
	}

	void TimerManager::run()
	{
		while (m_state == TIMER_MANAGER_START)
		{
			pthread_mutex_lock(&m_lstMutex);
			Timer *p = m_headTimer;
			while (p != nullptr)
			{
				if (p->m_timeout <= time(nullptr))
				{
					(p->handleTimeout)();
					if (p->m_type == Timer::TIMER_ONCE)
					{
						cout << "*********once timer*********" << endl;
						Timer *tmp = p;
						p = p->m_next;
						m_headTimer = p;
						delete tmp;
					}
					else
					{
						cout << "*********cricle timer*********" << endl;
						updateTimer();
					}
				}
				else
				{
					break;
				}
			}
			pthread_mutex_unlock(&m_lstMutex);

			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 100000;
			//select(0, nullptr, nullptr, nullptr, &tv);
			sleep(1);
			cout << "again 1 second" << endl;
		}

		cout << "thread exit" << endl;
		return;
	}
}




