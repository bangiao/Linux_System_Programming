#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
       #include <sys/types.h>
       #include <sys/wait.h>

int mysystem(const char *command);

int main(void)
{
	//system("dir | wc -w");
	mysystem("dir | wc -w");
	return 0;
}

int mysystem(const char *command)
{
	int status = 0;
	pid_t pid;
	
	if (NULL == command)
	{
		return 1;
	}
	
	pid = fork();
	if (pid < 0)
	{
		status = -1;
	}
	else if (0 == pid)
	{
		execl("/bin/sh", "sh", "-c", command, NULL);
		exit(127);
	}
	else
	{
		while (waitpid(pid, &status, 0) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			status = -1;
			break;
		}
	}
	
	
	return status;
}