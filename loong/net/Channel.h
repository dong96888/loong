/*
*   File name: TcpConn.h
*	 Describe:
*  Created on: 2016-3-12
*      Author: dengxiudong
*/


#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>

using namespace std;

namespace loong
{
class Channel
{
public:
	Channel();
    Channel(int sockfd);
    void setSockFd(int sockfd)
    {
        m_sockfd=sockfd;
    }

    void setReadCallback(function<void()> cb);
    void setWriteCallback(function<void()> cb);
    void setCloseCallback(function<void()> cb);
    void setErrCallback(function<void()> cb);

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    int getSockFd()
    {
        return m_sockfd;
    }
private:
    int m_sockfd;

    function<void()> m_readCallback;
    function<void()> m_writeCallback;
    function<void()> m_closeCallback;
    function<void()> m_errorCallback;
};
}

#endif  //

