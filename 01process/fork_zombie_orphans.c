#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 信号是什么?
// 信号是一种异步处理事件, 是一种机制
// 言外之意就是我们的程序在顺序执行的时候, 可以支持异步调用信号处理函数

int main(void)
{
    pid_t  pid;
    printf("before fork pid = %d\n", getpid());
    pid = fork();
    if (pid == -1)
    {
        perror("fork err");
        return -1;
    }
    else if (pid == 0)		//子进程
    {
        printf("child: %d\n", getpid());
        // 这个就是孤儿进程
        sleep(100);
    }
    else if (pid > 0)		//父进程
    {
        printf("parent: %d\n", getpid());
        // 这个就是 僵死进程
        //sleep(1000);
    }
    printf("after fork\n");
	return 0;
}
