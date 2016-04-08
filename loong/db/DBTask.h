/*
*   File name: DBTask.h
*	 Describe:
*  Created on: 2015-4-16
*      Author: dengxiudong
*/

#ifndef _DB_TASK_H_
#define _DB_TASK_H_

#include "Thread.h"
#include "Cond.h"

#include "mysql_connection.h"
#include "mysql_driver.h"
#include "cppconn/exception.h"
#include "cppconn/driver.h"
#include "cppconn/connection.h"
#include "cppconn/resultset.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/statement.h"


#include <list>

using namespace std;
using namespace sql;

namespace loong
{
	class DBTask :
		public Thread
	{
	public:
		DBTask(string dbip, string userName, string password, string dbName);
		~DBTask();

		void addTask(string sql);

	private:
		void run();

	private:
		Connection * createConnetion();
		void execTask(string sql);

	private:
		list<string> m_taskList;
		Driver *m_driver;
		Connection *m_dbConn;
		Cond m_cond;

		//数据库相关信息
		char m_connUrl[32];
		string m_userName;
		string m_password;
		string m_dbName;

	};
}

#endif

