#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

int mydaemon()
{
	int ret = 0;
	pid_t pid = fork();
	if (pid < 0)
	{
		ret = errno;
		perror("func fork err: ");
		return ret;
	}
	else if (pid > 0)
	{
		exit(0);
	}

	ret = setsid();
	if (ret < 0)
	{
		ret = errno;
		perror("func setsid err: ");
		exit(ret);
	}

	ret = chdir("/");
	if (ret < 0)
	{
		ret = errno;
		perror("func chdir err: ");
		exit(ret);
	}

	umask(0022);

	close(0);
	open("/dev/null", O_RDWR);
	ret = dup2(0, 1);
	if (ret < 0)
	{
		ret = errno;
		perror("func dup2 err: ");
		exit(ret);
	}
	ret = dup2(0, 2);
	if (ret < 0)
	{
		ret = errno;
		perror("func dup2 err: ");
		exit(ret);
	}
	return ret;
}

int main(void)
{
	int ret = 0;
	time_t tm_ret;
	time_t tm_t;
	struct tm *tm_time;
	int fd = 0;
	char arr_time[1024] = { 0 };

	fd = open("time.log", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0)
	{
		ret = errno;
		fprintf(stderr, "func err open");
		return ret;
	}

	mydaemon();

	while (1)
	{
		//sleep(3);
		tm_ret = time(&tm_t);
		tm_time = localtime(&tm_t);
		sprintf(arr_time, "year: %d, month: %d, day: %d, hour: %d, min: %d, sec: %d\t\n\n",
			tm_time->tm_year, tm_time->tm_mon, tm_time->tm_mday, tm_time->tm_hour,
			tm_time->tm_min, tm_time->tm_sec);
		do 
		{
			ret = write(fd, arr_time, strlen(arr_time));
		} while (errno == EINTR);
		if (ret < 0)
		{
			ret = errno;
			perror("func err write: ");
			return ret;
		}
	}

	return 0;
}
