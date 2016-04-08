/*
*   File name: ServiceFactoryBase.h
*	 Describe: ҵ�񹤳����࣬�ⲿ�̳����ҵ�񹤳��ഴ��ҵ�����
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