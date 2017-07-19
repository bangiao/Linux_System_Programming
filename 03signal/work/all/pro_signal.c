#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
/* 测试性能 */
/*
将阻塞两个信号  SIGINT   SIGRTMIN 信号
通过调用  SIGUSR1 信号去解除 上面信号的阻塞
*/
 
void func_block()
{
	int ret = 0;
	sigset_t set;
	ret = sigemptyset(&set);
	ret = sigaddset(&set, SIGINT);
	ret = sigaddset(&set, SIGRTMIN);
	ret = sigprocmask(SIG_BLOCK, &set, NULL);
}

void func_unblock()
{
	int ret = 0;
	sigset_t set;
	ret = sigemptyset(&set);
	ret = sigaddset(&set, SIGINT);
	ret = sigaddset(&set, SIGRTMIN);
	ret = sigprocmask(SIG_UNBLOCK, &set, NULL);
}


 void my_sigaction(int signum, siginfo_t *info, void *dat)
 {
	int ret = 0;
	if (SIGUSR1 == signum)
	{
		func_unblock();
		// sigset_t set;
		// ret = sigemptyset(&set);
		// ret = sigaddset(&set, SIGINT);
		// ret = sigaddset(&set, SIGRTMIN);
		// ret = sigprocmask(SIG_UNBLOCK, &set, NULL);
		printf("解除阻塞 recv sig num:%d\n", signum);
	}
	else if (SIGINT == signum || signum == SIGRTMIN)
	{
		printf("收到的数据是: %d\n", info->si_value.sival_int);
	}
	else 
	{
		printf("其他recv sig num:%d\n", signum);
		printf("收到的数据是 %d\n", info->si_value.sival_int);
	}
 }
int main(void)
{
	pid_t pid;
	int ret = 0;
	
	
	// 把 SIGINT  SIGRTMIN 都添加到阻塞状态字中
	func_block();
	// sigset_t set;
	// ret = sigemptyset(&set);
	// ret = sigaddset(&set, SIGINT);
	// ret = sigaddset(&set, SIGRTMIN);
	// ret = sigprocmask(SIG_BLOCK, &set, NULL);
	
	
	
	// 设置回调函数
	struct sigaction act;
	act.sa_sigaction = my_sigaction;
	act.sa_flags = SA_SIGINFO;	// 千万别忘记了
								// 注册非实时信号的处理函数
	ret = sigaction(SIGINT, &act, NULL);
	assert(!(-1 == ret));
								// 注册实时信号的处理函数
	ret = sigaction(SIGRTMIN, &act, NULL);
	assert(!(-1 == ret));
								// 注册用户自定义信号
	ret = sigaction(SIGUSR1, &act, NULL);
	assert(!(-1 == ret));
	
	
	
	pid = fork();
	assert(!(-1 == ret));
	if (0 == pid)
	{
		union sigval value;
		int i = 0;
		value.sival_int = 0;
		// Linux 内核 实时信号压力测试
		for (i = 0; i < 9*1024; i++)
		{
			// 发送信号
			value.sival_int++;
			ret = sigqueue(getppid(), SIGRTMIN, value);
			//assert(!(-1 == ret));
			//printf("发送可靠信号ok i = %d\n", i);
		}
		printf("signal max id is = %d\n", i);
		// 向父进程发送 SIGUSR1 解除阻塞
		ret = kill(getppid(), SIGUSR1);
		//assert(!(-1 == ret));
	}
	while (1)
	{
		sleep(1);
	}
	return 0;
}