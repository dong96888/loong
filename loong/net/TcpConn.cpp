/*
*   File name: TcpConn.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#include "TcpConn.h"

#include "Logger.h"
#include "BufferPool.h"
#include "Channel.h"

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>



namespace loong
{
	TcpConn::TcpConn(EventLoop *loop,int sockfd)
	:m_loop(loop),m_sockfd(sockfd),m_chan(new Channel(m_sockfd))
	{
		//printf("loop addr %x:%x\n", loop,m_loop);
		//printf("socket fd :%d\n", sockfd);
	}

	TcpConn::~TcpConn()
	{
		::close(m_sockfd);
		delete m_chan;
	}

	int TcpConn::getSocket()
	{
		return m_sockfd;
	}

	void TcpConn::onEstablished()
	{
		m_chan->setReadCallback(bind(&TcpConn::handleRead,this));
		m_chan->setWriteCallback(bind(&TcpConn::handleWrite,this));
		m_chan->setCloseCallback(bind(&TcpConn::handleClose,this));
		m_chan->setErrCallback(bind(&TcpConn::handleError,this));

		m_loop->addEvent(m_chan,EPOLLET | EPOLLIN | EPOLLOUT|EPOLLHUP|EPOLLERR);
	}


	void TcpConn::handleRead()
	{
		//printf("begin handleRead data!\n");
		int err=0;
		while(true)
		{
			ssize_t n = m_recvBuf.readNetData(m_sockfd, &err);
			if (n > 0)
			{
				if (err==0)
				{
					m_messageCallback(shared_from_this(), &m_recvBuf);
					return;
				}
				else //err==1 可能还有数据没有读完
				{
					m_messageCallback(shared_from_this(), &m_recvBuf);
					continue;
				}
			}
			else if (n == 0)
			{
				printf("read colse\n");
				handleClose();
				return;
			}
			else
			{
				if (err == EINTR)
				{
					continue;
				}
				else if (err == EAGAIN || errno == EWOULDBLOCK)
				{
					return;
				}
				else
				{
					//printf("read error!\n");
					handleError();
					return;
				}
			}
		}
		//printf("end handleRead data!\n");
	}

	void TcpConn::handleWrite()
	{
		//printf("begin handleWrite data!\n");
		if (m_sendBuf.readableBytes()<=0)
		{
			//printf("have not data can write\n");
			return;
		}
		while (m_sendBuf.readableBytes()>0)
		{
			ssize_t n = write(m_sockfd,m_sendBuf.peek(),m_sendBuf.readableBytes());
			if (n > 0)
			{
				m_sendBuf.retrieve(n);
			}
			else if(n==0)
			{
				//printf("connect close!\n");
				handleClose();
			}
			else
			{
				int err = errno;
				if(err == EAGAIN || err == EWOULDBLOCK)
				{
					return;
				}
				else
				{
					ERROR("read error!\n");
					handleError();
					return;
				}
			}
		}
		//printf("end handleRead data!\n");
	}

	void TcpConn::sendData(const void *data, size_t len)
	{
		//printf("send sockfd is: %d\n", m_sockfd);
		size_t oldlen=m_sendBuf.readableBytes();
		if(oldlen==0 && len==0)
		{
			return;
		}

		while(true)
		{
			size_t totalSize = oldlen + len;

			int niov = 2;
			struct iovec iov[niov];
			iov[0].iov_base = (void*)m_sendBuf.peek();
			iov[0].iov_len = oldlen;

			iov[1].iov_base = (void*)data;
			iov[1].iov_len = len;

			ssize_t n = writev(m_sockfd, iov, niov);
			if(n == totalSize)
			{
				m_sendBuf.retrieveAll();
				return;
			}
			else if(n <= 0)
			{
				if (n==0)
				{
					handleClose();
					return;
				}
				else //<0
				{
					int err = errno;
					if(err == EAGAIN || err == EWOULDBLOCK)
					{
						m_sendBuf.append(data, len);
						return;
					}
					else
					{
						ERROR("socket error: %s\n",strerror(errno));
						handleError();
						return;
					}
				}
			}
			else  //0<n<totalSize
			{
				if( n>oldlen)
				{
					m_sendBuf.retrieveAll();
					size_t tmp = n-oldlen;
					size_t len = len-tmp;
					oldlen=0;
					data=data+tmp;
				}
				else //0<n<oldlen
				{
					m_sendBuf.retrieve(n);
					oldlen=m_sendBuf.readableBytes();
				}
			}
		}
	}

	void TcpConn::handleClose()
	{
		m_loop->delEvent(m_chan,EPOLLET|EPOLLOUT|EPOLLHUP|EPOLLERR);
		//必须放到最后
		if (m_closeCallback!=nullptr)
		{
			m_closeCallback(shared_from_this());
		}

		//delete this;
	}

	void TcpConn::handleError()
	{
		m_loop->delEvent(m_chan,EPOLLET|EPOLLIN|EPOLLOUT|EPOLLHUP|EPOLLERR);
		if (m_errCallback!=nullptr)
		{
			m_errCallback(shared_from_this());
		}

		//delete this;
	}

	void TcpConn::close()
	{
		handleClose();
	}

}
