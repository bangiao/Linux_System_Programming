#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// 这是vfork 的主要应用场景

int main(void)
{
    pid_t  pid;
    
    printf("before fork pid = %d\n", getpid());
    
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
        execve("./hello", NULL, NULL);
        printf ("execve 未执行\n");
        exit(0);
    }
    else if (pid > 0)
    {
        printf("parent: %d\n", getpid());
    }
    
    printf("after fork pid = %d\n", getpid());
	return 0;
}
