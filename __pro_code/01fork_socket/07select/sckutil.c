#include "sckutil.h"

/* read�����ĵ��÷���
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
 * read_timeout - ����ʱ��⺯��������������
 * @fd: �ļ�������
 * @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
 * �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
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
 * write_timeout - д��ʱ��⺯��������д����
 * @fd: �ļ�������
 * @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
 * �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
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
 * accept_timeout - ����ʱ��accept
 * @fd: �׽���
 * @addr: ������������ضԷ���ַ
 * @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ģʽ
 * �ɹ���δ��ʱ�������������׽��֣���ʱ����-1����errno = ETIMEDOUT
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
 * activate_noblock - ����I/OΪ������ģʽ
 * @fd: �ļ������
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
 * deactivate_nonblock - ����I/OΪ����ģʽ
 * @fd: �ļ������
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
 * @fd: �׽���
 * @addr: Ҫ���ӵĶԷ���ַ
 * @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ģʽ
 * �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
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
			// һ�����ӽ��������׽��־Ϳ�д  ����connect_fdset������д������
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
			/* ret����Ϊ1����ʾ�׽��ֿ�д�������������������һ�������ӽ����ɹ���һ�����׽��ֲ�������*/
			/* ��ʱ������Ϣ���ᱣ����errno�����У���ˣ���Ҫ����getsockopt����ȡ�� */
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
 * readn - ��ȡ�̶��ֽ���
 * @fd: �ļ�������
 * @buf: ���ջ�����
 * @count: Ҫ��ȡ���ֽ���
 * �ɹ�����count��ʧ�ܷ���-1������EOF����<count
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
 * writen - ���͹̶��ֽ���
 * @fd: �ļ�������
 * @buf: ���ͻ�����
 * @count: Ҫ��ȡ���ֽ���
 * �ɹ�����count��ʧ�ܷ���-1
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
 * recv_peek - �����鿴�׽��ֻ��������ݣ������Ƴ�����
 * @sockfd: �׽���
 * @buf: ���ջ�����
 * @len: ����
 * �ɹ�����>=0��ʧ�ܷ���-1
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
 * readline - ���ж�ȡ����
 * @sockfd: �׽���
 * @buf: ���ջ�����
 * @maxline: ÿ����󳤶�
 * �ɹ�����>=0��ʧ�ܷ���-1
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
		else if (ret == 0) // �ػ���
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
