#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


int main(void)
{
	int ret = 0;
	int pipefd[2] = { 0 };

	ret = pipe(pipefd);
	if (ret < 0)
	{
		ret = errno;
		perror("func err pipe: ");
		goto END;
	}

	int flag = fcntl(pipefd[1], F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(flag, F_SETFL);

	pid_t pid = fork();
	if (0 == pid) // всп╢
	{
		int n = 0;
		close(pipefd[0]);
		pipefd[0] = 0;
		while (1)
		{
			ret = write(pipefd[1], "A", 1);
			if (ret < 0)
			{
				if (errno == EAGAIN)
				{
					printf("pipe is max\n");
					break;
				}
				ret = errno;
				perror("func write err: ");
				return ret;
			}
			printf("ret = %d, n = %d\n", ret, n += 10);
			n++;
		}
		printf("n = %d\n", n);
		close(pipefd[1]);
		pipefd[1] = 0;
		exit(0);
	}


	close(pipefd[2]);
	pipefd[2] = 0;
	while (1);
// 	int t = 5;
// 	while (t)
// 	{
// 		t = sleep(t);
// 	}

END:
	if (0 != pipefd[0])
	{
		shutdown(pipefd[0], SHUT_RDWR);
		pipefd[0] = 0;
	}
	if (0 != pipefd[1])
	{
		shutdown(pipefd[1], SHUT_RDWR);
		pipefd[1] = 0;
	}
	return ret;
}