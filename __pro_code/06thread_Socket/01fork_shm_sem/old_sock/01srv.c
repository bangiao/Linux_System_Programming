#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "comsock.h"
#include <signal.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)
        
void handle(int signum)
{
	int pid = 0;
	printf("recv signum:%d \n", signum);
	
	//避免僵尸进程
	while ((pid = waitpid(-1, NULL, WNOHANG) ) > 0)
	{
		printf("退出子进程pid%d \n", pid);
		fflush(stdout);
	} 
}

int main(void)
{
	int		ret = 0;
	int 	listenfd = 0;
	char	ip[1024] = {0};
	int		port = 0;
	int		connfd = 0;
	
	signal(SIGCHLD, handle);
	signal(SIGPIPE, SIG_IGN);
	

	ret = sck_server_init(8001, &listenfd);
	if (ret != 0)
	{
		printf("sckServer_init() err:%d \n", ret);
		return ret;
	}
	
	while(1)
	{
		int ret = 0;
		int wait_seconds = 5;
		

		ret = sck_server_accept(listenfd, wait_seconds, &connfd, ip, &port);
		if (ret >= 3000)
		{
			printf("timeout....\n");
			continue;
		}
		printf("客户端连接成功...\n");
		int pid = fork();
		if (pid == 0)
		{
			char			recvbuf[1024];
			unsigned int  	recvbuflen = 1024;
				
			close(listenfd);
			while(1)
			{
				memset(recvbuf, 0, sizeof(recvbuf));
				ret = sck_server_recv(connfd, 5, recvbuf, &recvbuflen);
				if (ret >= 3000)
				{
					printf("func sck_server_recv() err:%d \n", ret);
					break;
				}
				
				ret = sck_server_send(connfd, 6, recvbuf, recvbuflen);
				if (ret >= 3000)
				{
					printf("func sck_server_send() err:%d \n", ret);
					break;
				}
			}
			close(connfd);
			exit(ret);
		}
		else if (pid > 0)
		{
			close(connfd);
		}
	}
	
	sck_server_close(listenfd);
	sck_server_close(connfd);
	return 0;
}
