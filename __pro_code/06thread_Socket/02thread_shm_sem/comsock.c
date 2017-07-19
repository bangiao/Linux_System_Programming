#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "comsock.h"

typedef struct SockHandle
{
	//int sockfd[100]; // 这个可以根据具体情况适用数据结构
	int sockfd;
	int ArrayNum;
	int conntime;
	int sendtime;
	int recvtime;
} SockHandle;

/**
* activate_noblock - 设置I/O为非阻塞模式
* @fd: 文件描符符
*/
static int activate_nonblock(int fd)
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
* deactivate_nonblock - 设置I/O为阻塞模式
* @fd: 文件描符符
*/
static int deactivate_nonblock(int fd)
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

/**
* read_timeout - 读超时检测函数，不含读操作
* @fd: 文件描述符
* @wait_seconds: 等待超时秒数，如果为0表示不检测超时
* 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
*/
int read_timeout(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds < 0)
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
	} while (ret < 0 && errno == EINTR);
	if (ret < 0)
	{
		printf("func write_timeout if (ret < 0) err \n");
		return -1;
	}
	else if (0 == ret)
	{
		errno = ETIMEDOUT;
		printf("func write_timeout else if (0 == ret) is timeout err \n");
		return -1;
	}
	else if (ret > 0)
	{
		ret = 0;
		return ret;
	}
	return ret;
}

/**
* readn - 读取固定字节数
* @fd: 文件描述符
* @buf: 接收缓冲区
* @count: 要读取的字节数
* 成功返回count，失败返回-1，目标端关闭, 读取的小于cuont
*/
static ssize_t readn(int fd, void *buf, size_t count)
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
* write_timeout - 写超时检测函数，不含写操作
* @fd: 文件描述符
* @wait_seconds: 等待超时秒数，如果为0表示不检测超时
* 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
*/
static int write_timeout(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds >= 0)
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
			ret = -1;
		}
		else if (ret == 0)
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
* writen - 发送固定字节数
* @fd: 文件描述符
* @buf: 发送缓冲区
* @count: 要读取的字节数
* 成功返回count，失败返回-1 成功返回 0 客户端关闭返回 写了几个字符
*/
static ssize_t writen(int fd, const void *buf, size_t count)
{
	int ret = 0;
	ssize_t nmaxlen = count;
	ssize_t writelen = 0;
	char *pbuf = (char *)buf;
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
		pbuf = ((char *)pbuf) + writelen;
		nmaxlen -= writelen;
	}
	return count;
}

