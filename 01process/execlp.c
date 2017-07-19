#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// vfork 就是用着和 exec 一起使用的

int main01(void)
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
    }
    else if (pid > 0)
    {
        printf("parent: %d\n", getpid());
        
        // 具有自我结束能力的数组
        // l 可变参数 p 从 path 中查找
        execlp("ls", "ls", "-lt",NULL);
        
        
        _exit(0);
    }
    
    printf("after fork\n");
	return 0;
}

int main(int argc, char *argv[], char *envp[])
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
    }
    else if (pid > 0)
    {
        printf("parent: %d\n", getpid());
        
        //char *ars[] = { "aa=111", "bb=222", "cc=333", NULL };
        
        // 具有自我结束能力的数组
        /*
        int execle(const char *path, const char *arg,
                  ..., char * const envp[]);
        */
       
        // execle ("./hello", "./hello", NULL, ars);
        
        /*
        int execve(const char *filename, char *const argv[],
                  char *const envp[]);
        */
        execve("./hello", argv, envp);
        _exit(0);
    }
    
    printf("after fork\n");
	return 0;
}