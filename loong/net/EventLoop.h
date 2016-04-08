#ifndef _EVENT_LOOP_
#define _EVENT_LOOP_


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>



namespace loong
{
class Channel;
class EventLoop
{
    public:
		EventLoop();
		~EventLoop();
        void loop();
        int addEvent(Channel *chan, int events);
        int modEvent(Channel *chan, int events);
        int delEvent(Channel *chan, int events);
		void quit();

private:
    int m_pollfd;
    bool m_quit;
	
	size_t m_eventSize;
	struct epoll_event* m_events;


};
}
#endif

