#include "DBTask.h"

#include "AsynLog.h"


#include <string>

using namespace std;

namespace loong
{
	DBTask::DBTask(string dbip, string userName, string password, string dbName)
	{
		sprintf(m_connUrl, "tcp://%s:%d", dbip.c_str(), 3306);

		m_userName = userName;
		m_password = password;
		m_dbName = dbName;
	}

	DBTask::~DBTask()
	{
		if (m_dbConn != nullptr)
		{
			m_dbConn->close();
			delete m_dbConn;
		}
	}

	Connection *DBTask::createConnetion()
	{
		try
		{
			m_driver = sql::mysql::get_driver_instance();
			m_dbConn = m_driver->connect(m_connUrl, m_userName, m_password);//建立连接
		}
		catch (sql::SQLException&e)
		{
			ERROR("sql connection exception: %s", e.what());
			return nullptr;
		}
		catch (std::runtime_error&e)
		{
			ERROR("sql connection exception: %s", e.what());
			return nullptr;
		}

		return m_dbConn;
	}

	void DBTask::execTask(string sql)
	{
		if (m_dbConn->isClosed())
		{
			while (true)
			{
				if (createConnetion() == nullptr)
				{
					sleep(3);
				}
				else
				{
					break;
				}
			}
		}

		try
		{
			Statement *state = m_dbConn->createStatement();
			char useDb[16] = { '\0' };
			snprintf(useDb, 15, "use %s", m_dbName.c_str());
			state->execute(useDb);
			state->execute(sql);
			delete state;
		}
		catch (sql::SQLException &e)
		{
			ERROR("sql error:[%s]; sql:[%s]", e.what(), sql.c_str());
		}
		catch (...)
		{
			ERROR("error");
		}
	}

	void DBTask::addTask(string sql)
	{
		m_cond.lock();
		m_taskList.push_back(sql);
		m_cond.signal();
	}

	void DBTask::run()
	{
		if (createConnetion() == nullptr)
		{
			ERROR("connect database failed!");
			return;
		}

		list<string> localTask;
		while (true)
		{
			m_cond.lock();
			while (m_taskList.empty())
			{
				m_cond.wait();
			}
			localTask.swap(m_taskList);
			m_cond.unlock();

			list<string>::iterator it = localTask.begin();
			for (; it != localTask.end(); it++)
			{
				execTask(*it);
			}
		}
	}
}
