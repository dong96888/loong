/*
*   File name: RecvMessage.h
*	 Describe: 缓冲池，实现循环使用内存，以免产生内存碎片
*  Created on: 2015-3-3
*      Author: dengxiudong
*/


#ifndef _BUFFER_POOL_H_
#define _BUFFER_POOL_H_

#include "LoopQueue.h"
#include "Lock.h"

#include <stdint.h>

namespace loong
{
	class BufferPool
	{
	public:
		BufferPool();
		BufferPool(uint32_t size);
		~BufferPool();

		char * getBuf();
		void releaseBuf(char *bufPtr);
	private:
		LoopQueue<char *> m_bufQue;
		uint16_t m_size;
		Lock m_lock;
	};
	extern BufferPool *g_buffPool;
}

#endif

