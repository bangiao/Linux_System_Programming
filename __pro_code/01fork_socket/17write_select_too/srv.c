#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "comsock.h"

#define PORT 8001


int main(void)
{
	int ret = 0;
	int listenfd = 0;
	int acceptfd = 0;
	int nready = 0;
	int i = 0;
	char recvbuf[1024] = { 0 };


	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd)
	{
		printf("func socket err\n");
		return -1;
	}



	struct sockaddr_in my_addr;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int optval = 1;
	ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (-1 == ret)
	{
		perror("func setsockopt err if (-1 == ret): ");
		return ret;
	}

	ret = bind(listenfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
	if (-1 == ret)
	{
		perror("func err bind\n");
		return -1;
	}

	ret = listen(listenfd, 2);
	if (-1 == ret)
	{
		perror("func listen\n");
		return -2;
	}


	int epfd = epoll_create(2);

	struct epoll_event myevent;
	memset(&myevent, 0, sizeof(myevent));
	myevent.events = EPOLLIN;
	myevent.data.fd = listenfd;

	struct epoll_event arr_events[10];

	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &myevent);
	if (-1 == ret)
	{
		perror("func epoll_ctl err\n");
		return -4;
	}

	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);

	

	while (1)
	{
		nready = epoll_wait(epfd, arr_events, 10, -1);
		if (-1 == nready)
		{
			perror("func epoll_wait err\n");
			return -5;
		}
		if (arr_events[0].data.fd == listenfd)
		{
			int flag = 0;
			acceptfd = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen);
			if (-1 == ret)
			{
				perror("accept err \n");
				return -3;
			}

			flag = fcntl(acceptfd, F_GETFL);
			flag |= O_NONBLOCK;
			fcntl(acceptfd, F_SETFL, flag);

			memset(&myevent, 0, sizeof(myevent));
			myevent.data.fd = acceptfd;
			myevent.events = EPOLLIN;
			ret = epoll_ctl(epfd, EPOLL_CTL_ADD, acceptfd, &myevent);
			if (-1 == ret)
			{
				perror("func epoll_ctl err\n");
				return -6;
			}
			--nready;
		}
		
		for (i = 0; i < nready; i++)
		{
			int sockfd = arr_events[i].data.fd;
			if (arr_events[i].events == EPOLLIN)
			{
				int nread = readline(sockfd, recvbuf, sizeof(recvbuf));
				if (0 == nread)
				{
					memset(&myevent, 0, sizeof(myevent));
					myevent.data.fd = acceptfd;
					myevent.events = EPOLLIN;
					ret = epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &myevent);
					if (-1 == ret)
					{
						perror("func epoll_ctl err\n");
						return -7;
					}
					printf("client %d close\n", sockfd);
					ret = shutdown(sockfd, SHUT_RDWR);
					if (-1 == ret)
					{
						perror("shutdown err \n");
						return -8;
					}
					continue;
				}
				printf("recvbuf = %s\n", recvbuf + 4);
				write(sockfd, recvbuf, (size_t)nread);
				bzero(recvbuf, sizeof(recvbuf));
			}
		}
	}

	return 0;
}
