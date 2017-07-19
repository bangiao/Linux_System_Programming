#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/* signal 函数的作用就是注册消息 */

int main01(void)
{
	pid_t	pid;
	
	signal(SIGCHLD, SIG_IGN);
	
	pid = fork();
	if (-1 == pid)
	{
		perror("fork err");
		exit(0);
	}	
	if (0 == pid)
	{
		printf("child ... \n");
		exit(0);
	}
	
	while (1)
	{
		pause();
	}
	printf("main end\n");
	return 0;
}

void handeler(int num)
{
	printf("信号的 ID: %d\n", num);
	if (num == SIGQUIT)
	{
		exit(0);
	}
}

int main02(void)
{
	char tempchar= 0;
	
	signal(SIGINT, handeler);
	
	printf("如果你键入a字符, 那么将恢复 SIGINT 的行为\n");
	while ((tempchar = getchar()) != 'a')
	{
		pause();
	}
	// 默认函数的意思
	signal(SIGINT, SIG_IGN);
	printf("main .. begin\n");
	return 0;
}


// 测试 signal 函数的返回值
// typedef void (*sighandler_t)(int);

//typedef void (*sighandler_t)(int);

int main(void)
{
	int tempchar;
	sighandler_t old;	//多余  因为有 SIG_DEF
	
	old = signal(SIGINT, handeler);
	if (SIG_ERR == old)
	{
		perror("signal err");
		exit(0);
	}
	
	printf("如果你键入a字符, 那么将恢复 SIGINT 的行为\n");
	while ((tempchar = getchar()) != 'a')
	{
		pause();
	}
	signal(SIGINT, old);
	while (1)
	{
		pause();
	}
	return 0;
}
