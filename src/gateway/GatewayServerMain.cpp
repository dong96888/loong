#include "TcpServer.h"
#include "AsynLog.h"
#include "CfgFile.h"
#include "AsynLog.h"
#include "ServiceFactory.h"
#include "ConnectManager.h"

#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/resource.h>


static int g_threadNum = 0;
static int g_port = 0;
static string g_dbproxy_ip;
static int g_dbproxy_port = 0;

//可以最大文件描述符
const int MAX_FILE_HANDLE = 10000 * 60;

bool readConfig()
{
	CfgFile cfg("./gateway.conf");

	if (!cfg.getIntValue("thread_num", g_threadNum))
	{
		ERROR("read threadnum failed!");
		return false;
	}
	INFO("threadnum: %d", g_threadNum);

	if (!cfg.getIntValue("port", g_port))
	{
		ERROR("read port failed!");
		return false;
	}
	INFO("port: %d", g_port);

	if (!cfg.getStrValue("trade_server_ip", g_dbproxy_ip))
	{
		ERROR("read port failed!");
		return false;
	}
	INFO("port: %s", g_dbproxy_ip.c_str());

	if (!cfg.getIntValue("trade_server_port", g_dbproxy_port))
	{
		ERROR("read port failed!");
		return false;
	}
	INFO("port: %d", g_dbproxy_port);

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
		ERROR("set virtual memory ulimit failed:%s", strerror(errno));
		exit(-1);
	}

	//不对资源使用进行限制
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	//RLIMIT_CORE产生core文件
	if (setrlimit(RLIMIT_CORE, &rlim) < 0)
	{
		ERROR("set core ulimit failed:%s", strerror(errno));
		exit(-1);
	}

	rlim.rlim_cur = MAX_FILE_HANDLE;
	rlim.rlim_max = MAX_FILE_HANDLE;
	//设置对打开的最大文件描叙符不做限制
	if (setrlimit(RLIMIT_NOFILE, &rlim) < 0)
	{
		ERROR("set max file handle ulimit failed:%s", strerror(errno));
		exit(-1);
	}

	//后台运行且不屏蔽输出
	daemon(1, 1);
}


int main()
{
	daemonInit();

	if (!AsynLog::getInstance()->openFile())
	{
		ERROR("open file failed");
		return -1;
	}
	//启动配置文件线程
	AsynLog::getInstance()->start();

	if (!readConfig())
	{
		ERROR("read config error!\n");
		return -1;
	}

	ServiceFactoryBase *serviceFactory = new ServiceFactory();
	//启动服务器
	TcpServer server((uint16_t)g_port, g_threadNum);
	////////////
	//启动连接数据库代理
	ConnectManager conn(&server);
	if (!conn.connTreadeServer(g_dbproxy_ip, g_dbproxy_port))
	{
		ERROR("connect trade server failed!\n");
		return -1;
	}
	////////////
	if (!server.start(serviceFactory))
	{
		ERROR("start server failed!\n");
		return -1;
	}
	delete serviceFactory;
	return 0;
}