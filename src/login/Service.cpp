#include "Service.h"

#include "AsynLog.h"
#include "SendMessage.h"
#include "TcpConn.h"
#include "AsynLog.h"

#include <string.h>
#include <iostream>

#include <netinet/in.h>

using namespace std;



Service::Service()
{
}


Service::~Service()
{
}

void Service::process(TcpConn *conn, MsgHead *msgHead, char *buf, uint16_t len)
{
	if (buf == nullptr)
	{
		return;
	}

	char respMsg[1024] = { "0" };
	char tmpbuf[] = { "response message!response message!response message!" };

	///////////////打包消息头和消息体///////////////////
	MsgHead msgH;
	msgH.cmd = msgHead->cmd;
	int totalLen = sizeof(MsgHead) + strlen(tmpbuf);
	msgH.len = htons(totalLen);
	memcpy(respMsg, (int8_t*)&msgH, sizeof(MsgHead));
	memcpy(respMsg + sizeof(MsgHead), tmpbuf, strlen(tmpbuf));
	////////////////////////////////////////////////////
	//发送消息
	DEBUG("buf len: %ld", strlen(tmpbuf));
	DEBUG("msg head len: %ld", sizeof(MsgHead));
	DEBUG("total msg len: %ld", totalLen);
	conn->sendMsg(1, respMsg, totalLen);
}

