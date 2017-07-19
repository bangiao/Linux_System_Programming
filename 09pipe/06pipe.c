#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct package
{
	char packbuf[1024];
	int packlen;
}package;

void errno_log(char *errfunc, int errid)
{
	errid = errno;
	perror(errfunc);
	exit(errid);
}

void my_sigaction(int signum, siginfo_t *st, void *dat)
{
	int mypid = 0;
	while ((mypid = waitpid(0, NULL, WNOHANG)) == 0)
	{
		printf("child destroy pid = %d\n", mypid);
	}
	exit(0);
}

int main()
{
	int ret = 0;
	int pipefd[2] = { 0 };
	package recvbuf;
	package sendbuf;

	memset(&recvbuf, 0, sizeof(recvbuf));
	memset(&sendbuf, 0, sizeof(sendbuf));

	ret = pipe(pipefd);
	if (ret < 0)
		errno_log("func pipe err: ", errno);

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = my_sigaction;
	act.sa_flags = SA_SIGINFO;
	ret = sigemptyset(&act.sa_mask);
	ret = sigaction(SIGCHLD, &act, NULL);

	int pid = 0;
	pid = fork();
	if (0 == pid)
	{
		close(pipefd[0]);
		pipefd[0] = 0;
		while (fgets(sendbuf.packbuf, sizeof(sendbuf.packbuf), stdin) != NULL)
		{
			if (0 == (strcmp(sendbuf.packbuf, "-1\n")))
			{
				break;
			}
			sendbuf.packlen = strlen(sendbuf.packbuf) - 1;
			ret = write(pipefd[1], &sendbuf, sizeof(sendbuf));
			if (ret < 0)
				errno_log("func err write: ", errno);
			memset(&sendbuf, 0, sizeof(sendbuf));
		}
		close(pipefd[1]);
		pipefd[1] = 0;
		exit(0);
	}
	if (pid > 0)
	{
		close(pipefd[1]);
		pipefd[1] = 0;
		while (1)
		{
			memset(&recvbuf, 0, sizeof(recvbuf));
			ret = read(pipefd[0], &recvbuf, sizeof(recvbuf));
			printf("recvbuf = %s, recvlen = %d\n", recvbuf.packbuf, recvbuf.packlen);
		}
	}

	if (0 != pipefd[0])
		ret = shutdown(pipefd[0], SHUT_RDWR);
	if (0 != pipefd[1])
		ret = shutdown(pipefd[1], SHUT_RDWR);
	return 0;
}
