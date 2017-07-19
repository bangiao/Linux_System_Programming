#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_PATH 256

void func_daemon()
{
	// 1. 创建子进程, 结束父进程
	pid_t pid;
	
	pid = fork();
	if (-1 == pid)
	{
		printf("进程创建失败\n");
		return ;
	}
	else if (0 != pid)
	{
		printf("父进程pid = %d\n", (int) getpid());
		exit(0);
	}
	printf("子进程 pid = %d\n", (int)getpid());
	
	// 2. 子进程创建绘画框 setsid() 使得子进程彻底脱离 shell
	setsid();
	
	
	// 3. 改变当前目录为根目录 chdir
	if (chdir("/") < 0)
	{
		perror("chdir\n");
		exit(1);
	}
	
	
	// 4. 重置文件权限掩码 umask
	umask(0);
	

	// 5. 关闭文件描述符
	close (0);
	open("/dev/null",  O_RDWR);
	dup2(0, 1);
	dup2(0, 2);
	close (1);
	close (2);
}


int main(void)
{
	int fd = 0;
	
	//  在此循环中可以实现守护进程的核心工作
	
    FILE *fp= NULL;
    fp = fopen("zhazha.txt", "w");
    if (NULL == fp)
    {
        printf("func err fopen()\n");
        return -1;
    }

	func_daemon();

	char buf[MAX_PATH] = {0};
	int i = 0;
	while (1)
	{
		i++;
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "buf = %d\n", i);
        fwrite(buf, sizeof(*buf), MAX_PATH, fp);
		sleep(1);
	}
	return 0;
}







