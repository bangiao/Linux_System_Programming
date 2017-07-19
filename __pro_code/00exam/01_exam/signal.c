#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/*
信号相关试题
1、请编写一个程序，设置SIGINT和SIGQUIT信号，并在该程序中实现从文件中读取信息的操作，
并保证在读取文件且只有在读取文件的过程中不会被发送的SIGINT和SIGQUIT信号所打断。

   提示：在文件读取前阻塞信号，在文件读取后解除阻塞。
*/

void func_sigaction(int signum, siginfo_t *set, void *dat)
{
    if (SIGINT == signum)
    {
        printf("SIGINT 信号递达 %d\n", set->si_value.sival_int);
        return ;
    }
    else if (SIGQUIT == signum)
    {
        printf("SIGQUIT 信号递达 %d\n", set->si_value.sival_int);
    }
    else
    {
        printf("接收到未知信号\n");
    }
}

int main()
{
    // 1 设置SIGINT和SIGQUIT信号
    int ret = 0;
    struct sigaction act;
    act.sa_sigaction = func_sigaction;
    ret = sigemptyset(&act.sa_mask);
    if (-1 == ret)
    {
        printf("func err sigemptyset()\n");
        return -1;
    }

    // 注册回调函数 SIGINT
    ret = sigaction(SIGINT, (struct sigaction *)&act, NULL);
    if (-1 == ret)
    {
        perror("func err SIGINT sigaction: ");
        return -2;
    }

    // 注册回调函数 SIGQUIT
    ret = sigaction(SIGQUIT, (struct sigaction *)&act, NULL);
    if (-1 == ret)
    {
        perror("func err SIGQUIT sigaction: ");
        return -3;
    }

    // 阻塞信号
    sigset_t set;
    ret = sigemptyset(&set);
    if (-1 == ret)
    {
        printf("func err sigemptyset()\n");
        return -4;
    }

    ret = sigaddset(&set, SIGINT);
    if (-1 == ret)
    {
        printf("func err SIGINT sigaddset()\n");
        return -5;
    }

    ret = sigaddset(&set, SIGQUIT);
    if (-1 == ret)
    {
        printf("func err SIGQUIT sigaddset()\n");
        return -6;
    }

    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (-1 == ret)
    {
        perror("func err sigprocmask:");
        return -7;
    }

    // 2 实现从文件中读取信息, 不会被发送的SIGINT和SIGQUIT信号所打断
    union sigval value;
    value.sival_int = 10;
    ret = sigqueue(getpid(), SIGINT ,value);
    if (-1 == ret)
    {
        perror("func err sigqueue: ");
        return -8;
    }

    value.sival_int = 20;
    ret = sigqueue(getpid(), SIGQUIT ,value);
    if (-1 == ret)
    {
        perror("func err sigqueue: ");
        return -9;
    }

    FILE *fp = NULL;
    fp = fopen("test", "a+");
    if (NULL == fp)
    {
        printf("func err fopen()\n");
        return -10;
    }

    char revbuf[1024] = {0};
    char *tmp = NULL;
    while (NULL != (tmp = fgets(revbuf, sizeof(revbuf), fp)))
    {
        printf("%s\n", tmp);
    }
    if (NULL != fp)
    {
        fclose (fp);
        fp = NULL;
    }

    ret = sigemptyset(&set);
    if (-1 == ret)
    {
        printf("func err sigemptyset\n");
        return -11;
    }
    ret = sigaddset(&set, SIGINT);
    if (-1 == ret)
    {
        printf("func err SIGINT sigaddset\n");
        return -12;
    }
    ret = sigaddset(&set, SIGQUIT);
    if (-1 == ret)
    {
        printf("func err SIGQUIT sigaddset\n");
        return -13;
    }
    ret = sigprocmask(SIG_UNBLOCK, &set, NULL);
    if (-1 == ret)
    {
        printf("func err SIG_UNBLOCK sigprocmask\n");
        return -14;
    }
    return 0;
}


