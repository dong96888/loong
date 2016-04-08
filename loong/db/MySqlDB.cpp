/************************************************************************************
*   File name: MySqlDB.h
*	 Describe: 支持单线程和多线程，在单线程中使用一个连接，多线程中定义一个临时对象，
*			   传入连接池对象的指针，从连接池中获取连接，临时对象超出作用域释放对象
*			   时会自动释放连接到连接池。
*  Created on: 2015-8-5
*      Author: dengxiudong
*************************************************************************************/
#include "MySqlDB.h" 

#include "DBException.h"   
#include <string.h>
#include <sstream>

namespace loong
{
	ResultSet::ResultSet(MYSQL_RES* res)
	{
		m_res = res;

		// map table field key to index in the result array
		int num_fields = mysql_num_fields(m_res);
		MYSQL_FIELD* fields = mysql_fetch_fields(m_res);
		for (int i = 0; i < num_fields; i++)
		{
			m_keyMap.insert(make_pair(fields[i].name, i));
		}
	}

	ResultSet::~ResultSet()
	{
		if (m_res)
		{
			mysql_free_result(m_res);
			m_res = nullptr;
		}
	}

	bool ResultSet::next()
	{
		m_row = mysql_fetch_row(m_res);
		if (m_row)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int ResultSet::getIndex(const string& columnLabel)
	{
		map<string, int>::iterator it = m_keyMap.find(columnLabel);
		if (it == m_keyMap.end())
		{
			return -1;
		}
		else
		{
			return it->second;
		}
	}

	int32_t ResultSet::getInt(const string & columnLabel)
	{
		int idx = getIndex(columnLabel);
		if (idx == -1)
		{
			int errCode = -1;
			string errMsg = "Without this field!";
			throw DBException(errCode, errMsg);
		}

		int32_t value;
		stringstream ss;
		ss << m_row[idx];
		ss >> value;
		return value;
	}

	uint32_t ResultSet::getUInt(const string & columnLabel)
	{
		int idx = getIndex(columnLabel);
		if (idx == -1)
		{
			int errCode = -1;
			string errMsg = "Without this field!";
			throw DBException(errCode, errMsg);
		}

		uint32_t value;
		stringstream ss;
		ss << m_row[idx];
		ss >> value;
		return value;
	}

	int64_t ResultSet::getInt64(const string & columnLabel)
	{
		int idx = getIndex(columnLabel);
		if (idx == -1)
		{
			int errCode = -1;
			string errMsg = "Without this field!";
			throw DBException(errCode, errMsg);
		}

		int64_t value;
		stringstream ss;
		ss << m_row[idx];
		ss >> value;
		return value;
	}

	uint64_t ResultSet::getUInt64(const string & columnLabel)
	{
		int idx = getIndex(columnLabel);
		if (idx == -1)
		{
			int errCode = -1;
			string errMsg = "Without this field!";
			throw DBException(errCode, errMsg);
		}

		uint64_t value;
		stringstream ss;
		ss << m_row[idx];
		ss >> value;
		return value;
	}

	float ResultSet::getFloat(const string & columnLabel)
	{
		int idx = getIndex(columnLabel);
		if (idx == -1)
		{
			int errCode = -1;
			string errMsg = "Without this field!";
			throw DBException(errCode, errMsg);
		}

		float value;
		stringstream ss;
		ss << m_row[idx];
		ss >> value;
		return value;
	}

	double ResultSet::getDouble(const string & columnLabel)
	{
		int idx = getIndex(columnLabel);
		if (idx == -1)
		{
			int errCode = -1;
			string errMsg = "Without this field!";
			throw DBException(errCode, errMsg);
		}

		double value;
		stringstream ss;
		ss << m_row[idx];
		ss >> value;
		return value;
	}

	string ResultSet::getString(const string & columnLabel)
	{
		int idx = getIndex(columnLabel);
		if (idx == -1)
		{
			int errCode = -1;
			string errMsg = "Without this field!";
			throw DBException(errCode, errMsg);
		}

		return m_row[idx];
	}


	//用连接池的连接初始化
	MySqlDB::MySqlDB()
	{
		mysqlconn = new MYSQL;
		mysql_library_init(0, nullptr, nullptr);
		mysql_init(mysqlconn);
		mysql_options(mysqlconn, MYSQL_SET_CHARSET_NAME, "gbk");
		isPoolConn = false;
		result = nullptr;
	}

	//用连接池的连接初始化
	MySqlDB::MySqlDB(MySqlPool *pool) :
		connPool(pool)
	{
		mysqlconn = connPool->getConn();
		isPoolConn = true;
		result = nullptr;
	}

	MySqlDB::~MySqlDB()
	{
		if (!isPoolConn)
		{
			mysql_close(mysqlconn);
			delete mysqlconn;
		}
		else
		{
			connPool->pushConn(mysqlconn);
		}
	}

	//连接MySQL 
	void MySqlDB::connect(char* server, int port, char* username, char* password, char* database)
	{
		if (mysql_real_connect(mysqlconn, server, username, password, database, port, 0, 0) != nullptr)
		{
			char value = 1;
			mysql_options(mysqlconn, MYSQL_OPT_RECONNECT, &value);
			return;
		}

		string errMsg = mysql_error(mysqlconn);
		int errCode = mysql_errno(mysqlconn);
		throw DBException(errCode, errMsg);
	}

	void MySqlDB::execSQL(string sqlStr)
	{
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
	}

	int MySqlDB::update(const string sqlStr)
	{
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
		int rows = mysql_affected_rows(mysqlconn);
		return rows;
	}

	uint64_t MySqlDB::insert(const string sqlStr)
	{
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
		uint64_t seqId = mysql_insert_id(mysqlconn);
		return seqId;
	}

	//读取数据 
	ResultSet * MySqlDB::query(const string sqlStr)
	{
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}

		MYSQL_RES *res = mysql_store_result(mysqlconn);
		if (result == nullptr)
		{
			return nullptr;
		}

		ResultSet* resSet = new ResultSet(res);
		return resSet;
	}

