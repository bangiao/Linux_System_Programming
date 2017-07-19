#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	pid_t pid;

	FILE *fp = NULL;
	fp = fopen("./zhazha.txt", "a+");
	if (NULL == fp)
	{
		printf ("func err fopen()\n");
		return -2;
	}
    printf("open file success\n");
	pid = fork();
	if (-1 == pid)
	{
		perror("func err \n");
		return -1;
	}
	else if (pid > 0)
	{
		printf("parent pid = %d\n", getpid());
        fclose (fp);
		exit(0);
	}
	
	setsid();
	
	chdir("/");
	
	umask(0022);
	
	close(0);
	open("/dev/null",  O_RDWR);
	dup2(0, 1);
	dup2(0, 2);
	
	close (1);
	close (2);
	
	char buf[1024] = {0};
	int len = 0;
	while (1)
	{
		memset(&buf, 0, sizeof(buf));
		memcpy(buf, "zhazha", 6);
		fwrite(buf, sizeof(*buf), 1024, fp);
        sleep(1);
	}
	
	return 0;
}

















