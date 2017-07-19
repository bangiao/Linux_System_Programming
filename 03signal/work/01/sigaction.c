#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void my_sigaction(int signum, siginfo_t * s_t, void *data)
{
	printf("recv signum = %d \n", signum);
	printf("recv s_t.val = %d\n", s_t->si_int);
	exit(0);
}

int main(void)
{
	int ret = 0;
	struct sigaction act;
	act.sa_sigaction = my_sigaction;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	
	ret = sigaction(SIGINT, &act, NULL);
	if (-1 == ret)
	{
		perror("func err sigaction()\n");
		return -1;
	}
	printf("pid = %d\n", getpid());
	while (1)
	{
		;
	}
	return 0;
}
