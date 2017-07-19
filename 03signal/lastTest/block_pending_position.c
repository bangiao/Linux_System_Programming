#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

// 信号在创建后, 内核会先观察 block 阻塞状态字, 如果
// 状态字为阻塞则设置 pending 状态字为 阻塞 1 否则 为 0 直接递达信号

void printSignal()
{
	int ret = 0;
	int i = 0;
	
	sigset_t set;
	sigemptyset(&set);
	sigpending(&set);
	
	for (i = 1; i <= 64; i++)
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
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	ret = sigprocmask(SIG_BLOCK, &set, NULL);
	if (-1 == ret)
	{
		printf("func err sigprocmask()\n");
		return -1;
	}
	
	printSignal();
	
	union sigval value;
	value.sival_int = 10;
	
	ret = sigqueue(getpid(), SIGINT, value);
	if (-1 == ret)
	{
		perror("func err sigqueue()\n");
		return -2;
	}
	
	printSignal();
	
	return 0;
}






















