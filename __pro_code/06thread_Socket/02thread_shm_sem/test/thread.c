#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>


void *my_routine(void *arg)
{
	int fd = 0;
	fd = *(int *)arg;
	printf("my_routine say fd = %d\n", fd);
	return NULL;
}


int main(void)
{
	int ret = 0;
	pthread_t tid;
	int fd = 0;

	fd = open("text.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (-1 == fd)
	{
		ret = errno;
		perror("func err open: ");
		return ret;
	}
	printf("main say fd  = %d\n", fd);
	ret = pthread_create(&tid,NULL, my_routine, (void *)&fd);
	if (0 != ret)
	{
		printf("func err pthread_create if (0 != ret) ret = %d\n", ret);
		return ret;
	}

	ret = pthread_join(tid, NULL);

	close(fd);
	fd = 0;

	return 0;
}