#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/* 子进程想父进程发送消息 */

void myhandler(int num)
{
	if (SIGINT == num)
	{
		printf("recv signal SIGINT\n");
	}
	else if (SIGUSR1 == num)
	{
		printf("recv signal SIGUSR1\n");
	}
	else
	{
		printf("recv signal id num = %d\n", num);
	}	
	
}

int main(void)
{
	pid_t	pid;
	printf("main ... begin()\n");
	
	if ((signal(SIGINT, myhandler)) == SIG_ERR)
	{
		perror("func signal err\n");
		return -1;
	}
	if ((signal(SIGUSR1, myhandler)) == SIG_ERR)
	{
		perror("func signal err \n");
		return -2;
	}
	
	pid = fork();
	if (-1 == pid)
	{
		printf("fork err ... \n");
		return 0;
	}
	/*
	if (0 == pid)
	{
		//pid = getpgrp();
		pid = getppid();	// 就是父进程的 ID
		//向 进程组发送信号
		//kill(-pid, SIGUSR1);	//   本程序中发送给父进程和子进程  两个进程
		//向老爹发送信号
		kill(pid, SIGUSR1);
		//killpg(pid, SIGUSR1);
		exit(0);
	}
	*/
	if (0 == pid)
	{
		pid = getpgrp();
		killpg(pid, SIGUSR1);
		exit(0);
	}
	
	int n = 3;
	
	
	// 睡个够
	printf("父进程开始睡眠\n");
	do
	{
		n = sleep(n);
	}while (n > 0);
	printf("父进程重新唤醒\n");
	printf("sleep 函数执行完毕以后返回...\n");
	
	return 0;
}