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
	//����������Ҫִ�е�ҵ�����
	void ServiceFactoryBase::createObject()
	{
		m_objMap[LOGIN] = new BaseService;
	}
#endif

	//��ȡ��Ӧ��Ϣ�Ĵ������
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
