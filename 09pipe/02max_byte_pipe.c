#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

#define TEST_MAX (68 * 1024)

int main(void)
{
	int ret = 0;
	int ncount = 0;
	

	char arr_send[TEST_MAX] = {0};
	char arr_send2[TEST_MAX] = {0};

	memset(arr_send, 'A', sizeof(arr_send));
	memset(arr_send2, 'B', sizeof(arr_send2));


	int pipefd[2] = { 0 };
	ret = pipe(pipefd);
	if (ret < 0)
	{
		ret = errno;
		perror("func err pipe: ");
		return ret;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		ret = errno;
		perror("func err fork: ");
		return ret;
	}
	else if (0 == pid)
	{
		close(pipefd[0]);
		pipefd[0] = 0;
		ret = write(pipefd[1], arr_send2, sizeof(arr_send2));
		printf("pid = %d, write bype is %d\n", getpid(), ret);
		exit(0);
	}

	pid = fork();
	if (pid < 0)
	{
		ret = errno;
		perror("func err fork: ");
		return ret;
	}
	else if (0 == pid)
	{
		close(pipefd[0]);
		pipefd[0] = 0;
		ret = write(pipefd[1], arr_send, sizeof(arr_send));
		printf("pid = %d, write bype is %d\n", getpid(), ret);
		exit(0);
	}

	close(pipefd[1]);
	pipefd[1] = 0;


	sleep(1);
	int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	char arr_rec[4096] = { 0 };
	int n = 1;
	while (1)
	{
		ret = read(pipefd[0], arr_rec, sizeof(arr_rec));
		if (ret == 0)
		{
			perror("func err read: ");
			break;
		}
		printf("n = %02d, pid = %d, write %d arr_rec[4095] = %c\n", n++, getpid(), ret, arr_rec[4095]);
		write(fd, arr_rec, ret);
	}
END:
	if (pipefd[0] != 0)
		shutdown(pipefd[0], SHUT_RDWR);
	if (pipefd[1] != 0)
		shutdown(pipefd[1], SHUT_RDWR);
	return ret;
}