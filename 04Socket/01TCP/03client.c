#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

/* client */

int sockfd = 0;

void func_destroy(void)
{
	close (sockfd);
}

int main(void)
{
	printf("客户端开始启动\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		perror("func socket err \n");
		return -1;
	}
	// 设置关闭调用函数
	atexit(func_destroy);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8001);
	//int inet_aton(const char *cp, struct in_addr *inp);
	//addr.sin_addr.s_addr = inet_addr("192.168.106.128");
	inet_aton("192.168.106.128", &(addr.sin_addr));
	if(-1 == connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		perror("func err connect ()");
		return -2;
	}
	
	char revbuf[1024] = {0};
	char sendbuf[1024] = {0};
	while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
	{
		// 向服务写数据
		write(sockfd, sendbuf, strlen(sendbuf));
		// 从服务器读取数据
		read(sockfd, revbuf, sizeof(revbuf));
		fputs(revbuf, stdout);
		memset(revbuf, 0, sizeof(revbuf));
		memset(sendbuf, 0, sizeof(sendbuf));
	}
	
	return 0;
}
















