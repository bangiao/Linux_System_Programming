/* 
 * File:   main.c
 * Author: luv letter
 *
 * Created on 2017年3月15日, 下午7:36
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
/*
 * 客户端 -- 这是一个短链接
 */
int main(int argc, char** argv)
{
	int ret = 0;
	int i = 0;
	for(i = 0; i < 10; i++)
	{
		// 创建socket fd
		int sockfd = 0;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (-1 == sockfd)
		{
			perror("func err socket: ");
			return -1;
		}
		
		// 连接服务器
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8001);
		addr.sin_addr.s_addr = inet_addr("192.168.106.128");
		ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
		if (-1 == ret)
		{
			printf("connect err \n");
			return -2;
		}
		
		char writebuf[128] = {0};
		sprintf(writebuf, "hello world i = %d\n", i);
		// write 
		write(sockfd, writebuf, sizeof(writebuf));
		
		// read
		char revbuf[128] = {0};
		int revlen = read(sockfd, revbuf, sizeof(revbuf));
		if (0 == revlen)
		{
			printf("no data read\n");
			return -3;
		}
		if (-1 == revlen)
		{
			perror("func err read:");
			return -4;
		}
		
		ret = fputs(revbuf, stdout);
		if (EOF == ret)
		{
			printf("func err fputs\n");
			return -5;
		}
		
		// destroy sockfd
		if (0 == sockfd)
		{
			close (sockfd);
			sockfd = 0;
		}
	}
	
	return (EXIT_SUCCESS);
}

