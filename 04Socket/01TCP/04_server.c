#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* Server */

int newfd = 0;

void func_exit (void)
{
	close(newfd);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int listenfd = 0;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd)
	{
		perror("func err socket: ");
		return -1;
	}
	
	int optval = 1;
	ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (-1 == ret)
	{
		perror("func err setsockopt: ");
		return -4;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8001);
	addr.sin_addr.s_addr = inet_addr("192.168.106.128");
	ret = bind(listenfd, (struct sockaddr *)&addr, sizeof(addr));
	if (-1 == ret)
	{
		perror("func err bind: ");
		return -2;
	}
	
	ret = listen(listenfd, SOMAXCONN);
	if (-1 == ret)
	{
		perror("func err listen: ");
		return -3;
	}

	struct sockaddr_in target_addr;
	socklen_t socklen = sizeof(target_addr);
	newfd = accept(listenfd, (struct sockaddr *)&target_addr, &socklen);
	printf("client connect success : ip is %s, port is %d\n",
		inet_ntoa(target_addr.sin_addr), ntohs(target_addr.sin_port));
	close(listenfd);
	char revbuf[1024] = { 0 };
	atexit(func_exit);
	while (1)
	{
		ret = read(newfd, revbuf, sizeof(revbuf));
		if (0 == ret)
		{
			printf("客户端已经退出\n");
			return -4;
		}
		printf("revbuf = %s, len = %d\n", revbuf, ret);
		write(newfd, revbuf, sizeof(revbuf));
		memset(revbuf, 0, sizeof(revbuf));
	}
	return 0;
}
