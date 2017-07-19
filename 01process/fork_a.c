#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// fork 进程的引申
// 对每个进程有独立的内存空间的引申
//
// 做全局变量的引申
//
// 写时复制
// 1. 子进程修改变量的时候 copy
// 2. 只拷贝一页, 没有则发送缺页中断 , 继续拷贝

int g_num = 100;

int main(void)
{
    int num = 10;

    pid_t  pid;
    printf("before fork pid = %d\n", getpid());
    pid = fork();
    if (pid == -1)
    {
        perror("fork err");
        return -1;
    }
    else if (pid == 0)
    {
        printf("child: %d\n", getpid());
        num++;//11
        printf("child num = %d &num = %p\n", num, &num);
        g_num++;//11
        printf("child g_num = %d &g_num = %p\n", g_num, &g_num);
    }
    else if (pid > 0)
    {
        printf("parent: %d\n", getpid());
        num++;//11
        printf("parent num = %d &num = %p\n", num, &num);
        g_num++;//11
        printf("parent g_num = %d &g_num = %p\n", g_num, &g_num);
    }
    printf("after fork\n");
	return 0;
}
