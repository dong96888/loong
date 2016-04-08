/*
*   File name: DBException.h
*	 Describe: MySQL数据库的异常处理类。
*  Created on: 2015-8-5
*      Author: dengxiudong
*/

#ifndef _MYSQL_POOL_H_
#define _MYSQL_POOL_H_ 
   

#include "Lock.h"

#include <mysql/mysql.h>
#include <stdint.h>
#include <semaphore.h>

#include <deque>
#include <iostream> 
#include <string> 
#include <vector> 
   
using namespace std; 

namespace loong
{
	class MySqlPool
	{
	public:
		MySqlPool(int poolsize);
		virtual ~MySqlPool();

		int create(string server, int port, string username, string password,
			string database, string &errMsg);
		MYSQL *getConn();
		void pushConn(MYSQL *conn);

	private:
		Lock queLock;
		sem_t queSem;
		deque<MYSQL *> queMySqlConn;

	};
}

#endif
