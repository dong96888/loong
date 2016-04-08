#include "AsynLog.h"

#include "BaseType.h"
#include "Utils.h"

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


#include <iostream>

using namespace std;

namespace loong
{
	const uint32_t MAX_LOG_SIZE = 1024 * 1024 * 10;
	static const char* LevelMsg[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
	AsynLog::AsynLog()
	{
		m_level = LOG_DEBUG;
		m_bufSize = 2048;
		m_buf = (char*)malloc(m_bufSize);
		memset(m_buf, 0, m_bufSize);
	}

	AsynLog::~AsynLog()
	{
		if (m_buf != nullptr)
		{
			free(m_buf);
		}
	}

	void AsynLog::run()
	{
		//循环从队列中获取日志并写入日志文件
		while (true)
		{
			m_cond.lock();
			while (m_logQue.empty())
			{
				m_cond.wait();
			}
			m_logQue.swap(m_localQue);
			m_cond.unlock();

			deque<string>::iterator ite = m_localQue.begin();
			for (; ite != m_localQue.end(); ite++)
			{
				fflush(m_fd);
				if (fseek(m_fd, 0L, SEEK_END) == -1)
				{
					cout << "fseek error:" << strerror(errno) << endl;
				}

				//如果日志文件大于指定的文件大小则创建一个新的文件
				if (ftell(m_fd) >= MAX_LOG_SIZE)
				{
					closeFile();
					if (!openFile())
					{
						cout << "open file failed!!!" << endl;
					}
				}
				fprintf(m_fd, "%s\n", (*ite).c_str());
				fflush(m_fd);
			}
			m_localQue.clear();
		}
	}


	bool AsynLog::openFile()
	{
		if (access("./log", R_OK) != 0)
		{
			system("mkdir log");
		}

		char timebuf[80] = { '\0' };
		long curTime = time(nullptr);
		struct tm *stm = localtime(&curTime);
		strftime(timebuf, 80, "%Y-%m-%d", stm);
		char fileName[100] = { '\0' };
		snprintf(fileName, 100, "./log/log%s.log", timebuf);
		m_fd = fopen(fileName, "ab+");
		if (m_fd == nullptr)
		{
			cout << "open log file failed!" << endl;
			return false;
		}

		return true;
	}

	void AsynLog::closeFile()
	{
		fclose(m_fd);
		m_fd = nullptr;
	}

	void AsynLog::write(const char *file, const int line, Level level, const char *format, ...)
	{
		//cout << "-----"<<gettid()<<"------:" << pthread_self() << endl;
		if (m_level > level)
		{
			return;
		}
		memset(m_buf, 0, m_bufSize);
		char strtime[80] = { '\0' };
		getMsecTime(strtime);
		int n = snprintf(m_buf, m_bufSize, "[%s %s] %s:%d ", strtime, LevelMsg[int(level)], file, line);
		va_list ap;
		va_start(ap, format);
		while (true)
		{
			int logLen = vsnprintf(m_buf + n, m_bufSize - n - 1, format, ap);
			if (logLen >= m_bufSize - n - 2)
			{
				m_bufSize *= 2;
				char *tmp = m_buf;
				m_buf = (char *)realloc(m_buf, m_bufSize);
				if (m_buf == nullptr)
				{
					m_buf = tmp;
					tmp = nullptr;
					break;
				}
			}
			else
			{
				break;
			}
		}
		va_end(ap);
		m_cond.lock();
		m_logQue.push_back(m_buf);
		m_cond.signal();
	}
}

#if 0
int main()
{
    if (!AsynLog::getInstance()->openFile())
    {
        cout<<"open file failed"<<endl;
        return 0;
    }
	AsynLog::getInstance()->setLevel(AsynLog::LOG_DEBUG);
	AsynLog::getInstance()->start();

    DEBUG("1111111111");
    INFO("222222222222");
    WARN("333333333333");
    ERROR("66666666666666666");
	sleep(1000000);
    return 1;
}
#endif
























