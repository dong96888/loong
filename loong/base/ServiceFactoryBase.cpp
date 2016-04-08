#include "ServiceFactoryBase.h"

#include "BaseService.h"

#include <iterator>

#include <stdint.h>



namespace loong
{
	ServiceFactoryBase *g_serviceFactory = nullptr;
	ServiceFactoryBase::ServiceFactoryBase()
	{
	}

	ServiceFactoryBase::~ServiceFactoryBase()
	{
		map<uint16_t, BaseService*>::iterator ite = m_objMap.begin();
		for (; ite != m_objMap.end(); ite++)
		{
			delete (*ite).second;
		}
	}

#if 0
	//创建所有需要执行的业务对象
	void ServiceFactoryBase::createObject()
	{
		m_objMap[LOGIN] = new BaseService;
	}
#endif

	//获取对应消息的处理对象
	BaseService *ServiceFactoryBase::getObject(uint16_t cmd)
	{
		map<uint16_t, BaseService*>::iterator ite = m_objMap.find(cmd);
		if (ite != m_objMap.end())
		{
			return ite->second;
		}

		return nullptr;
	}
}
