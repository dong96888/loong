/*
*   File name: RecvMessage.h
*	 Describe: ҵ����ӿ��࣬��Ҫʵ��һ��ҵ����ʱֻҪ�̳����
			   ����࣬ʵ�ֽӿ�process��ע�ᵽҵ����map�оͿ���
*  Created on: 2015-3-3
*      Author: dengxiudong
*/


#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "BaseService.h"
#include "TcpConn.h"

#include <stdint.h>

class Service :public BaseService
{
public:
	Service();
	~Service();

	virtual void process(TcpConn * conn, MsgHead * msgHead, char * buf, uint16_t len);
};



#endif