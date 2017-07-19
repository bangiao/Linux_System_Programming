#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/* signal 函数的作用就是注册消息 不够仔细 */
/* signal 就是向内核的 PCB 进程控制块的block状态字置 0  */

void handler(int num)
{
	if (SIGINT == num)
	{
		printf("recv signal num = %d\n", num);
		signal (SIGINT, SIG_IGN);
	}
	else if (SIGQUIT == num)
	{
		sigset_t bset;
		sigemptyset(&bset);
		sigaddset(&bset, SIGINT);
		sigprocmask(SIG_UNBLOCK, &bset, NULL);
	}
}

void  _sa_sigaction(int num, siginfo_t *info, void *p)
{
	printf("recv sig num = %d\n", num);
}

int main(void)
{
	int ret = 0;
	struct sigaction act;
	
	// 这下面的两个参数只能任选一个
	//act.sa_handler = handler;
	act.sa_sigaction = _sa_sigaction;
	
	// 原理同signal 但是他有附加数据
	ret = sigaction(SIGINT, &act, NULL);
	if (-1 == ret)
	{
		perror("func err sigaction ()");
		return -1;
	}
	
	for(;;)
	{
		sleep(1);
	}
	return 0;
}