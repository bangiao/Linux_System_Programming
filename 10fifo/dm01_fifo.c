#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

/*
利用管道，两个进程间进行文件复制。
fifow
读文件1.txt	
	写入管道
fifor	
	读管道
	写入2.txt文件
	*/

int main(int argc, char *argv[])
{
	//建立管道文件
	int outfd = 0;
	
	//打开管道文件，准备写数据
	outfd = open("fifop", O_WRONLY | O_NONBLOCK);
	//outfd = open("pp", O_WRONLY ); //阻塞模式
	if (outfd == -1)
	{
		printf("infd open() err..\n");
		exit(0);
	}
	
	return 0;
}


	/*
	   int fcntl(int fd, int cmd);
	 int fcntl(int fd, int cmd, long arg);
	
	int flags = 0;
	flags = fcntl(infd, F_GETFD);
	flags = flags | O_NONBLOCK;
	
	fcntl(infd, F_SETFD, flags);
	 */