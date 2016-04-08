/*
*   File name: RecvMessage.h
*	 Describe: 创建和保存业务对象
*  Created on: 2015-3-3
*      Author: dengxiudong
*/


#ifndef _SERVICE_FACTORY_H_
#define _SERVICE_FACTORY_H_

#include "BaseService.h"

#include "ServiceFactoryBase.h"

#include <map>

#include <stdint.h>


using namespace std;


class ServiceFactory :public ServiceFactoryBase
{
public:
	ServiceFactory();
	~ServiceFactory();

	void createObject();

	BaseService * getObject(uint16_t cmd);

private:
	map<uint16_t, BaseService*> m_objMap;

};






#endif