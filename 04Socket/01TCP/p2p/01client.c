#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
/* client */

/*
父进程 :
	从键盘中读取数据, 发送数据
子进程:
	客户端检测出来, 对方关闭, 就发送给父进程退出
	子进程也退出
注册一个信号
子进程挂了告诉父进程 你也得挂
*/

void my_sighandler(int signum)
{
	// 父进程接收到消息
	printf("recv signum = %d\n", signum);
	exit(0);
}

int main(void)
{
	int sockfd = 0;
	// 创建客户端的 socket
	printf("客户端开始启动\n");
	int ret = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		perror("func socket err \n");
		return -1;
	}
	
	// 连接服务器
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8001);
	inet_aton("192.168.106.128", &(addr.sin_addr));
	if(-1 == connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		perror("func err connect ()");
		goto END;
	}
	
	pid_t pid = 0;
	pid = fork();
	if (-1 == pid)
	{
		perror("func err fork:");
		goto END;
	}
	else if (pid > 0)		// 父进程
	{
		// 父进程注册消息
		signal(SIGRTMIN, my_sighandler);
		char sendbuf[1024] = {0};
		while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
		{
			// 从键盘中读取数据, 发送数据
			write(sockfd, sendbuf, strlen(sendbuf));
			memset(sendbuf, 0, sizeof(sendbuf));
		}
	}
	else if (0 == pid)		// 子进程
	{
		char revbuf[1024] = {0};
		while (1)
		{
			// 从服务器读取数据
			ret = read(sockfd, revbuf, sizeof(revbuf));
			if (-1 == ret)
			{
				perror("func err read: ");
				break;
			}
			else if (0 == ret)
			{
				printf("对方已经退出..\n");
				break;
			}
			// 将读取出来的数据打印在屏幕上面
			fputs(revbuf, stdout);
			memset(revbuf, 0, sizeof(revbuf));
		}
		kill(getppid(), SIGRTMIN);
	}
END:
		if (0 != sockfd)
		{
			close (sockfd);
			sockfd = 0;
		}
	return 0;
}
















