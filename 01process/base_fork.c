#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 1.perror 这个是显示错误码的  在shell 中使用  perror 100 显示错误码
//
// 2. fork 特点
// 一次调用两次返回
// fork 是一个系统调用在Linux内核中完成
// 是在每一个进程各自的进程空间中返回
//
// 为什么父进程和子进程的返回值不一样
// 父子的关系是一对N的关系, 老爹找儿子的pid比较难
//
// fork 理解角度: 就是两个进程同时往下运行
//
// 为什么子函数选择从fork 之后运行 , 而不是 main 函数开始运行呢?
// 这个和fork函数的机制有关
// 写时复制, 子进程要拷贝父进程的代码段 数据段 堆栈段 PCB进程控制块
// PCB进程控制块就是linux内核管理进程的数据结构 task_struct
// 因为孩子拷贝了父进程的运行场景, 所以child就没有必要再看一遍了
// 这个时候资源已经复制了, 没要在重新开始运行

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
    else if (pid == 0)
    {
        printf("child: %d\n", getpid());
    }
    else if (pid > 0)
    {
        printf("parent: %d\n", getpid());
    }
    printf("after fork\n");
	return 0;
}
