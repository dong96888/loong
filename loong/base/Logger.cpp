#include "Logger.h"

#include "BaseType.h"
#include "Utils.h"
#include "BufferPool.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>


#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

namespace loong
{
	const uint16_t LOG_LINE_SIZE=4096;
	const uint32_t MAX_LOG_SIZE = 1024 * 1024 * 10;
	static const char* LevelMsg[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
	
	Logger::Logger(const char* fileName)
	{
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		m_logPath=fileName;
		openFile(fileName);
		m_level = LOG_DEBUG;
		m_lastDate=0;
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
	}
	
	Logger::Logger()
	{
		m_fd=stdout;
		m_level = LOG_DEBUG;
		m_lastDate=0;
	}

	Logger::~Logger()
	{
		if (m_fd!=stdout)
		{
			fclose(m_fd);
		}
	}

	bool Logger::openFile(string filepath)
	{
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		if (filepath=="")
		{
			filepath="./log";
		}
		if (access(filepath.c_str(), R_OK) != 0)
		{
			string str="mkdir -p "+filepath;
			system(str.c_str());
		}
		char timebuf[80] = { '\0' };
		long curTime = time(nullptr);
		struct tm *stm = localtime(&curTime);
		strftime(timebuf, 80, "%Y-%m-%d", stm);
		char fileName[100] = { '\0' };
		snprintf(fileName, 100, "./log/%s.log", timebuf);
		m_fd = fopen(fileName, "ab+");
		if (m_fd == nullptr)
		{
			cout << "open log file failed!" << endl;
			return false;
		}
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		return true;
	}

	void Logger::closeFile()
	{
		fclose(m_fd);
		m_fd = nullptr;
	}

	void Logger::write(const char *file, const int line, Level level, const char *format, ...)
	{
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		//cout << "-----"<<gettid()<<"------:" << pthread_self() << endl;
		if (m_level > level)
		{
			return;
		}

		char buf[LOG_LINE_SIZE]={'\0'};
		memset(buf, 0, LOG_LINE_SIZE);
		char strtime[80] = { '\0' };
		getMsecTime(strtime);
		int n = snprintf(buf, sizeof(buf), "[%s %s] %s:%d ", strtime, LevelMsg[int(level)], file, line);
		
		va_list ap;
		va_start(ap, format);
		int logLen = vsnprintf(buf + n, LOG_LINE_SIZE - n - 1, format, ap);
		va_end(ap);
		
		time_t currTime=time(nullptr);
		time_t currDate=currTime-currTime%(24*60*60);		
		
		m_mtx.lock();
		if (m_fd!=stdout)
		{
			if (currDate>m_lastDate)
			{
				m_lastDate=currDate;
				closeFile();
				if (!openFile(m_logPath))
				{
					cout << "open file failed!!!" << endl;
				}
			}
			
			/*
			fflush(m_fd);
			if (fseek(m_fd, 0L, SEEK_END) == -1)
			{
				cout << "fseek error:" << strerror(errno) << endl;
			}

			//如果日志文件大于指定的文件大小则创建一个新的文件
			if (ftell(m_fd) >= MAX_LOG_SIZE)
			{
				closeFile();
				if (!openFile(m_logPath))
				{
					cout << "open file failed!!!" << endl;
				}
			}
			*/			
		}
		fprintf(m_fd, "%s\n", buf);
		m_mtx.unlock();
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
	}
}

#if 0
int main()
{
	if (!Logger::getInstance().openFile())
	{
		cout<<"open file failed"<<endl;
		return 0;
	}
	Logger::getInstance().setLevel(Logger::LOG_DEBUG);
	Logger::getInstance().start();

	DEBUG("1111111111");
	INFO("222222222222");
	WARN("333333333333");
	ERROR("66666666666666666");
	sleep(1000000);
	return 1;
}
#endif









