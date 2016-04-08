/*
*   File name: TimingWheel.h
*	 Describe: 定时器类，使用类似Linux内核的时间轮实现方法
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _TIMING_WHEEL_H_
#define _TIMING_WHEEL_H_

#include "Thread.h"
#include "Lock.h"


namespace loong
{
	typedef void(*callback)(void *);
	class WTimer
	{
	public:
		//rot轮转几圈定时器到期
		//ts 槽的索引
		WTimer(int rot, int ts, callback TimeOutCall, void *data)
			: m_rotation(rot), m_timeSlot(ts), timeOutfunc(TimeOutCall),
			m_callbackPara(data), next(nullptr), prev(nullptr)
		{
		}

	public:
		//轮转几圈定时器到期
		int m_rotation;
		// 槽的索引
		int m_timeSlot;
		//到时后的回调函数
		callback timeOutfunc;
		//自定义函数
		void* m_callbackPara;
		//链表的指针
		WTimer* next;
		WTimer* prev;
	};

	class TimingWheel :public Thread
	{
	public:
		TimingWheel();

		~TimingWheel();

		WTimer* addTimer(int timeout, callback TimeOutCall, void *data);

		//删除一个定时器，主要是链表的删除的操作
		void delTimer(WTimer* timer);

		//每一个滴答间隔调用一次tick函数 time为当前服务器时间
		void tick(unsigned long currTime);
		void run();

	private:
		//槽个数
		static const int N = 3600;
		//滴答间隔(每移动一个槽的时间间隔)
		static const int TI = 1;
		//时间轮
		WTimer *m_slots[N];
		//当前槽索引
		int m_currSlot;
		//最后更新
		unsigned int m_lastTickTime;
		Lock m_lock;
	};

	extern TimingWheel g_timingWheel;
}

#endif
