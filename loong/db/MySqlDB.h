/*
*   File name: MySqlDB.h
*	 Describe: 支持单线程和多线程，在单线程中使用一个连接，多线程中定义一个临时对象，
*			   传入连接池对象的指针，从连接池中获取连接，临时对象超出作用域释放对象
*			   时会自动释放连接到连接池。
*  Created on: 2015-8-5
*      Author: dengxiudong
*/

#ifndef _MYSQL_DB_H_ 
#define _MYSQL_DB_H_ 
   
#include "MySqlPool.h"

#include <mysql/mysql.h>
#include <stdint.h>

#include <iostream> 
#include <string> 
#include <vector>
#include <map>
   
using namespace std; 
namespace loong
{
	class ResultSet
	{
	public:
		ResultSet(MYSQL_RES* res);
		virtual ~ResultSet();

		bool next();
		int getIndex(const string & columnLabel);

		double getDouble(uint32_t columnIndex);
		float getFloat(const string& columnLabel);
		double getDouble(const string& columnLabel);

		string getString(uint32_t columnIndex);
		string getString(const string& columnLabel);

		int32_t getInt(uint32_t columnIndex);
		int32_t getInt(const string& columnLabel);

		uint32_t getUInt(uint32_t columnIndex);
		uint32_t getUInt(const string& columnLabel);

		int64_t getInt64(uint32_t columnIndex);
		int64_t getInt64(const string& columnLabel);

		uint64_t getUInt64(uint32_t columnIndex);
		uint64_t getUInt64(const string& columnLabel);
	private:
		int getIndex(const char* key);

		MYSQL_RES* 			m_res;
		MYSQL_ROW			m_row;
		map<string, int>	m_keyMap;
	};

	class MySqlDB
	{
	public:
		MySqlDB();
		MySqlDB(MySqlPool *pool);
		virtual ~MySqlDB();
		//单个连接时连接数据库
		void connect(char* server, int port, char* username, char* password, char* database);
		//传入sql语句执行数据库操作
		void execSQL(const string sqlStr);
		//传入sql语句执行更新数据，返回更新数据的记录数
		int update(const string sqlStr);
		//传入sql语言直接插入数据，返回插入的自增ID
		uint64_t insert(const string sqlStr);
		ResultSet * query(const string sqlStr);
		//查询表获得多条结果
		int query(const string sqlStr, vector<vector<string> >& data);
		//查询表获得一条记录
		int query(const string sqlStr, vector<string>& data);

		void execTrans(vector<string> &sqlVec);
		void beginTrans();
		void rollback();
		void commit();

		//写入二进制值(用stmt),返回last_insert_id(),并设置err值(成功值为0)
		//insert into TABLE(key1,key2,key3) values(val1,val2,val3)
		uint64_t insert(string table, map<string, string> &value);
		//更改二进制值(用stmt),返回更新的行数
		//update TABLE set key1=val1,key2=val2,key3=val3 where FILTER
		int update(string table, map<string, string> &value, string filter = "");

	private:
		MYSQL *mysqlconn;             //MySQL对象
		MYSQL_RES *result;            //MySQL查询返回的结果
		bool isPoolConn;
		MySqlPool *connPool;
	};
}

#endif
