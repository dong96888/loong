#include "TcpServer.h"
#include "Logger.h"
#include "CfgFile.h"
#include "Global.h"
#include "MySqlDB.h"
#include "DBException.h"
#include "TcpConn.h"

#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <unordered_map>
#include <mutex>

using namespace std;

using namespace loong;


static int g_threadNum = 0;
static int g_port = 0;

//可以最大文件描述符
const int MAX_FILE_HANDLE = 10000 * 60;



bool readConfig()
{
	CfgFile cfg("./login.conf");

	if (!cfg.getIntValue("thread_num", g_threadNum))
	{
		ERROR("read threadnum failed!");
		return false;
	}
	INFO("threadnum: %d", g_threadNum);

	/*
	string ip;
	if (!cfg.getStrValue("ip", ip))
	{
		cout << "read ip failed!" << endl;
		return false;
	}
	cout << "ip: " << ip << endl;
	*/

	if (!cfg.getIntValue("port", g_port))
	{
		ERROR("read port failed!");
		return false;
	}
	INFO("port: %d", g_port);

	return true;
}


void daemonInit()
{
	signal(SIGALRM, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	//忽略ctrl + c操作
	signal(SIGINT, SIG_IGN);

	//设置最大虚拟内存为2GB.
	size_t kOneGB = 1000 * 1024 * 1024;
	struct rlimit rlim = { 2 * kOneGB, 2 * kOneGB };
	if (setrlimit(RLIMIT_AS, &rlim) < 0)
	{
		printf("set virtual memory ulimit failed:%s", strerror(errno));
		exit(-1);
	}

	//不对资源使用进行限制
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	//RLIMIT_CORE产生core文件
	if (setrlimit(RLIMIT_CORE, &rlim) < 0)
	{
		printf("set core ulimit failed:%s", strerror(errno));
		exit(-1);
	}

	rlim.rlim_cur = MAX_FILE_HANDLE;
	rlim.rlim_max = MAX_FILE_HANDLE;
	//设置对打开的最大文件描叙符不做限制
	if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
	{
		printf("set max file handle ulimit failed:%s", strerror(errno));
		exit(-1);
	}

	//后台运行且不屏蔽输出
	daemon(1, 1);
}

const static int kHeaderLen = sizeof(int16_t);
const static int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
const static int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimit


void onMessage(shared_ptr<TcpConn> conn, Buffer* buf)
{
	DEBUG("read sockfd is: %d\n", conn->getSocket());
	//printf("read sockfd is: %d\n", conn->getSocket());
	while (buf->readableBytes() >= kMinMessageLen + kHeaderLen)
	{
		const int16_t len = buf->peekInt16();

		if (len > kMaxMessageLen || len < kMinMessageLen)
		{
			conn->close();
			break;
		}
		else if (buf->readableBytes() >= len + kHeaderLen)
		{
			//char msg[100]={'\0'};
			//memcpy(msg,buf->peek(),len);
			//conn->sendData((void *)buf->peek(),buf->readableBytes());
			//buf->retrieveAll();
			conn->sendData((void *)buf->peek(),kHeaderLen+len);
			//printf("send len: %d\n",kHeaderLen+len);
			buf->retrieve(kHeaderLen+len);
		}
		else
		{
			break;
		}
	}
}

unordered_map<int,shared_ptr<TcpConn>> g_connlist;
mutex g_connMtx;
void onConn(shared_ptr<TcpConn> conn)
{
	DEBUG("have new client connect server!");
	g_connMtx.lock();
	g_connlist.insert(pair<int,shared_ptr<TcpConn>>(conn->getSocket(),conn));
	g_connMtx.unlock();
	return;
}

void onClose(shared_ptr<TcpConn> conn)
{
	DEBUG("have client close connect!");
	g_connMtx.lock();
	g_connlist.erase(conn->getSocket());
	g_connMtx.unlock();
	return;
}

void onError(shared_ptr<TcpConn> conn)
{
	DEBUG("have client close connect or network error!");
	g_connMtx.lock();
	g_connlist.erase(conn->getSocket());
	g_connMtx.unlock();
	return;
}


int main()
{
	daemonInit();

	if (!readConfig())
	{
		printf("read config error!\n");
		return -1;
	}
	/*
	if (!AsynLog::getInstance().openFile())
	{
		ERROR("open file failed");
		return -1;
	}
	//启动配置文件线程
	AsynLog::getInstance().start();
*/
#if 0
	//数据库连接池初始化
	try{
		g_sqlPool=new MySqlPool(9);
	}catch(exception &e)
	{
		printf("create mysqlpool object failed: %s", e.what());
	}catch(...)
	{
		printf("create mysqlpool object failed\n");
	}
	string errMsg;
	if (0!=g_sqlPool->create("172.16.7.251",3306,"ryt2014","ryt2014","trader",errMsg))
	{
		printf("%s,init mysqldb connect pool failed!\n",errMsg.c_str());
	}
	INFO("init mysqldb connetct pool success;");
	///////////////////////////////////////////////
	//test mysqldb
	MySqlDB sqldb(g_sqlPool);
	//string sql="update test set c=999 where a=66;";
	string sql="insert into test(b,c) values(99,999);";
	//string sql="select * from test;";
	try{
		//vector<string> data;
		//sqldb.execSQL(sql);
		//int rows=sqldb.update(sql);
		//int rows=sqldb.query(sql, data);
		//int rows=sqldb.insert(sql);
		map<string,string> data;
		char str[20]={'\0'};
		sprintf(str,"%d",88);
		data["b"]=str;
		sprintf(str,"%d",99);
		data["c"]=str;
		int rows = sqldb.update("test", data, "b=6");
		INFO("rows: %d", rows);
		/*for(int i=0;i<rows;i++)
		{
			printf("data[%d]: %s\n", i, data[i].c_str());
		}*/
	}catch(DBException &e)
	{
		ERROR("sqlerr: %s", e.getMsg().c_str());
	}
#endif
	////////////////////////


	//启动服务器
	TcpServer ts((uint16_t)g_port);

	ts.setMessageCallback(std::bind(&onMessage, std::placeholders::_1,std::placeholders::_2));
	ts.setConnCallback(std::bind(&onConn, std::placeholders::_1));
	ts.setCloseCallback(std::bind(&onClose, std::placeholders::_1));
	ts.setErrCallback(std::bind(&onError, std::placeholders::_1));

	//g_threadNum=0;
	if (!ts.start(g_threadNum))
	{
		ERROR("start server failed!\n");
		return -1;
	}
	//delete serviceFactory;
	return 0;
}
