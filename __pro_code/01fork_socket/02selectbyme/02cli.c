#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "comsock.h"
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

int main()
{	
	int 		ret = 0;
	void 		*handle = NULL;
	int 		i = 0;
	 char				*data = "aaaaaafffffffffffssssss";
	 unsigned int 		datalen = 10;
	 
	 char out[1024] = {0};
	 unsigned int outlen = 1024;
	//客户端环境初始化
	//int sckCliet_init(void **handle, char *ip, int port, int contime, int sendtime, int revtime);
	ret = sck_client_init(&handle, 15, 5, 5, 10);
	ret = sck_client_getconn(handle, "127.0.0.1", 8001);
	
	for (i = 0; i < 1000; i++)
	{
		//客户端发送报文
		ret = sck_client_send(handle, data, datalen);
		if (ret == Sck_ErrTimeOut)
		{
			//ret = sckClient_send(handle,  connfd, data, datalen);
		}
		
		//客户端端接受报文
		ret = sck_client_recv(handle, out, &outlen);
		out[outlen] = '\0';
		printf("data: %s \n", out);
	}
	
	// 客户端环境释放 
	ret = sck_client_destroy(handle);
    return 0;
}

