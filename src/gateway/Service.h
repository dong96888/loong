/*
*   File name: RecvMessage.h
*	 Describe: 业务处理接口类，当要实现一个业务处理时只要继承这个
			   这个类，实现接口process后注册到业务处理map中就可以
*  Created on: 2015-3-3
*      Author: dengxiudong
*/


#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "BaseService.h"

#include <stdint.h>

class Service :public BaseService
{
public:
	Service();
	~Service();

	virtual void process(int sockfd, uint16_t cmd, char * buf, uint16_t len);
};



#endif