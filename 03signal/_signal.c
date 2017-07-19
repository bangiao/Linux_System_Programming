#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

void printSignalset(sigset_t s)
{
    int i = 0;
    for (i = 0; i < 32; i++)
    {
    	// sigismember 从 集合  s 中读取 第 i 位的值 (0/1)
        if (1 == sigismember(&s, i))
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
    }
    puts("");
}

int main()
{
		//sigset_t  ***是一个64位的数*** ,  类似于这样 0000000000000000000000000000000000000000000000000000....
		// ***这里面每一个位置都对应着一个信号(信号是有 ID 的 kill -l 查看信号 ID), 其位置上面的值就是这个信号的状态***
    sigset_t s, p;
    int i = 0;
		
		// sigemptyset 这个函数设置Linux ***用户自定义的 block 集合 *** (sigset_t s) 将其内容清空(置 0)
    sigemptyset(&s); //(sigset_t s) 将其内容清空(置 0)
    sigaddset(&s, SIGQUIT);// 将 集合  s 中的 SIGQUIT 信号位置置 1 
    sigaddset(&s, SIGINT);// 同上, 将 SIGINT 信号所属的位置置 1
    sigprocmask(SIG_BLOCK, &s, NULL);// 将 sigset_t s 集合以  ***或的形式***  注册进  ***内核中的 block 集合*** , 
    while (1)
    {
        sigpending(&p);	// 读取内核中的 pending 集合
        printSignalset(p);// 打印出现在 pending 中的信号状态, 1 为未决态, 0 表示信号已经发送
        if (10 == i)
        {
        		sigemptyset(&s); // 将 s 集合的内容再次清空
            sigaddset(&s, SIGINT);	//  将 SIGINT 信号所属的位置置 1
            sigprocmask(SIG_UNBLOCK, &s, NULL);// SIG_UNBLOCK 就是将 s 集合 取反再与 的形式注入 内核的block 就是置 SIGINT 成 0 
            // 比如 : 先 1 取反变 0,  0 与 原先的 1 这样的话原先的 1 就变成了 0  
        }
        sleep(1);
        printf("循环第 %d 次\n", i + 1);
        i++;
    }
    return 0;
}
