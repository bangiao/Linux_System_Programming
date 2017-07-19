#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
/* Server */

// 父进程:
// 	检测客户端的退出
// 	检测子进程的退出
// 	接收子进程的数据
// 子进程:
// 	从键盘中接收数据
// 	发送数据

// 父进程得发信号, 告诉子进程, 我死了, 你必须也得死

/*
 typedef void (*sighandler_t)(int);

       sighandler_t signal(int signum, sighandler_t handler);

*/

// 子进程收到父进程的信号
void my_sighandler(int signum)
{
	printf("子进程 recv signal is = %d\n", signum);
	exit(0);
}

int main()
{
	int listenfd = 0;
	int accfd = 0;
	int ret = 0;
	// 创建sockfd 文件描述符
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd)
	{
		perror("func socket err \n");
		goto END;
	}
	
	// bind 绑定文件描述符
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8001);
	addr.sin_addr.s_addr = inet_addr("192.168.106.128");// 也可以填本地IP 或者 INADDR_ANY
	if ( bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("func bind err \n");
		goto END;
	}
	
	// 监听
	if (0 > listen(listenfd, SOMAXCONN))
	{
		perror("func err listen\n");
		goto END;
	}
	
	// accept
	printf("客户端正在等待连接中 ... \n");
	struct sockaddr_in peer_addr;
	socklen_t peer_addrlen = sizeof(peer_addr);
	accfd = accept(listenfd, (struct sockaddr *)&peer_addr, &peer_addrlen);
	if (-1 == accfd)
	{
		perror("func err accept()\n");
		goto END;
	}
	
	printf("peeraddr = %s, peerport = %d\n", 
		(char *)inet_ntoa(peer_addr.sin_addr), 
		(int)ntohs(peer_addr.sin_port));
	
	pid_t pid;
	pid = fork();
	if (-1 == pid)
	{
		perror("func err fork: ");
		goto END;
	}
	else if (pid > 0)
	{
		// 父进程:
		// 	检测客户端的退出
		// 	检测子进程的退出
		// 	接收客户端的数据
		char revbuf[1024] = {0};
		while (1)
		{
			// read
			ret = read(accfd, revbuf, sizeof(revbuf));
			if (0 == ret)
			{
				// 如果在读的过程中, 入股对方关闭, 会返回 0 
				printf("对方已经关闭\n");
				// 父进程告诉子进程, 说 他挂了
				kill(pid, SIGRTMIN);
				goto END;
			}
			else if (ret < 0)
			{
				// 表示读数据失败
				printf("读数据失败\n");
				kill(pid, SIGRTMIN);
				goto END;
			}
			fputs(revbuf, stdout);
			memset(revbuf, 0, sizeof(revbuf));
		}
		
	}
	else if (0 == pid)
	{
		// 子进程:
		// 	从键盘中接收数据
		// 	发送数据
		// write
		// 子进程需要注册信号回调函数
		signal(SIGRTMIN, my_sighandler);
		char sendbuf[128]= {0};
		while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
		{
			write(accfd, sendbuf, sizeof(sendbuf));
		}
	}
END:
	if (0 != listenfd)
	{
		close (listenfd);
		listenfd = 0;
	}
	if (0 != accfd)
	{
		close (accfd);
		accfd = 0;
	}
	return 0;
}
















