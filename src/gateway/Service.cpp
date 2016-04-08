#include "Service.h"

#include "AsynLog.h"
#include "SendMessage.h"
#include "string.h"


#include <iostream>

using namespace std;



Service::Service()
{
}


Service::~Service()
{
}

//业务类接口函数，所有派生类都必须实现这个函数
void Service::process(int sockfd, uint16_t cmd, char *buf, uint16_t len)
{
	if (buf == NULL)
	{
		return;
	}
	cout << "这个是抽象接口，将来定义成纯虚函数" << endl;
	DEBUG("这个是抽象接口，将来定义成纯虚函数");
	char respMsg[1024] = "response message response message response message!";

	//发送消息
	SendMessage::sendMsg(sockfd, cmd, respMsg, strlen(respMsg));
}
