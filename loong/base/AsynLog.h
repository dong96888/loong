/*
*   File name: AsynLog.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _ASYN_LOG_H_
#define _ASYN_LOG_H_

#include "Thread.h"
#include "Cond.h"

#include <string>
#include <deque>
#include <stdio.h>

using namespace std;

namespace loong
{
	class AsynLog :public Thread
	{
	public:
		enum Level
		{
			LOG_DEBUG = 0,
			LOG_INFO,
			LOG_WARN,
			LOG_ERROR,
			LOG_FATAL,
		};

		//设置日志级别
		void setLevel(Level level) { m_level = level; }
		Level getLevel() { return m_level; }
		bool openFile();
		void write(const char * file, const int line, Level level, const char * format, ...);
		static AsynLog *getInstance()
		{
			static AsynLog log;
			return &log;
		}

	protected:
		void run();

	private:
		AsynLog();
		~AsynLog();
		void closeFile();

	private:
		FILE *m_fd;

		char *m_buf;
		int m_bufSize;

		Level m_level;
		//共享日志队列
		deque<string> m_logQue;
		//本线程交换队列
		deque<string> m_localQue;
		Cond m_cond;
	};

#define DEBUG(...) \
do \
{ \
        AsynLog::getInstance()->write(__FILE__, __LINE__, AsynLog::LOG_DEBUG, __VA_ARGS__); \
} while (0)

#define INFO(...) \
do \
{ \
        AsynLog::getInstance()->write(__FILE__, __LINE__, AsynLog::LOG_INFO, __VA_ARGS__); \
} while (0)

#define WARN(...) \
do \
{ \
        AsynLog::getInstance()->write(__FILE__, __LINE__, AsynLog::LOG_WARN, __VA_ARGS__); \
} while (0)

#define ERROR(...) \
do \
{ \
        AsynLog::getInstance()->write(__FILE__, __LINE__, AsynLog::LOG_ERROR, __VA_ARGS__); \
} while (0)

#define FATAL(...) \
do \
{ \
        AsynLog::getInstance()->write(__FILE__, __LINE__, AsynLog::LOG_FATAL, __VA_ARGS__); \
} while (0)
}

#endif







