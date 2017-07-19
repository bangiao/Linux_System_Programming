#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    pid_t  pid;
    printf("before fork pid = %d\n", getpid());
    // 避免僵尸进程的方法
    // 通过信号的注册函数, 忽略子进程死信号
    signal(SIGCHLD, SIG_IGN);
    pid = fork();
    if (pid == -1)
    {
        perror("fork err");
        return -1;
    }
    else if (pid == 0)		//子进程
    {
        printf("child: %d\n", getpid());
    }
    else if (pid > 0)		//父进程
    {
        printf("parent: %d\n", getpid());
        // 这个成 僵死进程
        sleep(1000);
    }
    printf("after fork\n");
	return 0;
}
