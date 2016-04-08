/*
*   File name: LoopQueue.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef __LOOPQUEUE__
#define __LOOPQUEUE__

#include "LoopQueue.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>


namespace loong
{
	const uint32_t QUEUE_SIZE = 1024;

	template <class _Type>
	class LoopQueue
	{
	public:
		LoopQueue();
		LoopQueue(uint32_t bufsize);
		virtual ~LoopQueue();
		_Type pop_front();
		bool push_back(_Type item);
		//返回容量:warning:需要外部数据一致性
		uint32_t capacity() {
			return m_capacity;
		}
		//返回当前个数:warning:需要外部数据一致性
		uint32_t size() {
			return m_size;
		}
		//是否满:warning: 需要外部控制数据一致性
		bool full() {
			return (m_size >= m_capacity);
		}
		bool empty() {
			return (m_size == 0);
		}
		
		void swap(LoopQueue<_Type> &oth);
		
		void clear();

	protected:
		uint32_t m_head;
		uint32_t m_tail;
		uint32_t m_size;
		uint32_t m_capacity;
		_Type *pBuf;

	};


	template <class _Type>
	LoopQueue<_Type>::LoopQueue() : m_head(0), m_tail(0), m_size(0)
	{
		pBuf = new _Type[QUEUE_SIZE];//默认QUEUE_SIZE
		m_capacity = QUEUE_SIZE;
	}


	template <class _Type>
	LoopQueue<_Type>::LoopQueue(unsigned int bufsize) : m_head(0), m_tail(0)
	{
		if (bufsize > QUEUE_SIZE || bufsize < 1)
		{
			pBuf = new _Type[QUEUE_SIZE];
			m_capacity = QUEUE_SIZE;
		}
		else
		{
			pBuf = new _Type[bufsize];
			m_capacity = bufsize;
		}
	}


	template <class _Type>
	LoopQueue<_Type>::~LoopQueue()
	{
		delete[] pBuf;
		pBuf = nullptr;
		m_head = m_tail = m_size = m_capacity = 0;
	}


	//前面弹出一个元素
	template <class _Type>
	_Type LoopQueue<_Type>::pop_front()
	{
		if (empty())
		{
			return nullptr;
		}
		_Type itemtmp;
		itemtmp = pBuf[m_head];
		m_head = (m_head + 1) % m_capacity;
		--m_size;
		return itemtmp;
	}


	//从尾部加入队列
	template <class _Type>
	bool LoopQueue<_Type>::push_back(_Type item)
	{
		if (full())
		{
			printf("loop queue full!\n");
			return false;
		}
		pBuf[m_tail] = item;
		m_tail = (m_tail + 1) % m_capacity;
		++m_size;
		return true;
	}
	
	//交换队列
	template <class _Type>
	void LoopQueue<_Type>::swap(LoopQueue<_Type> &oth)
	{
		std::swap(m_head,oth.m_head);
		std::swap(m_tail,oth.m_tail);
		std::swap(m_capacity,oth.m_capacity);
		std::swap(m_size,oth.m_size);
	}
	
	template <class _Type>
	void LoopQueue<_Type>::clear()
	{
		m_head=0;
		m_tail=0;
	}
}

#endif
