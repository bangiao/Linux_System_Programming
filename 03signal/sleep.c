#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/* sleep 是可中断睡眠 */

void handeler(int num)
{
	printf("信号的 ID: %d\n", num);
	if (num == SIGQUIT)
	{
		exit(0);
	}
}

int main(void)
{
	printf("main ... begin\n");
	signal(SIGINT, handeler);
	// 可中断睡眠
	sleep(100);
	
	// 百分百包睡觉的方法
	/*
	int n = 100;
	do
	{
		n = sleep(n);
	}while (n > 0);
	*/
	printf("main ... begin\n");
	return 0;
}
