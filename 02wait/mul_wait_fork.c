#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void loopFunc(int num)
{
	printf("LoopFunc() %d\n", num);
}

int	main(void)
{
	int ret = 0;
	int	loopNum = 0;
	int	ProcNum = 0;
	int	i = 0, j = 0;

	printf("请输入进程数:");
	scanf("%d", &ProcNum);
	printf("请输入每个进程运行的圈数:");
	scanf("%d", &loopNum);

	pid_t	 pid;
	printf("before fork	pid	=	%d\n", getpid());

	for (i = 0; i < ProcNum; i++)
	{
		pid = fork();
		if (pid == 0)
		{
			for (j = 0; j < loopNum; j++)
			{
				loopFunc(j);
			}
			exit(0);
		}
	}
	// 检测到所有子进程的退出父进程才退出
	while (1)
	{
		//ret = wait(NULL);
		//pid_t waitpid(pid_t pid, int *status, int options);
		
		// 因为进程的空间是独立的, 所以每个进程都有自己的内存空间, 所以都需要wait
		ret = waitpid(-1, &status, 0);
		if (-1 == ret)
		{
			// 让父进程阻塞阻塞过程可能被别的信号中断 需要在做异常处理
			if (EINTR == errno)
			{
				continue;
			}
			break;
		}
	}
	printf("父进程退出after	fork\n");
	return 0;
}