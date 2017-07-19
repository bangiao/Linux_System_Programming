#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// 文件描述符 和 指针  都是这样的;  
// 它会复制不同的指针指向同一块内存
// 演示父子进程 共享文件描述符的情况
// 相当于 两个fd 相同一块内存空间 
// 因为两个进程共享了文件指针偏移 , 所以能向文件中有序写数据

int	main(void)
{
	pid_t  pid;
	printf("before fork pid = %d\n", getpid());
	
	int	fd = 0;
	fd = open("test.txt", O_WRONLY);
	if (-1 == fd)
	{
		perror("open err");
		exit(0);
	}
	
	signal(SIGCHLD,	SIG_IGN);
	pid	= fork();
	if (pid	== -1)
	{
		perror("fork err");
		return -1;
	}
	else if	(pid ==	0)		//子进程
	{
		write (fd, "child", 5);
		printf("child: %d\n", getpid());
		close (fd); // close 需要关闭两次
	}
	else if	(pid > 0)		//父进程
	{
		write (fd, "parent", 6);
		printf("parent: %d\n", getpid());
		close (fd); // close 需要关闭两次
	}
	printf("after fork\n");
	return 0;
}
