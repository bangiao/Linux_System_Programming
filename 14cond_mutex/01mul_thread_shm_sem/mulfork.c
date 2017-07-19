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

#include "myipc_sem.h"
#include "myipc_shm.h"

int g_key = 0x3333;

void *my_routine(void *arg)
{
	int ret = 0;
	int i = 0;
	int semid = 0;
	sem_open(g_key, &semid);
	sem_p(semid);
	int loopnum = (int)arg;
	for (i = 0; i < loopnum; i++)
	{
		printf("线程循环 %d 圈\n", loopnum);
	}
	sem_v(semid);
	return NULL;
}

int main(void)
{
	int res;
	int procnum = 10;
	int loopnum = 100;

	pthread_t tid[1024];

	int  i = 0;

	printf("请输入要创建子进程的个数 : \n");
	scanf("%d", &procnum);

	printf("请输入让每个子进程测试多少次 :\n");
	scanf("%d", &loopnum);

	//共享内存创建
	int		ret = 0;
	int 	shmhdl = 0;
	ret = IPC_CreatShm(".", sizeof(int), &shmhdl);
	if (ret != 0)
	{
		printf("func IPC_CreatShm() err:%d \n", ret);
		return ret;
	}

	//信号量的创建
	int  	semid = 0;
	ret = sem_creat(g_key, &semid);
	if (ret != 0)
	{
		printf("func sem_creat() err:%d,重新按照open打开信号酿\n", ret);
		if (ret == SEMERR_EEXIST)
		{
			ret = sem_open(g_key, &semid);
			if (ret != 0)
			{
				printf("按照打开的方式，重新获取sem失败:%d \n", ret);
				return ret;
			}
		}
		else
		{
			return ret;
		}

	}

	int  val = 0;
	ret = sem_getval(semid, &val);
	if (ret != 0)
	{
		printf("func sem_getval() err:%d \n", ret);
		return ret;
	}
	printf("sem val:%d\n", val);

	for (i = 0; i < procnum; i++)
	{
		ret = pthread_create(&tid[i], NULL, my_routine, (void *)loopnum);
	}

	for (i = 0; i < procnum; i++)
	{
		ret = pthread_join(tid[i], NULL);
		printf("线程结束成功 tid = %d\n", tid[i]);
	}

	return 0;
}




