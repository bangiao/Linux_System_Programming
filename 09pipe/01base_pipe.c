#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
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

	pid_t pid = fork();
	if (pid < 0)
	{
		ret = errno;
		perror("func err fork: ");
		goto END;
	}
	else if (0 == pid) // ×ÓÐ´
	{
		close(pipefd[0]);
		pipefd[0] = 0;
		ret = write(pipefd[1], "zhazha", 6);
		if (ret < 0)
		{
			ret = errno;
			perror("func write err: ");
			goto END;
		}
		close(pipefd[1]);
		pipefd[1] = 0;
		exit(0);
	}

	// ¸¸¶Á
	close(pipefd[1]);
	pipefd[1] = 0;
	char recvbuf[1024] = { 0 };
	int recvlen = 6;
	ret = read(pipefd[0], recvbuf, recvlen);
	if (ret < 0)
	{
		ret = errno;
		perror("func err read: ");
		goto END;
	}
	printf("recvbuf = %s, recvlen = %d\n", recvbuf, recvlen);

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