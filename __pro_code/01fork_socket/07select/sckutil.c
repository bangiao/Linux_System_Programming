#include "sckutil.h"

/* read函数的调用方法
int ret;
ret = read_timeout(fd, 5);
if (ret == 0)
{
	read(fd, ...);
}
else if (ret == -1 && errno == ETIMEDOUT)
{
	timeout....
}
else
{
	ERR_EXIT("read_timeout");
}
*/

/**
 * read_timeout - 读超时检测函数，不含读操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int read_timeout(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds <= 0)
	{
		printf("func write_timeoutif if (wait_sec < 0)");
		return -1;
	}

	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(fd, &writefds);
	struct timeval timeout;
	timeout.tv_sec = wait_seconds;
	timeout.tv_usec = 0;
	do
	{
		ret = select(fd + 1, NULL, &writefds, NULL, &timeout);
	}
	while (ret < 0 && errno == EINTR);
	if (ret < 0)
	{
		printf("func write_timeout if (ret < 0) err \n");
		return -1;
	}
	else if (0 == ret)
	{
		printf("func write_timeout else if (0 == ret) is timeout err \n");
		return -1;
	}
	else if (ret > 0)
	{
		ret = 0;
		return ret;
	}
}

/**
 * write_timeout - 写超时检测函数，不含写操作
 * @fd: 文件描述符
 * @wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int write_timeout(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set write_fdset;
		struct timeval timeout;

		FD_ZERO(&write_fdset);
		FD_SET(fd, &write_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}

	return ret;
}

/**
 * accept_timeout - 带超时的accept
 * @fd: 套接字
 * @addr: 输出参数，返回对方地址
 * @wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回已连接套接字，超时返回-1并且errno = ETIMEDOUT
 */
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds < 0)
	{
		ERR_EXIT("accept_timeout  if (wait_sec < 0)");
	}
	socklen_t addrlen = sizeof(struct sockaddr_in);
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	struct timeval timeout;
	timeout.tv_sec = wait_seconds;
	timeout.tv_usec = 0;
	do
	{
		ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
	}
	while (ret < 0 && ret == EINTR);
	if (ret < 0)
	{
		ERR_EXIT("accept_timeout  if (-1 == ret)");
	}
	else if (0 == ret)
	{
		ERR_EXIT("accept_timeout  else if (0 == ret)");
	}
	if (NULL != addr)
	{
		ret = accept(fd, (struct sockaddr *) addr, &addrlen);
	}
	else
	{
		ret = accept(fd, NULL, NULL);
	}
	return ret;
}

/**
 * activate_noblock - 设置I/O为非阻塞模式
 * @fd: 文件描符符
 */
void activate_nonblock(int fd)
{
	int ret;
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		ERR_EXIT("fcntl");

	flags |= O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
		ERR_EXIT("fcntl");
}

/**
 * deactivate_nonblock - 设置I/O为阻塞模式
 * @fd: 文件描符符
 */
void deactivate_nonblock(int fd)
{
	int ret;
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		ERR_EXIT("fcntl");

	flags &= ~O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
		ERR_EXIT("fcntl");
}


/**
 * connect_timeout - connect
 * @fd: 套接字
 * @addr: 要连接的对方地址
 * @wait_seconds: 等待超时秒数，如果为0表示正常模式
 * 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
 */
int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if (wait_seconds > 0)
		activate_nonblock(fd);

	ret = connect(fd, (struct sockaddr*)addr, addrlen);
	if (ret < 0 && errno == EINPROGRESS)
	{
		//printf("11111111111111111111\n");
		fd_set connect_fdset;
		struct timeval timeout;
		FD_ZERO(&connect_fdset);
		FD_SET(fd, &connect_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			// 一但连接建立，则套接字就可写  所以connect_fdset放在了写集合中
			ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret < 0)
			return -1;
		else if (ret == 1)
		{
			//printf("22222222222222222\n");
			/* ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
			/* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
			int err;
			socklen_t socklen = sizeof(err);
			int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
			if (sockoptret == -1)
			{
				return -1;
			}
			if (err == 0)
			{
				//printf("3333333333333\n");
				ret = 0;
			}
			else
			{
				//printf("4444444444444444:%d\n", err);
				errno = err;
				ret = -1;
			}
		}
	}
	if (wait_seconds > 0)
	{
		deactivate_nonblock(fd);
	}
	return ret;
}

/**
 * readn - 读取固定字节数
 * @fd: 文件描述符
 * @buf: 接收缓冲区
 * @count: 要读取的字节数
 * 成功返回count，失败返回-1，读到EOF返回<count
 */
ssize_t readn(int fd, void *buf, size_t count)
{
	size_t nmaxlen = count;
	size_t nreadlen = 0;
	char *pbuf = buf;

	if (NULL == pbuf)
	{
		ERR_EXIT("func err readn if (NULL == buf)");
	}
	while (nmaxlen > 0)
	{
		nreadlen = read(fd, pbuf, nmaxlen);
		if (nreadlen < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			ERR_EXIT("func err readn if (nreadlen < 0)");
			return -1;
		}
		else if (0 == nreadlen)
		{
			return count - nmaxlen;
		}
		pbuf += nreadlen;
		nmaxlen -= nreadlen;
	}
	return count;
}

/**
 * writen - 发送固定字节数
 * @fd: 文件描述符
 * @buf: 发送缓冲区
 * @count: 要读取的字节数
 * 成功返回count，失败返回-1
 */
ssize_t writen(int fd, const void *buf, size_t count)
{
	ssize_t nmaxlen = count;
	ssize_t writelen = 0;
	char *pbuf = (char *)buf;
	if (NULL == buf)
	{
		ERR_EXIT("func err write if (NULL == buf) ");
	}
	if (count <= 0)
	{
		ERR_EXIT("func err write if (count <= 0)");
	}
	while (nmaxlen > 0)
	{
		if ((writelen = write(fd, pbuf, nmaxlen)) < 0)
		{
			if (writelen < 0 && errno == EINTR)
			{
				continue;
			}
			ERR_EXIT("func err write if ((writelen = write(connfd, pbuf, nmaxlen)) < 0)");
			return -1;
		}
		else if (0 == writelen)
		{
			continue;
		}
		pbuf += writelen;
		nmaxlen -= writelen;
	}
	return count;
}

/**
 * recv_peek - 仅仅查看套接字缓冲区数据，但不移除数据
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @len: 长度
 * 成功返回>=0，失败返回-1
 */
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
	while (1)
	{
		int ret = recv(sockfd, buf, len, MSG_PEEK);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

/**
 * readline - 按行读取数据
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @maxline: 每行最大长度
 * 成功返回>=0，失败返回-1
 */
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
	int ret;
	int nread;
	char *bufp = buf;
	int nleft = maxline;
	while (1)
	{
		ret = recv_peek(sockfd, bufp, nleft);
		if (ret < 0)
			return ret;
		else if (ret == 0) // 关机了
			return ret;

		nread = ret;
		int i;
		for (i=0; i<nread; i++)
		{
			if (bufp[i] == '\n')
			{
				ret = readn(sockfd, bufp, i+1);
				if (ret != i+1)
					exit(EXIT_FAILURE);

				return ret;
			}
		}

		if (nread > nleft)
			exit(EXIT_FAILURE);

		nleft -= nread;
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
			exit(EXIT_FAILURE);

		bufp += nread;
	}

	return -1;
}
