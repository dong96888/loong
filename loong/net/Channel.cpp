#include "Channel.h"

namespace loong
{
	Channel::Channel()
	{	
	}

	Channel::Channel(int sockfd):m_sockfd(sockfd)
	{
	}

	void Channel::setReadCallback(function<void()> cb)
	{
		m_readCallback=cb;
	}
	void Channel::setWriteCallback(function<void()> cb)
	{
		m_writeCallback=cb;
	}
	void Channel::setCloseCallback(function<void()> cb)
	{
		m_closeCallback=cb;
	}
	void Channel::setErrCallback(function<void()> cb)
	{
		m_errorCallback=cb;
	}

	void Channel::handleRead()
	{
		if (m_readCallback!=nullptr)
		{
			m_readCallback();
		}
	}
	void Channel::handleWrite()
	{
		if (m_writeCallback!=nullptr)
		{
			m_writeCallback();
		}	
	}
	void Channel::handleClose()
	{
		if (m_closeCallback!=nullptr)
		{
			m_closeCallback();
		}
	}
	void Channel::handleError()
	{
		if (m_errorCallback!=nullptr)
		{
			m_errorCallback();
		}
	}

}










