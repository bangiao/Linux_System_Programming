#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main(void)
{
	int ret = 0;
	pid_t pid = 0;
	int nPid = 0;
	/*
	union sigval {
               int   sival_int;
               void *sival_ptr;
           };
	*/
	union sigval value;
	printf("«Î ‰»Î pid\n");
	scanf("%d", &nPid);
	pid = (pid_t )nPid;
	
	value.sival_int = 100;
	
	ret = sigqueue(pid, SIGINT, value);
	if (-1 == ret )
	{
		perror("func err sigqueue()\n");
		return -1;
	}
	return 0;
}