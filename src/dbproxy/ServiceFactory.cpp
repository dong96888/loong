#include "ServiceFactory.h"

#include "BaseService.h"

#include <iterator>

#include <stdint.h>




const uint16_t LOGIN = 0x0001;

ServiceFactory::ServiceFactory()
{
}

ServiceFactory::~ServiceFactory()
{
	map<uint16_t, BaseService*>::iterator ite = m_objMap.begin();
	for (; ite != m_objMap.end(); ite++)
	{
		delete (*ite).second;
	}
}

//创建所有需要执行的业务对象
void ServiceFactory::createObject()
{
	m_objMap[LOGIN] = new BaseService;
}

//获取对应消息的处理对象
BaseService *ServiceFactory::getObject(uint16_t cmd)
{
	map<uint16_t, BaseService*>::iterator ite = m_objMap.find(cmd);
	if (ite != m_objMap.end())
	{
		return ite->second;
	}

	return NULL;
}