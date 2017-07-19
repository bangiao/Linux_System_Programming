#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
3、信号可靠信号不可靠信号相关
编程程序，要去实现如下功能：
	父进程创建子进程1和子进程2、子进程1向子进程2发送可靠信号，并传送额外数据为子进程1的pid*2;
	子进程2接受可靠信号的值，并发送给父进程，父进程把接受的值进行打印。
	提示：用sigqueue和sigaction实现
*/
/*
struct sigaction {
       void     (*sa_handler)(int);
       void     (*sa_sigaction)(int, siginfo_t *, void *);
       sigset_t   sa_mask;
       int        sa_flags;
       void     (*sa_restorer)(void);
   };
*/


void My_sigaction(int signum, siginfo_t *st, void *dat)
{
	if (SIGRTMIN == signum)
	{
		int ret = 0;
		union sigval value;
		value.sival_int = st->si_int;
		ret = sigqueue(getppid(), SIGRTMIN+1, value);
		if (-1 == ret)
		{
			perror("func err sigqueue: ");
			return ;
		}
		exit(0);
	}
	else if (SIGRTMIN+1 == signum)
	{
		printf("父进程接收到信号 %d\n", st->si_int);
	}
	else
	{
		printf("接收到未知信号\n");
	}
}

int main(void)
{
	// 1 创建两个进程
	int ret = 0;
	pid_t pid1 = 0 , pid2 = 0;
	pid1 = fork();
	if (-1 == pid1)
	{
		perror("func err fork()");
		return -1;
	}
	else if (0 == pid1)
	{
		while (1)
		{
			// 2 子进程1向子进程2发送 可靠信号，
			union sigval value;
			// 3 并传送额外数据为子进程1的pid*2;
			value.sival_int = ((int )pid1) * 2;
			ret = sigqueue(pid2, SIGRTMIN, value);
			if (-1 == ret)
			{
				perror("func err sigqueue: ");
				return -2;
			}
			exit(0);
		}
	}
	// 4 子进程2接受可靠信号的值，并发送给父进程，
	// 5 父进程把接受的值进行打印。
	pid2 = fork();
	if (-1 == pid2)
	{
		perror("func err fork()");
		return -2;
	}
	else if (0 == pid2)
	{
		struct sigaction act;
		sigemptyset(&act.sa_mask);
		act.sa_sigaction = My_sigaction;
		ret = sigaction(SIGRTMIN, &act, NULL);
		if (-1 == ret)
		{
			perror("func err sigaction: ");
			return -3;
		}
		while (1);
	}
	wait(NULL);
	
	return 0;
}










