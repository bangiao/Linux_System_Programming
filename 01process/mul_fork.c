#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void loopFunc(int num)
{
	printf("LoopFunc() %d\n", num);
}

int	main11(void)
{
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
		if (pid == -1)
		{
			perror("fork err");
			return -1;
		}
		else if (pid == 0)
		{
			for (j = 0; j < loopNum; j++)
			{
				loopFunc(j);
			}
			exit(0);
		}
		else if (pid > 0)
		{
			;
		}
	}
	printf("after	fork\n");
	return 0;
}

int main(void)
{
	fork();
	fork();
	fork();
	printf("hello ...\n");
	return 0;
}