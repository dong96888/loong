#include "EventLoop.h"

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "Logger.h"
#include "Channel.h"


namespace loong
{
	const int DefaultEventSize = 128;
	const int MaxEventSize = 10240 * 2;
	EventLoop::EventLoop()
	{
		m_pollfd = epoll_create(EPOLL_CLOEXEC);

		if (m_pollfd < 0)
		{
			ERROR("epoll create error %s", strerror(errno));
			exit(-1);
		}

		m_eventSize = DefaultEventSize;
		m_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * m_eventSize);
		memset(m_events, 0, sizeof(struct epoll_event) * m_eventSize);
		m_quit=true;

	}

	EventLoop::~EventLoop()
	{
		free(m_events);
	}

	void EventLoop::loop()
	{
		m_quit=false;
		while (!m_quit)
		{
			int nfds = epoll_wait(m_pollfd, m_events, m_eventSize, -1);
			if (nfds < 0)
			{
				ERROR("epoll_wait error %s", strerror(errno));
				return;
			}

			for (int i = 0; i < nfds; ++i)
			{
				if (m_events[i].events & EPOLLIN)
				{
					Channel* chan=static_cast<Channel*>(m_events[i].data.ptr);
					//printf("have data can read!\n");
					chan->handleRead();
					continue;
				}
				else if (m_events[i].events & EPOLLOUT)
				{
					Channel* chan=static_cast<Channel*>(m_events[i].data.ptr);
					chan->handleWrite();
					continue;
				}
				else if ((m_events[i].events & EPOLLHUP) && (!(m_events[i].events & EPOLLIN)))
				{
					printf("jian ting dao epoll close\n");
					Channel* chan=static_cast<Channel*>(m_events[i].data.ptr);
					chan->handleClose();
					continue;
				}
				else if (m_events[i].events & EPOLLERR)
				{
					printf("jian ting dao epoll error\n");
					Channel* chan=static_cast<Channel*>(m_events[i].data.ptr);
					chan->handleError();
					continue;
				}
			}

			if (nfds == static_cast<int>(m_eventSize) && static_cast<const int>(m_eventSize) != MaxEventSize)
			{
				m_eventSize *= 2;
				if (static_cast<const int>(m_eventSize) > MaxEventSize)
				{
					m_eventSize = MaxEventSize;
				}

				m_events = (struct epoll_event*)realloc(m_events, sizeof(struct epoll_event) * m_eventSize);
			}
		}
		m_quit=true;
	}


	int EventLoop::addEvent(Channel *chan, int events)
	{
		assert(m_pollfd > 0);

		struct epoll_event event;

		event.data.ptr = chan;
		event.events = events;

		int ret = epoll_ctl(m_pollfd, EPOLL_CTL_ADD, chan->getSockFd(), &event);
		if (ret < 0)
		{
			ERROR("epoll_ctl add error %s", strerror(errno));
			return -1;
		}
		return 0;
	}


	int EventLoop::modEvent(Channel *chan, int events)
	{
		assert(m_pollfd > 0);

		struct epoll_event event;

		event.data.ptr = chan;
		event.events = events;

		int ret = epoll_ctl(m_pollfd, EPOLL_CTL_MOD, chan->getSockFd(), &event);
		if (ret < 0)
		{
			ERROR("epoll_ctl add error %s", strerror(errno));
			return -1;
		}
		return 0;
	}


	int EventLoop::delEvent(Channel *chan, int events)
	{
		assert(m_pollfd > 0);

		struct epoll_event event;

		event.data.ptr = chan;
		event.events = events;

		int ret = epoll_ctl(m_pollfd, EPOLL_CTL_DEL, chan->getSockFd(), &event);
		if (ret < 0)
		{
			ERROR("epoll_ctl remove error %s", strerror(errno));
			return -1;
		}

		return 0;
	}

	void EventLoop::quit()
	{
		m_quit=true;
	}

}
