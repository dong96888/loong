/*
*   File name: Logger.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _ASYN_LOG_H_
#define _ASYN_LOG_H_

#include "Thread.h"
#include "Cond.h"
#include "LoopQueue.h"

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <stdio.h>

using namespace std;

namespace loong
{
	class Logger
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
		void write(const char * file, const int line, Level level, const char * format, ...);
		static Logger &getInstance()
		{
			static Logger log("");
			return log;
		}


	private:
		Logger(const char* fileName);
		Logger();
		~Logger();
		
		bool openFile(string filepath="");
		void closeFile();

	private:
		FILE *m_fd;
		Level m_level;
		string m_logPath;
		mutex m_mtx;
		time_t m_lastDate;
	};

#define DEBUG(...) \
do \
{ \
        Logger::getInstance().write(__FILE__, __LINE__, Logger::LOG_DEBUG, __VA_ARGS__); \
} while (0)

#define INFO(...) \
do \
{ \
        Logger::getInstance().write(__FILE__, __LINE__, Logger::LOG_INFO, __VA_ARGS__); \
} while (0)

#define WARN(...) \
do \
{ \
        Logger::getInstance().write(__FILE__, __LINE__, Logger::LOG_WARN, __VA_ARGS__); \
} while (0)

#define ERROR(...) \
do \
{ \
        Logger::getInstance().write(__FILE__, __LINE__, Logger::LOG_ERROR, __VA_ARGS__); \
} while (0)

#define FATAL(...) \
do \
{ \
        Logger::getInstance().write(__FILE__, __LINE__, Logger::LOG_FATAL, __VA_ARGS__); \
} while (0)
}

#endif







