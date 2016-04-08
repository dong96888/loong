#include "ServiceFactory.h"

#include "Service.h"
#include "Cmd.h"

#include <iterator>

#include <stdint.h>



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

//����������Ҫִ�е�ҵ�����
void ServiceFactory::createObject()
{
	m_objMap[LOGIN] = new Service();
}

//��ȡ��Ӧ��Ϣ�Ĵ������
BaseService *ServiceFactory::getObject(uint16_t cmd)
{
	map<uint16_t, BaseService*>::iterator ite = m_objMap.find(cmd);
	if (ite != m_objMap.end())
	{
		return ite->second;
	}

	return NULL;
}