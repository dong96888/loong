#include "Buffer.h"


#include <errno.h>
#include <sys/uio.h>

namespace loong
{
	const char Buffer::kCRLF[] = "\r\n";

	const size_t Buffer::kInitialSize;

	ssize_t Buffer::readNetData(int fd, int* err)
	{
		// saved an ioctl()/FIONREAD call to tell how much to read
		char extrabuf[65536];
		struct iovec vec[2];
		const size_t writable = writableBytes();
		const size_t exbuflen = sizeof extrabuf;
		vec[0].iov_base = begin()+m_wIndex;
		vec[0].iov_len = writable;
		vec[1].iov_base = extrabuf;
		vec[1].iov_len = exbuflen;
		// when there is enough space in this buffer, don't read into extrabuf.
		// when extrabuf is used, we read 128k-1 bytes at most.
		const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
		const ssize_t n = readv(fd, vec, iovcnt);
		if (n < 0)
		{
			*err = errno;
		}
		else if (n <= writable)
		{
			m_wIndex += n;
		}
		else
		{
			if (n==writable+exbuflen)
			{
				//需要再次读取数据
				*err = 1;
			}
			m_wIndex = m_size;
			append(extrabuf, n - writable);
		}

		return n;
	}

}

