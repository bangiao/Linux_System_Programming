#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

void perr_exit(const char *s)
{
	perror(s);
	exit(1);
}


int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;
again:
	if ((n = accept(fd, sa, salenptr)) < 0)
	{
		if ((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			perr_exit("accept error");
	}
	return n;
}


int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	int n;
	if ((n = bind(fd, sa, salen)) < 0)
		perr_exit("bind error");
	return n;
}


int Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	int n;
	if ((n = connect(fd, sa, salen)) < 0)
		perr_exit("connect error");
	return n;
}


int Listen(int fd, int backlog)
{
	int n;
	if ((n = listen(fd, backlog)) < 0)
		perr_exit("listen error");
	return n;
}


int Socket(int family, int type, int protocol)
{
	int n;
	if ((n = socket(family, type, protocol)) < 0)
		perr_exit("socket error");
	return n;
}


ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;
again:
	if ((n = read(fd, ptr, nbytes)) == -1)
	{
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}


ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
	ssize_t n;
again:
	if ((n = write(fd, ptr, nbytes)) == -1)
	{
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}


int Close(int fd)
{
	int n;
	if ((n = close(fd)) == -1)
		perr_exit("close error");
	return n;
}


/**
 * readn - 读取固定字节数
 * @fd: 文件描述符
 * @buf: 接收缓冲区
 * @count: 要读取的字节数
 * 成功返回count; 失败返回-1; 目标端关闭, 读取的小于cuont
 */
ssize_t readn(int fd, void *buf, size_t count)
{
	int ret = 0;
	size_t nmaxlen = count;
	size_t nreadlen = 0;
	char *pbuf = buf;

	if (NULL == pbuf)
	{
		ret = -1;
		printf("func readn if (NULL == pbuf) err ret = %d\n", ret);
		return ret;
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
			ret = -1;
			printf("func err readn if (nreadlen < 0) ret = %d\n", ret);
			return ret;
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
 * 成功返回count，失败返回-1 成功返回 0 客户端关闭返回 写了几个字符
 */
ssize_t writen(int fd, const void *buf, size_t count)
{
	int ret = 0;
	ssize_t nmaxlen = count;
	ssize_t writelen = 0;
	char *pbuf = (char *) buf;
	if (NULL == buf)
	{
		ret = -1;
		printf("func err writen if (NULL == buf)  ret = %d\n", ret);
		return ret;
	}
	if (count <= 0)
	{
		ret = -1;
		printf("func err writen if (count <= 0) ret = %d\n", ret);
		return ret;
	}
	while (nmaxlen > 0)
	{
		if ((writelen = write(fd, pbuf, nmaxlen)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			ret = -1;
			printf("func err writen if ((writelen = write(connfd, pbuf, nmaxlen)) < 0) ret = %d\n", ret);
			return ret;
		}
		else if (0 == writelen) // 没写入字符, 这个时候就返回写了多少个字符
		{
			return count - nmaxlen;
		}
		pbuf = ((char *) pbuf) + writelen;
		nmaxlen -= writelen;
	}
	return count;
}


static ssize_t my_read(int fd, char *ptr)
{
	static int read_cnt;
	static char *read_ptr;
	static char read_buf[100];

	if (read_cnt <= 0)
	{
again:
		if ((read_cnt = read(fd, read_buf, sizeof (read_buf))) < 0)
		{
			if (errno == EINTR)
				goto again;
			return -1;
		}
		else if (read_cnt == 0)
			return 0;
		read_ptr = read_buf;
	}
	read_cnt--;
	*ptr = *read_ptr++;
	return 1;
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
