#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/* signal 函数的作用就是注册消息 不够仔细 */
/* signal 就是向内核的 PCB 进程控制块的block状态字置 0  */

void myhandler(int num)
{
	if (SIGINT == num)
	{
		printf("recv SIGINT num = %d\n", num);
		//signal (SIGINT, SIG_IGN);
	}
	else if (SIGQUIT == num)
	{
        // 这句话是错误的， 是失效的， 原因不知
		sigset_t bset;
		sigemptyset(&bset);
		sigaddset(&bset, SIGINT);
		sigprocmask(SIG_UNBLOCK, &bset, NULL);
		printf("recv SIGQUIT num = %d\n", num);
	}
	else
	{
		printf("recv 其他信号 num = %d\n", num);
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

/*
SIG_BLOCK
       #include <signal.h>

       int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
*/

/*
typedef void (*sighandler_t)(int);

       sighandler_t signal(int signum, sighandler_t handler);
*/

int main(void)
{
	int ret = 0;
	sigset_t set;
	sigset_t oset;
	// 阻塞 SIGINT SIGQUIT 信号
	ret = sigemptyset(&set);
	ret = sigaddset(&set, SIGINT);
	//ret = sigaddset(&set, SIGQUIT);
	ret = sigprocmask(SIG_BLOCK, &set, NULL);

	// 注册回调函数
	signal(SIGINT, myhandler);
	signal(SIGQUIT, myhandler);
	
	while (1)
	{
		memset(&oset, 0, sizeof(oset));
		sigpending(&oset);
		printfsigset(oset);
		sleep(1);
	}
	return 0;
}
