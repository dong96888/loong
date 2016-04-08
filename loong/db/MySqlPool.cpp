/*************************************************************************
* @File: MySqlPool.cpp
* @Author:  dengxiudong 258446166@qq.com
* @Date: 2015年08月06日 星期四 09时20分53秒
* @Brief:
*
*
**************************************************************************/
#include "MySqlPool.h" 

#include "Lock.h"

#include <string.h>
#include <iterator>

namespace loong
{
	//构造函数 初始化各个变量和数据 
	MySqlPool::MySqlPool(int poolsize)
	{
		if ((sem_init(&queSem, 0, 0)) < 0) {
			throw errno;
		}

		SmartLock slock(queLock);
		for (int i = 0; i < poolsize; ++i)
		{
			MYSQL *conn = new MYSQL;
			if (mysql_library_init(0, nullptr, nullptr) != 0)
			{
				throw strerror(errno);
			}
			if (mysql_init(conn) == nullptr)
			{
				throw mysql_error(conn);
			}
			if (mysql_options(conn, MYSQL_READ_DEFAULT_GROUP, "libmysqld_client_r"))
			{
				throw mysql_error(conn);
			}
			if (mysql_options(conn, MYSQL_OPT_COMPRESS, nullptr))
			{
				throw mysql_error(conn);
			}
			if (mysql_options(conn, MYSQL_SET_CHARSET_NAME, "gbk") != 0)
			{
				throw mysql_error(conn);
			}
			queMySqlConn.push_back(conn);
		}
	}

	MySqlPool::~MySqlPool()
	{
		SmartLock slock(queLock);
		deque<MYSQL*>::iterator it = queMySqlConn.begin();
		for (; it != queMySqlConn.end(); it++)
		{
			mysql_close(*it);
			mysql_library_end();
			delete *it;
		}
		sem_destroy(&queSem);
	}

	int MySqlPool::create(string server, int port, string username,
		string password, string database, string &errMsg)
	{
		SmartLock slock(queLock);
		int errCode = 0;
		deque<MYSQL*>::iterator it = queMySqlConn.begin();
		for (; it != queMySqlConn.end(); it++)
		{
			if (mysql_real_connect(*it, server.c_str(), username.c_str(), password.c_str(),
				database.c_str(), port, 0, 0) == nullptr)
			{
				errMsg = mysql_error(*it);
				errCode = mysql_errno(*it);
				return errCode;
			}
			char value = 1;
			errCode = mysql_options(*it, MYSQL_OPT_RECONNECT, &value);
			if (errCode != 0)
			{
				errMsg = mysql_error(*it);
				return errCode;
			}
			sem_post(&queSem);
		}
		return errCode;
	}

	MYSQL *MySqlPool::getConn()
	{
		sem_wait(&queSem);
		SmartLock slock(queLock);
		MYSQL *conn = queMySqlConn.front();
		queMySqlConn.pop_front();
		return conn;
	}

	void MySqlPool::pushConn(MYSQL *conn)
	{
		SmartLock slock(queLock);
		queMySqlConn.push_back(conn);
		sem_post(&queSem);
	}
}