	//读取数据 
	int MySqlDB::query(const string sqlStr, vector<vector<string> >& data)
	{
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}

		int rows = 0;
		result = mysql_store_result(mysqlconn);
		if (result == nullptr)
		{
			return rows;
		}

		rows = mysql_num_rows(result);
		data.reserve(rows);

		int field = mysql_num_fields(result);
		MYSQL_ROW line = nullptr;
		line = mysql_fetch_row(result);

		string temp;
		while (nullptr != line)
		{
			vector<string> linedata;
			linedata.reserve(field);
			for (int i = 0; i < field; i++)
			{
				if (line[i])
				{
					temp = line[i];
					linedata.push_back(temp);
				}
				else
				{
					temp = "";
					linedata.push_back(temp);
				}
			}
			line = mysql_fetch_row(result);
			data.push_back(linedata);
		}

		if (result != nullptr)
		{
			mysql_free_result(result);
			result = nullptr;
		}

		return rows;
	}

	//读取数据 
	int MySqlDB::query(const string sqlStr, vector<string>& data)
	{
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}

		int rows = 0;
		result = mysql_store_result(mysqlconn);
		if (result == nullptr)
		{
			return rows;
		}

		rows = mysql_num_rows(result);
		int field = mysql_num_fields(result);
		data.reserve(field);

		MYSQL_ROW line = nullptr;
		line = mysql_fetch_row(result);

		string temp;
		if (nullptr != line)
		{
			for (int i = 0; i < field; i++)
			{
				if (line[i])
				{
					temp = line[i];
					data.push_back(temp);
				}
				else
				{
					temp = "";
					data.push_back(temp);
				}
			}
		}

		if (result != nullptr)
		{
			mysql_free_result(result);
			result = nullptr;
		}
		return rows;
	}

	void MySqlDB::execTrans(vector<string> &sqlVec)
	{
		int errCode = 0;
		string errMsg;

		string sqlStr = "start transaction;";
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			errCode = mysql_errno(mysqlconn);
			errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}

		vector<string>::iterator it = sqlVec.begin();
		for (; it != sqlVec.end(); it++)
		{
			if (0 != mysql_real_query(mysqlconn, (*it).c_str(), sqlStr.size()))
			{
				errCode = mysql_errno(mysqlconn);
				errMsg = mysql_error(mysqlconn);
				sqlStr = "rollback;";
				if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
				{
					errCode = mysql_errno(mysqlconn);
					errMsg = mysql_error(mysqlconn);
					throw DBException(errCode, errMsg);
				}
				throw DBException(errCode, errMsg);
			}
		}
		sqlStr = "commit;";
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			errCode = mysql_errno(mysqlconn);
			errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
	}

	//事务控制,在事务过程中可以执行sql的
	void MySqlDB::beginTrans()
	{
		string sqlStr = "start transaction;";
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
	}

	void MySqlDB::commit()
	{
		string sqlStr = "commit;";
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
	}

	void MySqlDB::rollback()
	{
		string sqlStr = "rollback;";
		if (0 != mysql_real_query(mysqlconn, sqlStr.c_str(), sqlStr.size()))
		{
			int errCode = mysql_errno(mysqlconn);
			string errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
	}

	uint64_t MySqlDB::insert(string table, map<string, string> &value)
	{
		int errCode = 0;
		string errMsg;
		uint64_t i = 0;
		string sql = "insert into " + table + "(";
		map<string, string >::iterator it = value.begin();
		while (it != value.end())
		{
			sql += (it->first + ",");
			++it;
			++i;
		}
		sql.erase(sql.length() - 1);
		sql += ") value(";
		MYSQL_BIND bind[i];
		memset(bind, 0, sizeof(bind));
		it = value.begin();
		i = 0;
		while (it != value.end())
		{
			sql += "?,";
			my_bool ia = (it->second).empty() ? 1 : 0;
			bind[i].buffer_type = MYSQL_TYPE_STRING;
			bind[i].buffer = (char *)(it->second).data();
			bind[i].buffer_length = (it->second).length();
			bind[i].is_null = &ia;
			++it;
			++i;
		}
		sql.erase(sql.length() - 1);
		sql += ")";
		i = 0;

		MYSQL_STMT *stmt = mysql_stmt_init(mysqlconn);
		if (stmt == nullptr)
		{
			errCode = mysql_errno(mysqlconn);
			errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
		if (mysql_stmt_prepare(stmt, sql.data(), sql.length()))
		{
			errCode = mysql_stmt_errno(stmt);
			errMsg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw DBException(errCode, errMsg);
		}
		if (mysql_stmt_bind_param(stmt, bind)) {
			errCode = mysql_stmt_errno(stmt);
			errMsg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw DBException(errCode, errMsg);
		}
		if (mysql_stmt_execute(stmt)) {
			errCode = mysql_stmt_errno(stmt);
			errMsg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw DBException(errCode, errMsg);
		}
		uint64_t id = mysql_stmt_insert_id(stmt);
		mysql_stmt_close(stmt);

		return id;
	}

	int MySqlDB::update(string table, map<string, string> &value, string filter)
	{
		int errCode = 0;
		string errMsg;
		int i = 0;
		string sql = "update " + table + " set ";
		map<string, string >::iterator it = value.begin();
		while (it != value.end())
		{
			sql += (it->first + "=?,");
			++it;
			++i;
		}
		sql.erase(sql.length() - 1);
		if (!filter.empty())
		{
			sql += " where " + filter;
		}

		MYSQL_BIND bind[i];
		memset(bind, 0, sizeof(bind));
		it = value.begin();
		i = 0;
		while (it != value.end())
		{
			my_bool ia = (it->second).empty() ? 1 : 0;
			bind[i].buffer_type = MYSQL_TYPE_STRING;
			bind[i].buffer = (char *)(it->second).data();
			bind[i].buffer_length = (it->second).length();
			bind[i].is_null = &ia;
			++it;
			++i;
		}

		MYSQL_STMT *stmt = mysql_stmt_init(mysqlconn);
		if (stmt == nullptr)
		{
			errCode = mysql_errno(mysqlconn);
			errMsg = mysql_error(mysqlconn);
			throw DBException(errCode, errMsg);
		}
		if (mysql_stmt_prepare(stmt, sql.data(), sql.length()))
		{
			errCode = mysql_stmt_errno(stmt);
			errMsg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw DBException(errCode, errMsg);
		}
		if (mysql_stmt_bind_param(stmt, bind))
		{
			errCode = mysql_stmt_errno(stmt);
			errMsg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw DBException(errCode, errMsg);
		}
		if (mysql_stmt_execute(stmt))
		{
			errCode = mysql_stmt_errno(stmt);
			errMsg = mysql_stmt_error(stmt);
			mysql_stmt_close(stmt);
			throw DBException(errCode, errMsg);
		}
		int rows = mysql_affected_rows(mysqlconn);
		mysql_stmt_close(stmt);

		return rows;
	}
}

