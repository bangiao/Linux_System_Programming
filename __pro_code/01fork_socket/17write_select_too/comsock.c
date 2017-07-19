#include "comsock.h"


#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* activate_noblock - ����I/OΪ������ģʽ
* @fd: �ļ������
*/
int activate_nonblock(int fd)
{
	int ret = 0;
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		ret = flags;
		printf("func activate_nonblock err : if (flags == -1) err ret = %d\n", ret);
		return ret;
	}
	
	flags |= O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
	{
		printf("func activate_nonblock err : if (ret == -1) ret = %d\n", ret);
		return ret;
	}
	return ret;
}

/**
* deactivate_nonblock - ����I/OΪ����ģʽ
* @fd: �ļ������
*/
int deactivate_nonblock(int fd)
{
	int ret = 0;
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		ret = flags;
		printf("func deactivate_nonblock err : if (flags == -1) err ret = %d\n", ret);
		return ret;
	}
	flags &= ~O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
	{
		printf("func deactivate_nonblock err : if (ret == -1) ret = %d\n", ret);
		return ret;
	}
	return ret;
}

int recv_peek(int sockfd, void *buf, int len)
{
	while (1)
	{
		int ret = recv(sockfd, buf, len, MSG_PEEK);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

int readline(int sockfd, void *buf, int maxline)
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
		else if (ret == 0)
			return ret;

		nread = ret;
		int i;
		for (i = 0; i < nread; i++)
		{
			if (bufp[i] == '\n')
			{
				ret = readn(sockfd, bufp, i + 1);
				if (ret != i + 1)
				{
					printf("func readline readn err if (ret != i + 1)\n");
					exit(EXIT_FAILURE);
				}
				return ret;
			}
		}

		if (nread > nleft)
		{
			printf("func readline readn err if (nread > nleft)\n");
			exit(EXIT_FAILURE);
		}
		nleft -= nread;
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
		{
			printf("func readline readn err if (ret != nread)\n");
			exit(EXIT_FAILURE);
		}
		bufp += nread;
	}

	return -1;
}

/**
* readn - ��ȡ�̶��ֽ���
* @fd: �ļ�������
* @buf: ���ջ�����
* @count: Ҫ��ȡ���ֽ���
* �ɹ�����count��ʧ�ܷ���-1��Ŀ��˹ر�, ��ȡ��С��cuont
*/
int readn(int fd, void *buf, int count)
{
	int sockfd = fd;
	char *pbuf = buf;
	int nmaxlen = count;

	int ret = 0;
	int nreadlen = 0;

	if (NULL == pbuf)
	{
		ret = Sck_ErrParam;
		printf("funcc readn err\n ");
		return ret;
	}
	while (nmaxlen > 0)
	{
		nreadlen = read(sockfd, pbuf, nmaxlen);
		if (nreadlen < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			//////////////////////////////////////////////////////////////////////////
			perror("func readn if (nreadlen < 0):");
			return(Sck_ErrRetValue);
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
* read_select - ����ʱ��⺯��������������
* @fd: �ļ�������
* @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
* �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
*/
int read_select(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds <= 0)
	{
		ret = Sck_ErrParam;
		printf("funcc read_select err\n ");
		return ret;
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
	} while (ret < 0 && errno == EINTR);
	if (ret < 0)
	{
		ret = Sck_ErrRetValue;
		perror("func read_select select if (ret < 0): ");
		return Sck_ErrRetValue;
	}
	else if (0 == ret)
	{
		errno = ETIMEDOUT;
		ret = Sck_ErrRetValue;
		printf("func write_timeout else if (0 == ret) is timeout err \n");
		return ret;
	}
	else if (ret > 0)
	{
		ret = 0;
		return ret;
	}
	return ret;
}

/**
* writen - ���͹̶��ֽ���
* @fd: �ļ�������
* @buf: ���ͻ�����
* @count: Ҫ��ȡ���ֽ���
* �ɹ�����count��ʧ�ܷ���-1 �ɹ����� 0 �ͻ��˹رշ��� д�˼����ַ�
*/
int writen(int fd, const void *buf, int count)
{
	int sockfd = fd;
	char *pbuf = (char *)buf;
	ssize_t nmaxlen = count;


	int ret = 0;
	int writelen = 0;
	if (NULL == buf || count <= 0)
	{
		ret = Sck_ErrParam;
		printf("funcc writen err\n ");
		return ret;
	}

	while (nmaxlen > 0)
	{
		if ((writelen = write(sockfd, pbuf, nmaxlen)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			//////////////////////////////////////////////////////////////////////////
			perror("func writen if ((writelen = write(sockfd, pbuf, nmaxlen)) < 0) err: ");
			return(Sck_ErrRetValue);
		}
		else if (0 == writelen) // ûд���ַ�, ���ʱ��ͷ���д�˶��ٸ��ַ�
		{
			return count - nmaxlen;
		}
		pbuf = ((char *)pbuf) + writelen;
		nmaxlen -= writelen;
	}
	return count;
}

/**
* write_select - д��ʱ��⺯��������д����
* @fd: �ļ�������
* @wait_seconds: �ȴ���ʱ���������Ϊ0��ʾ����ⳬʱ
* �ɹ���δ��ʱ������0��ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
*/
int write_select(int fd, unsigned int wait_seconds)
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
		if (ret < 0)
		{
			ret = Sck_ErrRetValue;
		}
		else if (ret == 0)
		{
			ret = Sck_ErrRetValue;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}
	return ret;
}