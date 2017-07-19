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
		signal (SIGINT, SIG_IGN);
	}
	else if (SIGQUIT == num)
	{
        // 这句话是错误的， 是失效的， 原因不知
		sigset_t bset;
		sigemptyset(&bset);
		sigaddset(&bset, SIGINT);
		sigprocmask(SIG_UNBLOCK, &bset, NULL);
	}
}

void printfsigset(const sigset_t set)
{
	//int sigismember(const sigset_t *set, int signum);
	int i = 1;
	int ret = 0;
	for (i = 1; i <= 64; i++ )
	{
		if ( 0 == (ret = sigismember(&set, i)))
		{
			putchar('0');
		}
		else if (1 == ret)
		{
			putchar('1');
		}
		else
		{
			perror("func printfsigset () sigismember(() err\n ");
			return ;
		}
	}
	puts("");
}

int main(void)
{
	int ret = 0;
	sigset_t pset;	// pending set 未决状态字
	sigset_t bset;	// block set 阻塞状态字
	
	//  block set 清空
	sigemptyset(&bset);
	//  block bset 变量的 SIGINT 置 1
	sigaddset(&bset, SIGINT);
	// 将 block 的 bset 变量的值, 通过设置内核 PCB 进程控制块 修改 内核 PCB中的 block 阻塞状态字
	ret = sigprocmask(SIG_BLOCK, &bset, NULL);
	if (-1 == ret)
	{
		perror("func err sigprocmask()\n");
		return -2;
	}
	printfsigset(pset);

	if ((signal(SIGINT, handler)) == SIG_ERR)
	{
		perror("signal err ");
		return -1;
	}
	if ((signal(SIGQUIT, handler)) == SIG_ERR)
	{
		perror("signal err\n");
		return -2;
	}
	
	for(;;)
	{
		sigpending (&pset);
		printfsigset(pset);
		sleep(1);
	}
	return 0;
}
