#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(void)
{
	int ret = 0;
	int pipe_fd[2] = { 0 };
	pipe(pipe_fd);
	int count = 0;

	int flag = fcntl(pipe_fd[1], F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(pipe_fd[1], F_SETFL, flag);

	//close(pipe_fd[0]);		// 这个是一个主进程, 如果把这个关闭的话, 就产生了管道破裂的风险
	while (1)
	{
		ret = write(pipe_fd[1], "A", 1);
		if (ret < 0)
		{
			break;
		}
		count++;
	}
	printf("count = %d\n", count);
	return 0;
}
