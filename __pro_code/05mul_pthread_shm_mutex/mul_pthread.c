#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

#include "myipc_shm.h"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int g_key = 0x3333;

void *TestFunc(void * arg)
{
	int ncount = 0;
	int ret = 0;
	int shmhdl = 0;
	int *addr = NULL;

	pthread_mutex_lock(&mutex);
	ret = IPC_CreatShm(".", 0, &shmhdl);

	ret = IPC_MapShm(shmhdl, (void **)&addr);
	*((int *)addr) = *((int *)addr) + 1;
	ncount = *((int *)addr);
	printf("ncount:%d\n", ncount);
	ret = IPC_UnMapShm(addr);
	pthread_mutex_unlock(&mutex);
	printf("线程程正常退出 %d\n", (int)pthread_self());
	return NULL;
}

int main(void)
{
	int res;
	int procnum = 10;
	pthread_t tid[1024 * 10];


	int  i = 0, j = 0;

	printf("请输入要创建子进程的个数 : \n");
	scanf("%d", &procnum);

	//共享内存创建
	int		ret = 0;
	int 	shmhdl = 0;
	ret = IPC_CreatShm(".", sizeof(int), &shmhdl);
	if (ret != 0)
	{
		printf("func IPC_CreatShm() err:%d \n", ret);
		return ret;
	}

	for (i = 0; i < procnum; i++)
	{
		ret = pthread_create(&tid[i], NULL, TestFunc, NULL);
	}

	for (i = 0 ; i < procnum; i++)
	{
		pthread_join(tid[i], NULL);
	}
	IPC_DelShm(shmhdl);

	return 0;
}




