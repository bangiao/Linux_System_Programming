#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
/* 测试性能 */
/*
注册回调函数 SIGRTMIN  SIGUSR1
将阻塞这个信号   SIGRTMIN  信号
通过调用  SIGUSR1 信号去解除 上面信号的阻塞
*/

/* 内核调用回调函数  解除也是解除内核block的阻塞  和 程序PCB 互不影响 */
void my_sigaction(int num, siginfo_t *st, void *dat)
{
	int ret = 0;
	
	
	// 收到信号 SIGUSR1 解除 SIGRTMIN 阻塞
	if (SIGUSR1 == num)
	{
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGRTMIN);
		ret = sigprocmask(SIG_UNBLOCK, &set, NULL);
		printf("解除阻塞 recv num = %d\n", num);
	}
	// 收到信号 SIGRTMIN 打印出来 表示解除阻塞成功
	else if (SIGRTMIN == num)
	{
		printf("SIGRTMIN recv num = %d\n", num);
	}
	else
	{
		printf("其他信号 recv num = %d\n", num);
	}
}

void printSignal()
{
	int ret = 0;
	sigset_t set;
	sigemptyset(&set);
	sigpending(&set);
	int i = 1;
	for (; i <= 64; i++)
	{
		ret = sigismember(&set, i);
		if (0 == ret)
		{
			printf("0");
		}
		else if (1 == ret)
		{
			printf("1");
		}
	}
	puts("");
}

int main(void)
{
	int ret = 0;
	int i = 0;
	
	
	// 注册回调函数 SIGRTMIN  SIGUSR1
	struct sigaction act;
	act.sa_sigaction = my_sigaction;
	act.sa_flags = SA_SIGINFO;
	ret = sigaction(SIGRTMIN, &act, NULL);
	ret = sigaction(SIGUSR1, &act, NULL);
	
	
	// 将阻塞这个信号   SIGRTMIN  信号
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGRTMIN);
	ret = sigprocmask(SIG_BLOCK, &set, NULL);
	
	
	pid_t pid;
	pid = fork();
	assert(!(-1 == ret));
	if (0 == pid)
	{
		// 通过调用  SIGUSR1 信号去解除 上面信号的阻塞
		union sigval value;
		value.sival_int = 0;
		for (i = 0; i < 9*1024; i++)
		{
			// 不断的发送信号 SIGRTMIN 将 信号阻塞到 内核 的 block 中,内核外也有一个block
			value.sival_int++;
			sigqueue(getppid(), SIGRTMIN, value);
		}
		sigqueue(getppid(), SIGUSR1, value);
		exit(0);
	}
	printSignal();
	while (1)
	{
		sleep(1);
	}
	
	return 0;
}
























