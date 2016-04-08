#include "TimingWheel.h"

#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>

namespace loong
{
	TimingWheel g_timingWheel;
	TimingWheel::TimingWheel() : m_currSlot(0)
	{
		//获得服务器时间
		m_lastTickTime = time(nullptr);
		for (int i = 0; i < N; ++i)
		{
			m_slots[i] = nullptr;
		}
	}
	TimingWheel::~TimingWheel()
	{
		for (int i = 0; i < N; ++i)
		{
			WTimer* tmp = m_slots[i];
			while (tmp)
			{
				m_slots[i] = tmp->next;
				delete tmp;
				tmp = m_slots[i];
			}
		}
	}
	WTimer* TimingWheel::addTimer(int timeout, callback TimeOutCall, void *data)
	{
		if (timeout < 0)
		{
			return nullptr;
		}
		int ticks = 0;
		//最少要一个滴答间隔
		if (timeout < TI)
		{
			ticks = 1;
		}
		else
		{
			ticks = timeout / TI;
		}
		//m_rotation为0表示定时器到期
		int m_rotation = ticks / N;
		//计算槽索引
		int ts = (m_currSlot + (ticks % N)) % N;
		WTimer* timer = new WTimer(m_rotation, ts, TimeOutCall, data);
		//当前的槽上没有定时器就放在head位置，否则放在插入在head位置
		m_lock.lock();
		if (!m_slots[ts])
		{
			printf("add timer, m_rotation is %d, ts is %d, m_currSlot is %d\n", m_rotation, ts, m_currSlot);
			m_slots[ts] = timer;
		}
		else
		{
			timer->next = m_slots[ts];
			m_slots[ts]->prev = timer;
			m_slots[ts] = timer;
		}
		m_lock.unlock();
		return timer;
	}
	//删除一个定时器，主要是链表的删除的操作
	void TimingWheel::delTimer(WTimer* timer)
	{
		if (!timer)
		{
			return;
		}
		m_lock.lock();
		int ts = timer->m_timeSlot;
		if (timer == m_slots[ts])
		{
			m_slots[ts] = m_slots[ts]->next;
			if (m_slots[ts])
			{
				m_slots[ts]->prev = nullptr;
			}
			delete timer;
		}
		else
		{
			timer->prev->next = timer->next;
			if (timer->next)
			{
				timer->next->prev = timer->prev;
			}
			delete timer;
		}
		m_lock.unlock();
	}


	//每一个滴答间隔调用一次tick函数 time为当前服务器时间
	void TimingWheel::tick(unsigned long currTime)
	{
		//计算更新间隔进过了多少个滴答
		uint32_t tiCount = (currTime - m_lastTickTime) / TI;

		m_lock.lock();
		WTimer* tmp = m_slots[m_currSlot];
		printf("current slot is %d\n", m_currSlot);
		for (uint32_t i = 0; i < tiCount; ++i)
		{
			while (tmp)
			{
				printf("tick the timer once\n");
				if (tmp->m_rotation > 0)
				{
					tmp->m_rotation--;
					tmp = tmp->next;
				}
				else
				{
					tmp->timeOutfunc(tmp->m_callbackPara);
					if (tmp == m_slots[m_currSlot])
					{
						printf("delete header in m_currSlot\n");
						m_slots[m_currSlot] = tmp->next;
						delete tmp;
						if (m_slots[m_currSlot])
						{
							m_slots[m_currSlot]->prev = nullptr;
						}
						tmp = m_slots[m_currSlot];
					}
					else
					{
						tmp->prev->next = tmp->next;
						if (tmp->next)
						{
							tmp->next->prev = tmp->prev;
						}
						WTimer* tmp2 = tmp->next;
						delete tmp;
						tmp = tmp2;
					}
				}
			}
			//移动到下一个槽，时间轮是环所以需要%N
			++m_currSlot;
			m_currSlot = m_currSlot % N;

			m_lock.unlock();
		}
		m_lastTickTime = currTime;
	}


	void TimingWheel::run()
	{
		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 1000 * 100;
		while (true)
		{
			select(0, nullptr, nullptr, nullptr, &tv);
			tick(time(nullptr));
		}
	}
}
