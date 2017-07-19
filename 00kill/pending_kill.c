#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void printSet(sigset_t set)
{
    int ret = 0;
    int i = 1;
    for (i = 1; i < 65; i++)
    {
        if (0 == (ret = sigismember(&set, i)))
		{
			write(STDOUT_FILENO, "0", 1);
		}
		else if (1 == ret)
		{
			write(STDOUT_FILENO, "1", 1);
		}
		else 
		{
			perror("func err printSet()\n");
			return ;
		}
    }
    puts("");
}

int main(void)
{
    int ret = 0;
    sigset_t set;
   
	sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigprocmask(SIG_BLOCK, &set, NULL);
    
    signal(SIGQUIT, SIG_DFL);
    printf("kill begin...\n");
    // 1. 前面的信号被阻塞 kill 效果无效, 
    // 2. kill 并没有去修改 block 阻塞状态字 , 说明kill 另辟蹊径 不是用这种大众方法
    // 综上 kill 函数关注了 pending 的状态, 
	ret = kill(getpid(), SIGQUIT);
    if (-1 == ret)
    {
        perror("func err kill()\n");
        return -1;
    }
    printf("kill after\n");
    
    sigemptyset(&set);
    
	sigpending(&set);
    
    printSet(set);
	
	return 0;
}
