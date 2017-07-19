/* server.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <errno.h>
#include "wrap.h"

#define MAXLINE 80
#define SERV_PORT 8001
#define OPEN_MAX 1024

int main(void)
{
	int ret = 0;
	int acceptfd = 0;
	int listenfd = 0;
	int i = 0;
	int nreadlen = 0;
	char recvbuf[1024] = {0};


	listenfd = Socket(AF_INET, SOCK_STREAM, 0);


	struct sockaddr_in sa;
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	sa.sin_family = AF_INET;
	sa.sin_port = htons(SERV_PORT);
	

	int optval = 1;
	ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (ret < 0)
	{
		printf("func setsockopt err \n");
		return -1;
	}

	Bind(listenfd, (struct sockaddr *) &sa, sizeof (sa));


	Listen(listenfd, 20);


	int nread = 0;
	int maxi = 0;
	struct pollfd buf_fds[OPEN_MAX] = {0}; // 用于存放 fd 的数组
	memset(buf_fds, 0, sizeof(buf_fds));
	buf_fds[0].events = POLLIN;
	buf_fds[0].fd = listenfd;

	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof (peeraddr);
	
	for (i = 1; i < OPEN_MAX; i++)
	{
		buf_fds[i].fd = -1;
	}

	while (1)
	{
		nread = poll(buf_fds, maxi + 1, -1);
		if (buf_fds[0].revents & POLLIN)
		{
			peerlen = sizeof (peeraddr);
			acceptfd = Accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen); // 获得新的连接
			
			for (i = 0; i < OPEN_MAX; i++) // 将新的连接放在数组当中
			{
				if (buf_fds[i].fd < 0)
				{
					buf_fds[i].fd = acceptfd;
					break;
				}
			}
			
			if (i == OPEN_MAX)
			{
				perr_exit("too many clients");
			}
			
			// 将新的连接加入poll中
			buf_fds[i].events = POLLIN;
			if (i > maxi)
			{
				maxi = i;
			}
			if (--nread <= 0)
			{
				continue;
			}
		}
		
		// 类似于 子进程 索要做的程序
		for (i = 0; i < maxi + 1; i++)
		{
			// 代表fd可以写入输入, 有数据传入过来
			if (buf_fds[i].revents & POLLIN)
			{
				// 成功返回>=0，失败返回-1
				nreadlen = readline(buf_fds[i].fd, recvbuf, sizeof(recvbuf));
				if (ret < 0)
				{
					perr_exit("readn\n");
				}

				printf("recvbuf = %s\n", recvbuf + 4);
				 // 成功返回count，失败返回-1 成功返回 0 客户端关闭返回 写了几个字符
				ret = writen(buf_fds[i].fd, recvbuf, nreadlen);
				if (ret < 0)
				{
					perr_exit("writen\n");
				}
				else if (ret > 0 && ret < nreadlen)
				{
					perr_exit("client close\n");
				}
				bzero(recvbuf, sizeof(recvbuf));
				if (--nread <= 0)
					break;
			}
		}
	}
	return 0;
}
