#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* alarm 信号就是  cpu 设置 n 秒之后执行发送消息 SIGALRM */
// CPU 内核  发送 alarm 信号, 而 信号就是通过回调函数的方法实现的

typedef struct teather
{
	int age ;
	int num;
}teather;

teather g_t; 

void printGlobTeacher()
{
	printf("g_t.age = %d\n", g_t.age);
	printf("g_t.num = %d\n", g_t.num);
}

void myhandler(int num)
{
	printf("func myhandler()\n");
	
	printGlobTeacher();
	
	// 一秒钟发送一次 alarm 信号
	alarm(1); // 这句话的意思就是通知 内核 一秒钟以后再次回调 alarm 信号
}

int main(void)
{
	printf("main ... begin\n");
	
	teather t1, t2;
	t1.age = 30;
	t1.num = 30;
	t2.age = 40;
	t2.num = 40;
	
	
	if (SIG_ERR == signal(SIGALRM, myhandler))
	{
		perror("func signal err \n");
		return -1;
	}
	alarm (1);
	
	
	while (1)
	{
		// 这个就是 相当于 window 系统上面的  system("pause");
		// 但是 pause 是可中断的暂停
		//pause();
		//printf("pause return \n");
		
		g_t = t1;
		g_t = t2;
		
	}
	return 0;
}