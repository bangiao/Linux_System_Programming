
#define	_OS_LINUX_

#if defined _OS_LINUX_
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include "myipc_shm.h" 

#endif

int shmflag = 0;
int shmkey;


int IPC_CreatShm(char *shmseedfile, int shmsize, int *shmhdl)
{
    if(shmflag == 0)
    {
    	shmkey = ftok(shmseedfile, 'c');
    	if (shmkey == -1)
    	{
    		perror("ftok");
    		return -1;
    	}
    	    
    	shmflag = 1;
    }
    
    *shmhdl = shmget(shmkey,shmsize,IPC_CREAT|0666);
    if (*shmhdl == -1)
		return -2;
	return 0;

}

int IPC_MapShm(int  shmhdl, void  **mapaddr)
{
    void *tempptr = NULL;

    tempptr = (void *)shmat(shmhdl,0,SHM_RND);
    if ((int)tempptr == -1)
		return -1;
    *mapaddr = tempptr;
    return 0;
}

int IPC_UnMapShm(void *unmapaddr)
{
    int  rv;

    rv = shmdt((char *)unmapaddr);
    if (rv == -1)
		return -1;
    return 0;
}

int IPC_DelShm(int shmhdl)
{
    int  rv;

    rv = shmctl(shmhdl,IPC_RMID,NULL);
    if(rv < 0)
		return -1;
    return 0;
}
