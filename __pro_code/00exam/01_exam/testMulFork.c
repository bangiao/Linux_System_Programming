#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
进程基础相关
2、编程一个基本多进程测试框架，提示用户输入进程数、和每个进程数运行圈数。进行多进程压力测试。
要求父进程能监控所有子进程的退出，避免僵尸进程。
*/

int main(void)
{
    pid_t pid = 0;
    int proc_num = 0;
    int i = 0;

    printf("请输入子进程的数量:");
    scanf("%d", &proc_num);

    for (i = 0; i < proc_num; i++)
    {
        pid = fork();
        if (-1 == pid)
        {
            perror("fucn err fork: ");
            break;
        }
        if (0 == pid)
        {
            static int j = 0;
            printf("fork a new child process pid = %d, id = %d\n",
                   (int)getpid(), j++);
            int n = 5;
            while (n)
            {
                printf("sleep begin = %d\n", n);
                n = sleep(n);
                printf("sleep end = %d\n", n);
            }
            exit(0);
        }
    }
    int j = 0;
    int ret = 0;
    while (j < i)
    {
        printf("destroy process  %d\n", j++);
        ret = wait(NULL);
        if (-1 == ret)
        {
            break;
        }
    }
    printf("总共fork 了 %d 个进程\n ", j + 1);
    return 0;
}
