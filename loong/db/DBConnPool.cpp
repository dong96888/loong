#include "DBConnPool.h"

#include <stdexcept>
#include <exception>
#include <stdio.h>
#include "DBTask.h"



using namespace std;
using namespace sql;


const int MAX_CONN_NUM = 32;

namespace loong
{
	string g_dbip;
	string g_dbName;
	string g_password;

	ConnPool *ConnPool::m_connPool = nullptr;
	//���ӳع��캯��
	ConnPool::ConnPool(string url, string username, string password, int maxsize)
	{
		pthread_mutex_init(&m_lock, nullptr);
		m_maxSize = maxsize;
		m_curSize = 0;
		m_username = username;
		m_password = password;
		m_url = url;
		try
		{
			m_driver = sql::mysql::get_driver_instance();
		}
		catch (sql::SQLException&e)
		{
			printf("sql connection exception: %s", e.what());
		}
		catch (std::runtime_error&e)
		{
			printf("sql connection exception: %s", e.what());
		}
		initConnection(m_maxSize / 2);
	}

	//��ʼ�����ӳأ����������������һ���������
	void ConnPool::initConnection(int initConnNum)
	{
		Connection* conn;
		pthread_mutex_lock(&m_lock);
		for (int i = 0; i < initConnNum; i++)
		{
			conn = createConnection();
			if (conn)
			{
				m_connList.push_back(conn);
				++m_curSize;
			}
			else
			{
				printf("create mysql connection failed!\n");
			}
		}
		pthread_mutex_unlock(&m_lock);
	}

	//��ȡһ�����ӳ�ʵ��
	ConnPool* ConnPool::getInstance()
	{
		if (m_connPool == nullptr)
		{
			char connectName[100];
			sprintf(connectName, "tcp://%s:%d", g_dbip.c_str(), 3306);
			m_connPool = new ConnPool(connectName, g_dbName, g_password, MAX_CONN_NUM);
		}
		return m_connPool;
	}

	int ConnPool::releaseInstance()
	{
		if (m_connPool)
		{
			delete m_connPool;
			m_connPool = nullptr;
		}
		return 0;
	}

	//��������
	Connection* ConnPool::createConnection()
	{
		Connection * conn;
		try
		{
			conn = m_driver->connect(m_url, m_username, m_password);//��������
			return conn;
		}
		catch (sql::SQLException&e)
		{
			printf("GET MYSQL DRIVER INSTANCE FAILED!\n");
			return nullptr;
		}
		catch (std::runtime_error&e)
		{
			printf("RUNTIME ERROR!\n");
			return nullptr;
		}
	}

	//�����ӳ��л�ȡһ������
	Connection* ConnPool::getConnection()
	{
		Connection* conn;
		pthread_mutex_lock(&m_lock);

		if (m_connList.size() > 0) //���ӳ��л�������
		{
			conn = m_connList.front();//�õ���һ������
			m_connList.pop_front();//�Ƴ���һ������
			if (conn->isClosed())
			{
				//��������Ѿ����رգ�ɾ�������½���һ��
				delete conn;
				conn = createConnection();
			}
			if (conn == nullptr)
			{
				--m_curSize;
			}
			pthread_mutex_unlock(&m_lock);
			return conn;
		}
		else
		{
			if (m_curSize < m_maxSize)
			{
				//�����Դ����µ�����
				conn = createConnection();
				if (conn)
				{
					++m_curSize;
					pthread_mutex_unlock(&m_lock);
					return conn;
				}
				else
				{
					pthread_mutex_unlock(&m_lock);
					return nullptr;
				}
			}
			else //�������������Ѿ��ﵽmaxSize
			{
				pthread_mutex_unlock(&m_lock);
				return nullptr;
			}
		}
	}


	//�������ݿ�����
	void ConnPool::releaseConnection(sql::Connection * conn)
	{
		if (conn)
		{
			pthread_mutex_lock(&m_lock);
			m_connList.push_back(conn);
			pthread_mutex_unlock(&m_lock);
		}
	}

	//���ӳص���������
	ConnPool::~ConnPool()
	{
		destoryConnPool();
		pthread_mutex_destroy(&m_lock);
	}

	//�������ӳ�,����Ҫ�������ӳ��е�����
	void ConnPool::destoryConnPool()
	{
		list<Connection*>::iterator ite;
		pthread_mutex_lock(&m_lock);
		for (ite = m_connList.begin(); ite != m_connList.end(); ++ite)
		{
			destoryConnection(*ite);
		}
		m_curSize = 0;
		m_connList.clear();
		pthread_mutex_unlock(&m_lock);
	}

	//����һ������
	void ConnPool::destoryConnection(Connection* conn)
	{
		if (conn)
		{
			try
			{
				conn->close();
			}
			catch (sql::SQLException&e)
			{
				printf("[ERR]CLOSE CONNECTION ERROR!\n");
			}
			catch (std::exception&e)
			{
				printf("[ERR]DESTORY CONNECTION ERROR!\n");
			}

			delete conn;
		}
	}
}

