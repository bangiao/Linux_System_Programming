#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
/* Server */

/* 服务器这边至少有两个套接字 */

int main()
{
	int listenfd = 0;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listenfd)
	{
		perror("func socket err \n");
		return -1;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8001);
	addr.sin_addr.s_addr = inet_addr("192.168.106.128");// 也可以填本地IP 或者 INADDR_ANY
	if ( bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("func bind err \n");
		return -2;
	}
	
	// 一旦调用listen之后 这个 listenfd 将变成  被动套接字
	// 被动套接字只能接收不能发送
	// listen 需要维护两个队列 一个是 正在三次握手队列, 一个是已经完成三次握手的队列
	// 这个被动套接字就是一个只能听的套接字, 被动套接字
	if (0 > listen(listenfd, SOMAXCONN))
	{
		perror("func err listen\n");
		return -3;
	}
	
	struct sockaddr_in peer_addr;
	socklen_t peer_addrlen;// = sizeof(peer_addr);
	
	
	int accfd = 0;
	char revbuf[1024] = {0};
	while (1)
	{
		accfd = accept(listenfd, (struct sockaddr *)&peer_addr, &peer_addrlen);
		if (-1 == accfd)
		{
			perror("func err accept()\n");
			return -4;
		}
		
		pid_t pid = fork();
		if (-1 == pid)
		{
			close (accept);
			close (listenfd);
			perror("func err fork: ");
			return -6;
		}
		else if (0 == pid)
		{
			// 子进程不需要监听所以 close listenfd
			close(listenfd);
			while (1)
			{
				printf("ip:%s, port:%d--->", 
				(char *)inet_ntoa(peer_addr.sin_addr), 
				(int)ntohs(peer_addr.sin_port));
				int ret = read(accfd, revbuf, sizeof(revbuf));
				if (0 == ret)
				{
					// 如果在读的过程中, 入股对方关闭, 会返回 0 
					printf("对方已经关闭\n");
					return -5;
				}
				else if (ret < 0)
				{
					// 表示读数据失败
					printf("读数据失败\n");
					return -6;
				}
				//ssize_t write(int fd, const void *buf, size_t count);
				fputs(revbuf, stdout);
				write(accfd, revbuf, ret);
				memset(revbuf, 0, sizeof(revbuf));
			}
		}
		else if (pid > 0)
		{
			// 父进程用来监听的, 所以不需要 acceptfd
			close(accept);
		}
	}
	close (listenfd);
	close (accfd);
	return 0;
}
















