#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// 演示 vfork 和 fork 的区别
// vfork 共享父进程的数据段 堆栈段 代码段 ...等等
// 父子进程的 pid 不同
// vfork 函数必须和 execle 这一类函数 在一起 或者使用 exit _exit
// 不建议使用 vfork  

int main(void)
{
    pid_t  pid;
    
    printf("before fork pid = %d\n", getpid());
    
    int num;
    
    signal(SIGCHLD, SIG_IGN);
    
    pid = vfork();
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
        // exec 族函数
        //return 0;  err 
        //exit(0);  
        // 这里涉及到 系统函数和 c lib函数的区别
        _exit(0);
    }
    
    printf("after fork\n");
	return 0;
}
