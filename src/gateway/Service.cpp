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

//ҵ����ӿں��������������඼����ʵ���������
void Service::process(int sockfd, uint16_t cmd, char *buf, uint16_t len)
{
	if (buf == NULL)
	{
		return;
	}
	cout << "����ǳ���ӿڣ���������ɴ��麯��" << endl;
	DEBUG("����ǳ���ӿڣ���������ɴ��麯��");
	char respMsg[1024] = "response message response message response message!";

	//������Ϣ
	SendMessage::sendMsg(sockfd, cmd, respMsg, strlen(respMsg));
}