/**
* connect_timeout - connect
* @fd: 套接字
* @addr: 要连接的对方地址
* @wait_seconds: 等待超时秒数，如果为0表示正常模式
* 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
*/
static int connect_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret = 0;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	if (NULL == addr)
	{
		ret = -1;
		printf("func connect_timeout err if (NULL == addr) ret = %d\n", ret);
		return ret;
	}
	if (wait_seconds > 0)
	{
		ret = activate_nonblock(fd); // 设置非阻塞
		if (ret < 0)
		{
			printf("func connect_timeout ret = activate_nonblock(fd) err ret = %d\n", ret);
			return ret;
		}
	}
	ret = connect(fd, (struct sockaddr*) addr, addrlen);
	if (ret < 0 && errno == EINPROGRESS) // 如果有 EINPROGRESS 这个关键字出来表示, 连接比较慢 要我们等一等
	{
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
			goto END;
		}
		else if (ret < 0)
		{
			ret = -1;
			goto END;
		}
		else if (ret > 0) // 大于零的话有两种可能性, 一种是成功, 一种是 fd发生错误的情况
		{
			/* ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
			/* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
			int err;
			socklen_t socklen = sizeof(err);
			int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
			if (sockoptret == -1)
			{
				ret = -1;
				printf("funcc connect_timeout getsockopt  err if (sockoptret == -1) ret = %d\n", ret);
				goto END;
			}
			if (err == 0) //没错
			{
				ret = 0;
				goto END;
			}
			else // fd 错误
			{
				errno = err;
				ret = -1;
				goto END;
			}
		}
	}

END:
	if (wait_seconds > 0)
	{
		ret = deactivate_nonblock(fd); // 设置阻塞模式
		if (ret < 0)
		{
			printf("func connect_timeout ret = deactivate_nonblock(fd) err ret = %d\n", ret);
			return ret;
		}
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
static int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds < 0)
	{
		ret = -1;
		printf("func accept_timeout if (wait_seconds < 0) err ret = %d\n", ret);
		return ret;
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
	} while (ret < 0 && ret == EINTR);
	if (ret < 0)
	{
		ret = -1;
		//printf("accept_timeout  if (-1 == ret) ret = %d\n", ret);
		return ret;
	}
	else if (0 == ret)
	{
		ret = -1;
		errno = ETIMEDOUT;
		//printf("accept_timeout  else if (0 == ret) ret = %d\n", ret);
		return ret;
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





// 客户端初始化, 初始化只有一次

enum SckErrType sck_client_init(void **handle, const int conntime, const int sendtime, const int recvtime, const int nConNum)
{
	int ret = Sck_Ok;
	if (conntime <= 0 || sendtime < 0 || recvtime < 0 || nConNum <= 0 || NULL == handle)
	{
		ret = Sck_ErrParam;
		printf("func sck_client_init err : if (conntime < 0 || sendtime < 0 || recvtime < 0  || nConNum <= 0) ret = %d\n", ret);
		return ret;
	}
	if (*handle != NULL)
	{
		free(handle);
		handle = NULL;
	}
	SockHandle *tmp = malloc(sizeof(SockHandle));
	if (NULL == tmp)
	{
		ret = Sck_ErrMalloc;
		printf("func sck_client_init err if (NULL == tmp) ret = %d\n", ret);
		return ret;
	}
	memset(tmp, 0, sizeof(SockHandle));

	tmp->conntime = conntime;
	tmp->recvtime = recvtime;
	tmp->sendtime = sendtime;
	tmp->ArrayNum = nConNum;

	// 初始化 socket 

	*handle = (SockHandle *)tmp;
	return ret;
}


// 客户端 文件句柄关闭

enum SckErrType sck_client_close(void *handle)
{
	int ret = Sck_Ok;
	SockHandle *tmp = (SockHandle *)handle;

	if (0 != tmp->sockfd)
	{
		close(tmp->sockfd);
		tmp->sockfd = 0;
	}
	return ret;
}



// 客户端连接, 客户端连接的话可能存在多次连接的情况, 所以封装出一个新的函数

enum SckErrType sck_client_getconn(void *handle, const char *ip, const int port)
{
	int ret = Sck_Ok;
	if (NULL == handle || NULL == ip || port < 0 || port > 65537)
	{
		ret = Sck_ErrParam;
		printf("func sck_client_getconn err : if (NULL == handle || NULL == ip || port < 0 || port > 65535 || NULL == connfd) %d\n", ret);
		return ret;
	}
	SockHandle *tmp = (SockHandle *)handle;


	tmp->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (tmp->sockfd < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_client_init err if (tmp->sockfd < 0) ret = %d\n", ret);
		return ret;
	}


	// 设置目标主机 ip 和 端口
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(ip);


	// 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
	ret = connect_timeout(tmp->sockfd, &servaddr, tmp->conntime);
	if (ret < 0)
	{
		if (errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
		}
		else
		{
			ret = Sck_BaseErr;
		}
		printf("func sck_client_getconn err ret = connect_timeout(tmp->sockfd, &servaddr, tmp->conntime) ret = %d\n", ret);
		return ret;
	}

	return ret;
}



// 客户端发送消息

enum SckErrType sck_client_send(void *handle, const char *buf, const unsigned int buflen)
{
	int ret = Sck_Ok;

	SockHandle *tmp = (SockHandle *)handle;

	if (NULL == handle || NULL == buf || buflen <= 0 || buflen >= 1020)
	{
		ret = Sck_ErrParam;
		printf("func sck_client_send err if (NULL == handle || NULL == buf || buflen <= 0 ) ret = %d\n", ret);
		return ret;
	}
	// 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
	ret = write_timeout(tmp->sockfd, tmp->sendtime);
	if (ret < 0)
	{
		if (errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
		}
		else
		{
			ret = Sck_BaseErr;
		}
		printf("func sck_client_send write_timeout err if (ret < 0) ret = %d\n", ret);
		return ret;
	}

	unsigned char *netdata = (unsigned char *)malloc(buflen + 4);
	if (NULL == netdata)
	{
		ret = Sck_BaseErr;
		printf("func sck_client_send malloc er if (NULL == netdata) ret = %d\n", ret);
		return ret;
	}
	int netlen = htonl(buflen);
	memcpy(netdata, &netlen, 4);
	memcpy(netdata + 4, buf, buflen);

	// 成功返回count，失败返回-1 成功返回 0 客户端关闭返回 写了几个字符
	//static ssize_t writen(int fd, const void *buf, size_t count);
	ret = writen(tmp->sockfd, netdata, buflen + 4);
	if (-1 == ret)
	{
		ret = Sck_BaseErr;
		printf("func sck_client_send writen if (-1 == ret) ret = %d\n", ret);
		goto END;
	}
	if (ret < buflen + 4)
	{
		ret = Sck_ErrPeerClosed;
		printf("client is close ret = %d\n", ret);
		goto END;
	}

END:
	if (NULL != netdata)
	{
		free(netdata);
		netdata = NULL;
	}
	return ret;
}



// 客户端接收消息

enum SckErrType sck_client_recv(void *handle, char *buf, unsigned int *buflen)
{
	int ret = Sck_Ok;
	if (NULL == handle || NULL == buf || NULL == buflen)
	{
		ret = Sck_ErrParam;
		printf("func sck_client_recv err if (NULL == handle || NULL == buf || NULL == buflen) ret = %d\n", ret);
		return ret;
	}
	SockHandle *tmp = (SockHandle *)handle;

	// 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
	ret = read_timeout(tmp->sockfd, tmp->recvtime);
	if (ret < 0)
	{
		if (ETIMEDOUT == errno)
		{
			ret = Sck_ErrTimeOut;
			printf("func sck_client_recv read_timeout err if (ret < 0) ret = %d\n", ret);
			return ret;
		}
		ret = Sck_BaseErr;
		printf("func sck_client_recv sck_client_recv err if (ret < 0) ret = %d\n", ret);
		return ret;
	}

	int netdatalen = 0;

	// 成功返回count，失败返回-1，目标端关闭, 读取的小于cuont
	// 读取了长度

	ret = (int)readn(tmp->sockfd, &netdatalen, 4);
	if (ret < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_client_recv readn err if (nlen < 0) ret = %d\n", ret);
		return ret;
	}
	if (ret < 4)
	{
		ret = Sck_ErrPeerClosed;
		printf("func sck_client_recv readn err if (nlen < 4) ret = %d\n", ret);
		return ret;
	}

	int hlen = ntohl(netdatalen);

	// 读取剩下的数据
	// 成功返回count，失败返回-1，目标端关闭, 读取的小于cuont

	ret = (int)readn(tmp->sockfd, buf, hlen);
	if (ret < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_client_recv readn err if (ret < 0) ret = %d\n", ret);
		return ret;
	}
	if (ret < hlen)
	{
		ret = Sck_ErrPeerClosed;
		printf("func sck_client_recv readn err if (ret < nlen) ret = %d\n", ret);
		return ret;
	}

	*buflen = hlen;
	return 0;
}


// 客户端销毁

enum SckErrType sck_client_destroy(void *handle)
{
	int ret = Sck_Ok;
	if (NULL == handle)
	{
		ret = Sck_ErrParam;
		printf("func  sck_client_destroy err if (NULL == handle) ret = %d\n ", ret);
		return ret;
	}
	sck_client_close(handle);
	SockHandle *tmp = (SockHandle *)handle;
	if (0 != tmp->sockfd)
	{
		close(tmp->sockfd);
		tmp->sockfd = 0;
	}
	free(tmp);
	handle = NULL;
	return 0;
}



//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

// 服务端代码

// 服务器端初始化

enum SckErrType sck_server_init(const int port, int *listenfd)
{
	int ret = Sck_Ok;
	if (port <= 0 || port > 65536 || NULL == listenfd)
	{
		ret = Sck_ErrParam;
		printf("func sck_server_init err if (port <= 0 || port > 65536 || NULL == listenfd) ret = %d\n", ret);
		return ret;
	}
	int tmplistenfd = 0;
	tmplistenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (tmplistenfd < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_server_init socket  err if (ret < 0) ret = %d\n", ret);
		return ret;
	}


	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	ret = setsockopt(tmplistenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (-1 == ret)
	{
		ret = Sck_BaseErr;
		printf("func sck_server_init setsockopt err if (-1 == ret) ret = %d\n", ret);
		return ret;
	}

	ret = bind(tmplistenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_server_init bind err if (ret < 0) ret = %d\n", ret);
		return ret;
	}
	ret = listen(tmplistenfd, SOMAXCONN);
	if (ret < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_server_init listen err if (ret < 0) ret = %d\n", ret);
		return ret;
	}
	*listenfd = tmplistenfd;
	return ret;
}



// 服务器端拿数据
// enum SckErrType sck_server_accept(int listenfd, int timeout, int *acceptfd, char * const ip, int *port);

enum SckErrType sck_server_accept(const int listenfd, const int timeout, int *acceptfd, char ip[], int *port)
{
	int ret = Sck_Ok;
	if (listenfd < 0 || NULL == acceptfd || NULL == ip || NULL == port || timeout < 0)
	{
		ret = Sck_ErrParam;
		printf("func sck_server_accept err if (listenfd < 0 || NULL == acceptfd || NULL == ip || NULL == port) ret = %d\n", ret);
		return ret;
	}

	int tmp_accrptfd = 0;

	struct sockaddr_in peeraddr;
	memset(&peeraddr, 0, sizeof(peeraddr));

	tmp_accrptfd = accept_timeout(listenfd, &peeraddr, timeout);
	if (tmp_accrptfd < 0)
	{
		if (ETIMEDOUT == errno)
		{
			ret = Sck_ErrTimeOut;
			//printf("func sck_server_accept accept_timeout err if (ETIMEDOUT == errno) ret = %d\n", ret);
			return ret;
		}
		ret = Sck_BaseErr;
		//printf("func err sck_server_accept accept_timeout err if (*acceptfd < 0) ret = %d\n", ret);
		return ret;
	}

	char *tmpip = inet_ntoa(peeraddr.sin_addr);
	memcpy(ip, tmpip, strlen(tmpip));
	*port = htons(peeraddr.sin_port);
	*acceptfd = tmp_accrptfd;
	return ret;
}



// 服务器端发送消息

enum SckErrType sck_server_send(const int acceptfd, const int timeout, const char *buf, const unsigned int buflen)
{
	int ret = Sck_Ok;
	if (NULL == buf || buflen <= 0 || timeout < 0)
	{
		ret = Sck_ErrParam;
		printf("func sck_server_send err if (NULL == buf || buflen <= 0) ret  = %d\n", ret);
		printf("buf = %s, timeout = %d, buflen = %d\n", buf, timeout, buflen);
		return ret;
	}


	// 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
	ret = write_timeout(acceptfd, timeout);
	if (ret < 0)
	{
		if (errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			printf("func sck_server_send write_timeout err if (errno == ETIMEDOUT) ret = %d\n", ret);
			return ret;
		}
		ret = Sck_BaseErr;
		printf("func sck_server_send write_timeout err if (ret < 0) ret = %d\n", ret);
		return ret;
	}

	unsigned char *netdata = (unsigned char *)malloc(buflen + 4);
	if (NULL == netdata)
	{
		ret = Sck_ErrMalloc;
		printf("func er sck_server_send malloc if (NULL == netdata) ret = %d\n ", ret);
		return ret;
	}
	int netlen = htonl(buflen);
	memcpy(netdata, &netlen, 4);
	memcpy(netdata + 4, buf, buflen);

	// 成功返回count，失败返回-1 成功返回 0 客户端关闭返回 写了几个字符
	ret = writen(acceptfd, netdata, buflen + 4);
	if (-1 == ret)
	{
		ret = Sck_BaseErr;
		printf("func sck_server_send writen err if (-1 == ret) ret = %d\n", ret);
		goto END;
	}
	if (ret < buflen + 4)
	{
		ret = Sck_ErrPeerClosed;
		printf("func sck_server_send writen err if (ret < buflen + 4) ret = %d\n", ret);
		goto END;
	}
END:
	if (NULL != netdata)
	{
		free(netdata);
		netdata = NULL;
	}
	return ret;
}



// 服务器端接收消息

enum SckErrType sck_server_recv(const int acceptfd, const int timeout, char *buf, unsigned int *buflen)
{
	int ret = Sck_Ok;
	if (NULL == buf || NULL == buflen || timeout < 0)
	{
		ret = Sck_ErrParam;
		printf("func sck_server_recv err if (NULL == buf || NULL == buflen) ret = %d\n", ret);
		return ret;
	}

	// 成功（未超时）返回0，失败返回-1，超时返回-1并且errno = ETIMEDOUT
	ret = read_timeout(acceptfd, timeout);
	if (ret < 0)
	{
		if (ETIMEDOUT == errno)
		{
			ret = Sck_ErrTimeOut;
			printf("func sck_server_recv read_timeout err if (ETIMEDOUT == errno) ret = %d\n", ret);
			return ret;
		}
		ret = Sck_BaseErr;
		printf("func sck_server_recv sck_client_recv err if (ret < 0) ret = %d\n", ret);
		return ret;
	}

	int netdatalen = 0;

	// 成功返回count，失败返回-1，目标端关闭, 读取的小于cuont
	// 读取了长度

	ret = (int)readn(acceptfd, &netdatalen, 4);
	if (ret < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_server_recv readn err if (ret < 0) ret = %d\n", ret);
		return ret;
	}
	if (ret < 4)
	{
		ret = Sck_ErrPeerClosed;
		printf("func sck_server_recv readn err if (ret < 4) ret = %d\n", ret);
		return ret;
	}

	int hlen = ntohl(netdatalen);

	// 读取剩下的数据
	// 成功返回count，失败返回-1，目标端关闭, 读取的小于cuont

	ret = (int)readn(acceptfd, buf, hlen);
	if (ret < 0)
	{
		ret = Sck_BaseErr;
		printf("func sck_server_recv readn err if (ret < 0) ret = %d\n", ret);
		return ret;
	}
	if (ret < hlen)
	{
		ret = Sck_ErrPeerClosed;
		printf("func sck_server_recv readn err if (ret < nlen) ret = %d\n", ret);
		return ret;
	}

	*buflen = hlen;
	return 0;
}



// 服务端关闭套接字句柄

enum SckErrType sck_server_close(const int sockfd)
{
	int ret = Sck_Ok;

	close(sockfd);

	return ret;
}
