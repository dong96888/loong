#include "TcpClient.h"

#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <functional>

using namespace loong;

const static int kHeaderLen = sizeof(int16_t);
const static int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
const static int kMaxMessageLen = 64*1024*1024;

void onMessage(shared_ptr<TcpConn> conn, Buffer *buf)
{
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
			printf("recv message: %s\n", buf->peek()+2);
			buf->retrieve(kHeaderLen+len);
		}
		else
		{
			break;
		}
	}
}

void onConn(shared_ptr<TcpConn> conn)
{
	char buf[]="hello world!";
	char bytes[100];
	short len=strlen(buf);
	short lenn=htons(len);
	memcpy(bytes,(char*)&lenn,2);
	memcpy(bytes+2,buf,strlen(buf));
	
	conn->sendData(bytes,strlen(buf)+2);
}

#if 0
int main()
{
	EventLoop loop;
	TcpClient client(&loop);
	
	client.setMessageCallback(onMessage);
	client.setConnCallback(onConn);
	
	client.connect("10.240.4.247",8998);
	
	loop.loop();
	
	/*
	char buf[]="hello world!";
	char bytes[100];
	short len=strlen(buf);
	short lenn=htons(len);
	memcpy(bytes,(char*)&lenn,2);
	memcpy(bytes+2,buf,strlen(buf));
	
	conn->sendData(bytes,strlen(buf)+2);
	*/
}
#endif


#if 1 //连接池版
int main()
{
	EventLoop loop;
	vector<TcpClient*> cliVec;
	
	for(int i=0;i<20000;i++)
	{
		TcpClient *client=new TcpClient(&loop);
		client->setMessageCallback(onMessage);
		client->setConnCallback(onConn);
		
		client->connect("10.240.4.247",8998);
		cliVec.push_back(client);
	}
	
	loop.loop();
	getchar();
}
#endif
