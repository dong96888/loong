/*
*   File name: ServiceFactoryBase.h
*	 Describe: 业务工厂基类，外部继承这个业务工厂类创建业务对象
*  Created on: 2015-3-3
*      Author: dengxiudong
*/


#ifndef _SERVICE_FACTORY_BASE_H_
#define _SERVICE_FACTORY_BASE_H_

#include "BaseService.h"

#include <map>

#include <stdint.h>


using namespace std;

namespace loong
{
	class ServiceFactoryBase
	{
	public:
		ServiceFactoryBase();
		virtual ~ServiceFactoryBase();

		virtual void createObject() = 0;

		virtual BaseService * getObject(uint16_t cmd);

	protected:
		map<uint16_t, BaseService*> m_objMap;

	};
	extern ServiceFactoryBase *g_serviceFactory;
}


#endif