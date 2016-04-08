/*
*   File name: TimingWheel.h
*	 Describe: ��ʱ���࣬ʹ������Linux�ں˵�ʱ����ʵ�ַ���
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
		//rot��ת��Ȧ��ʱ������
		//ts �۵�����
		WTimer(int rot, int ts, callback TimeOutCall, void *data)
			: m_rotation(rot), m_timeSlot(ts), timeOutfunc(TimeOutCall),
			m_callbackPara(data), next(nullptr), prev(nullptr)
		{
		}

	public:
		//��ת��Ȧ��ʱ������
		int m_rotation;
		// �۵�����
		int m_timeSlot;
		//��ʱ��Ļص�����
		callback timeOutfunc;
		//�Զ��庯��
		void* m_callbackPara;
		//�����ָ��
		WTimer* next;
		WTimer* prev;
	};

	class TimingWheel :public Thread
	{
	public:
		TimingWheel();

		~TimingWheel();

		WTimer* addTimer(int timeout, callback TimeOutCall, void *data);

		//ɾ��һ����ʱ������Ҫ�������ɾ���Ĳ���
		void delTimer(WTimer* timer);

		//ÿһ���δ�������һ��tick���� timeΪ��ǰ������ʱ��
		void tick(unsigned long currTime);
		void run();

	private:
		//�۸���
		static const int N = 3600;
		//�δ���(ÿ�ƶ�һ���۵�ʱ����)
		static const int TI = 1;
		//ʱ����
		WTimer *m_slots[N];
		//��ǰ������
		int m_currSlot;
		//������
		unsigned int m_lastTickTime;
		Lock m_lock;
	};

	extern TimingWheel g_timingWheel;
}

#endif
