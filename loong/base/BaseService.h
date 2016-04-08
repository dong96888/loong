/*
*   File name: RecvMessage.h
*	 Describe: ҵ����ӿ��࣬��Ҫʵ��һ��ҵ����ʱֻҪ�̳����
			   ����࣬ʵ�ֽӿ�process��ע�ᵽҵ����map�оͿ���
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