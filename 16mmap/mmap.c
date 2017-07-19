/*************************************************************************
	> File Name: mmap.c
	> Author: 
	> Mail: 
	> Created Time: 2017年04月25日 星期二 21时19分29秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	int ret = 0;
	int fd = 0;
	
	fd = open("testfile", O_RDWR);
	if (-1 == fd)
	{
		perror("func err open: ");
		return -1;
	}
	
	char *p_mmap = NULL;
	size_t length = 1024;
	
	p_mmap = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == p_mmap)
	{
		perror("func err mmap: ");
		return -2;
	}
	
	char buf[1024] = {0};
	
	strcpy(buf, p_mmap);
	printf("buf = %s\n", buf);
	
	//bzero(p_mmap, strlen(p_mmap));
	// memset(p_mmap, ' ', strlen(p_mmap) + 1);
	
	memcpy(p_mmap, "zhazha", strlen("zhazha") + 1);

	ret = munmap(p_mmap, length);
	if (-1 == ret)
	{
		perror("func err munmap: ");
		return -3;
	}
	if (0 != fd)
	{
		close(fd);
		fd = 0;
	}
	return 0;
}
