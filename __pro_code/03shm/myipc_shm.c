#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>

#include "myipc_shm.h"

int shmflag = 0;
int shmkey = 0;

int IPC_CreatShm(char *shmfilepath, int shmsize, int *shmhande)
{
	if (0 == shmflag)
	{
		shmkey = ftok(shmfilepath, 'c');
		if (-1 == shmkey)
		{
			perror("func err ftok: ");
			return -1;
		}
		shmflag = 1;
	}
	*shmhande = shmget(shmkey, shmsize, IPC_CREAT | 0666);
	if (-1 == shmhande)
	{
		perror("func err shmget: ");
		return -2;
	}
	return 0;
}

int IPC_MapShm(int shmhande, void **mapaddr)
{
	void *tempptr = NULL;
	tempptr = (void *)shmat(shmhande, 0, SHM_RND);
	if ((int)tempptr == -1)
	{
		return -1;
	}
	*mapaddr = tempptr;
	return 0;
}

int IPC_UnMapShm(void *unmapaddr)
{
	int ret = 0;
	ret = shmdt((char *)unmapaddr);
	if (-1 == ret)
	{
		return -1;
	}
	return 0;
}

int IPC_DelShm(int shmhande)
{
	int ret = 0;
	ret = shmctl(shmhande, IPC_RMID, NULL);
	if (ret < 0)
	{
		return -1;
	}
	return 0;
}