/*
*   File name: RecvMessage.h
*	 Describe: 业务处理接口类，当要实现一个业务处理时只要继承这个
			   这个类，实现接口process后注册到业务处理map中就可以
*  Created on: 2015-3-3
*      Author: dengxiudong
*/


#ifndef _BASE_SERVICE_H_
#define _BASE_SERVICE_H_

#include <stdint.h>

namespace loong
{
	class TcpConn;
	struct MsgHead;
	class BaseService
	{
	public:
		BaseService();
		virtual ~BaseService();

		virtual void process(TcpConn * conn, MsgHead * msgHead, char * buf, uint16_t len) = 0;
	};
}


#endif