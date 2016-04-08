#include "BufferPool.h"

#include <stdlib.h>


namespace loong
{
	const uint32_t BUF_POOL_SIZE = 1024;
	const uint32_t BUF_SIZE = 4096;

	BufferPool *g_buffPool = new BufferPool();
	BufferPool::BufferPool()
	{
		m_size = BUF_POOL_SIZE;
		char *bufPtr = nullptr;
		//创建一定数量的内存，并放入队列
		for (unsigned int i = 0; i < BUF_POOL_SIZE; i++)
		{
			bufPtr = new char[BUF_SIZE];
			m_bufQue.push_back(bufPtr);
		}
	}

	BufferPool::BufferPool(uint32_t size) :m_size(size)
	{
		char *bufPtr = nullptr;
		for (unsigned int i = 0; i < m_size; i++)
		{
			bufPtr = new char[BUF_SIZE];
			m_bufQue.push_back(bufPtr);
		}
	}


	BufferPool::~BufferPool()
	{
		char *bufPtr = nullptr;
		for (unsigned int i = 0; i < m_size; i++)
		{
			bufPtr = m_bufQue.pop_front();
			delete[] bufPtr;
		}
	}

	//获得缓冲内存
	char *BufferPool::getBuf()
	{
		char *bufPtr = nullptr;
		m_lock.lock();
		if (m_bufQue.empty())
		{
			bufPtr = new char[BUF_SIZE];
		}
		else
		{
			bufPtr = m_bufQue.pop_front();
		}
		m_lock.unlock();

		return bufPtr;;
	}

	//释放不用了的缓冲内存
	void BufferPool::releaseBuf(char *bufPtr)
	{
		m_lock.lock();
		m_bufQue.push_back(bufPtr);
		m_lock.unlock();
	}
}
