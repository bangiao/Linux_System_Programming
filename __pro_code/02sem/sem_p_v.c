#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#include "sem_p_v.h"

union semun
{
	int              val;
	struct semid_ds *buf;
	unsigned short  *array;
	struct seminfo  *__buf;
};

// 创建当前信号
int sem_creat(int key, int *semid)
{
	int ret = 0;
	int tmp_semid = 0;
	if (NULL == semid)
	{
		printf("funce rr sem_creat param\n");
		return -1;
	}
	tmp_semid = semget(key, 1, 0666 | IPC_CREAT);
	if (tmp_semid < 0)
	{
		ret = errno;
		perror("func sem_creat semget err: ");
		return ret;
	}
	*semid = tmp_semid;
	return ret;
}

// 打开已有的信号
int sem_open(int key, int *semid)
{
	int ret = 0;
	int tmp_semid = 0;
	if (NULL == semid)
	{
		printf("funce rr sem_creat param\n");
		return -1;
	}
	tmp_semid = semget(key, 1, 0666);
	if (tmp_semid < 0)
	{
		ret = errno;
		perror("func sem_open semget err: ");
		return ret;
	}
	*semid = tmp_semid;
	return ret;
}

// 设置当前信号资源的数量
int sem_setval(int semid, int val)
{
	int ret = 0;
	union semun su;
	su.val = val;
	ret = semctl(semid, 0, SETVAL, su);
	if (ret < 0)
	{
		perror("func sem_setval semctl err semctl");
		return -1;
	}
	return ret;
}

// 读取当前信号资源的数量
int sem_getval(int semid, int *val)
{
	int ret = 0;
	union semun su;
	if (NULL == val)
	{
		printf("func err sem_getval\n");
		return -1;
	}
	ret = semctl(semid, 0, GETVAL, su);
	if (ret < 0)
	{
		perror("func sem_getval semctl err semctl");
		return ret;
	}
	*val = su.val;
	return ret;
}

// 信号的 P 加锁操作 就是信号量的val --
int sem_p(int semid)
{
	int ret = 0;
	struct sembuf sops = { 0, -1, 0 };
	ret = semop(semid, &sops, 1);
	if (ret < 0)
	{
		ret = errno;
		perror("func err sem_p semop if (ret < 0)\n");
		return ret;
	}
	return ret;
}

// 信号的 V 解锁操作, 就是信号量的val ++
int sem_v(int semid)
{
	int ret = 0;
	struct sembuf sops = { 0, 1, 0 };
	ret = semop(semid, &sops, 1);
	if (ret < 0)
	{
		ret = errno;
		perror("func err sem_v semop if (ret < 0)\n");
		return ret;
	}
	return ret;
}

// int main(void)
// {
// 	int ret = 0;
// 	int semid = 0;
// 	int val = 0;
// 
// 	key_t key = ftok(".", 'a');
// 	sem_creat(key, &semid);
// 	//semid = sem_open(0x11111);
// 	sem_setval(semid, 1);	// 信号量的资源数量 
// 	sem_getval(semid, &val);
// 	printf("val = %d\n", val);
// 
// 	pid_t pid = fork();
// 
// 	sem_p(semid);
// 	printf("pid = %d, val = %d\n", pid, val);
// 	sleep(3);
// 	sem_v(semid);
// 
// 	return 0;
// }