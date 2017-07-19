#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "sckutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
             
void handle(int signum)
{
	if (SIGCHLD == signum)
	{
		pid_t mypid = 0;
		while ((mypid = waitpid(-1, NULL, WNOHANG)) > 0)
		{
			;
		}
	}
	else
	{
		printf("recv other signal %d\n", signum);
	}
}


int main(void)
{
	int listenfd;
	
	signal(SIGCHLD, handle);
	
	if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8001);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		ERR_EXIT("setsockopt");

	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");
	if (listen(listenfd, SOMAXCONN) < 0)
		ERR_EXIT("listen");

	struct sockaddr_in peeraddr;
	socklen_t peerlen;	
	int conn;
	
	while(1)
	{
		int ret = 0;
		int wait_seconds = 5;
		//if ((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
			//ERR_EXIT("accept");
		conn = accept_timeout(listenfd, &peeraddr, 30);
		if (conn < 0)
		{
			printf("func err accept_timeout err \n");
			return -1;
		}

		printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
		
		int pid = fork();
		if (pid == 0)
		{
			char 	recvbuf[1024];
			int 	recvbuflen = 1024;
			int		datalen;
			close(listenfd);
			while(1)
			{
				memset(recvbuf, 0, sizeof(recvbuf));
				ret = read_timeout(conn, wait_seconds);
				if (ret == 0)
				{
					datalen = readline(conn, recvbuf, sizeof(recvbuf));
					if (datalen < 0 )
					{
						ERR_EXIT("read func err\n");
					}
					if (datalen == 0)
					{
						printf("�ͻ��˶Է��Ѿ��ر�\n");
						close(conn);
						ERR_EXIT("readline return 0");
					}
				}
				else if (ret == -1 && errno == ETIMEDOUT )
				{
					printf("����ʱ\n");
					continue;
				}
				else if (ret < 0)
				{
					ERR_EXIT("select ʧ��\n");
				}
				fputs(recvbuf, stdout);
				
				int wtlen = 0;
				ret = write_timeout(conn,  wait_seconds);
				if (ret == 0)
				{
					wtlen = writen(conn, recvbuf, datalen);
					printf("writen : %d\n", wtlen);
				}
				else if (ret == -1 && errno == ETIMEDOUT )
				{
					printf("д��ʱ\n");
					continue;
				}
				else if (ret < 0)
				{
					ERR_EXIT("select ʧ��\n");
				}
			}
		
		}
		else if (pid > 0)
		{
			close(conn);
		}
	}
	

	return 0;
}
