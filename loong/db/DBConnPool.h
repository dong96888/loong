/*
*   File name: DBConnPool.h
*	 Describe:
*  Created on: 2015-4-16
*      Author: dengxiudong
*/

#ifndef _DB_CONN_POOL_H_
#define _DB_CONN_POOL_H_


#include "mysql_connection.h"
#include "mysql_driver.h"
#include "cppconn/exception.h"
#include "cppconn/driver.h"
#include "cppconn/connection.h"
#include "cppconn/resultset.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/statement.h"

#include <pthread.h>
#include <list>


using namespace std;
using namespace sql;


extern string g_dbip;
extern string g_dbName;
extern string g_password;

namespace loong
{
	/*
	*连接池类
	*/
	class ConnPool
	{
	private:
		int m_curSize;//当前已建立的连接数
		int m_maxSize;//最大连接数
		string  m_username;
		string  m_password;
		string  m_url;

		list<Connection*> m_connList;//连接池的容器队列
		pthread_mutex_t m_lock; //线程锁

		static  ConnPool *m_connPool;
		Driver* m_driver;

		Connection* createConnection();//创建一个连接
		void initConnection(int initConnNum);//初始化连接池
		void destoryConnection(Connection *conn);//销毁连接对象
		void destoryConnPool();//销毁连接池
		ConnPool(string url, string user, string password, int maxSize);//构造方案
	public:
		~ConnPool();
		Connection* getConnection();//获得数据库连接
		void releaseConnection(Connection *conn);//释放连接
		static ConnPool* getInstance();//获取连接池对象
		static int releaseInstance();       //释放对象
	};
}

#endif //__DBCONNECTPOOL_H__
