#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include "myipc_sem.h"
#include "myipc_shm.h"

int key = 0x3333;
void TestFunc (int num, int loopnum)
{
	 int ncount = 0;
	int ret = 0;
	int shmhdl = 0;
	int *addr = NULL;
	
	int semid = 0;
	sem_open(key, &semid);
 	sem_p(semid); //临界区开始

		ret = IPC_CreatShm(".", 0, &shmhdl);
		
		ret =IPC_MapShm(shmhdl, (void **)&addr);
		*((int *)addr) =  *((int *)addr)  + 1;
		ncount = *((int *)addr);
		printf("ncount:%d\n", ncount);
		//addr[0] = addr[0] +1;
		ret =IPC_UnMapShm(addr);
		sleep(2);
		
	sem_v(semid);  //临界区开始
    printf("I am sun fork %d ,loopnum:%d\n",num,loopnum);	
    printf("sun  fork num :%d exit \n",num );
}
void mul_fork()
{
	int num , loopnum ;
	printf("please input fork num:\n");
	scanf("%d",&num );
	printf("please input fork loopnum:\n");
	scanf("%d",&loopnum );
	pid_t pid;
	int i, j;
	for(i = 0; i< num; i++)
	{
		pid = fork();
		if(pid == 0)
		{
			for(j = 0; j< loopnum ; j++)
			{
				TestFunc ( i, j+1);
			}
			exit(0);//子进程退出，
		}
		 
	}	
}
int main()
{
	printf("hello wudunxiong\n");
	int ret;
	int shmid ;
	//char *shmseedfile
	ret =  IPC_CreatShm("." ,1024,&shmid );//创建共享内存
	if(ret != 0 )
	{
		 ret = -1;
		 printf("IPC_CreatShm error :%d\n",ret );
		 return ret;
	}
	int semid;
	ret =  sem_creat( key,  &semid);//创建信号量
	if(ret != 0 )
	{
		printf("func sem_creat() err:%d,重新按照open打开信号量 \n", ret);
		if (ret == SEMERR_EEXIST)
		{
			ret = sem_open(key, &semid);//如果信号已经创建，那就直接打开
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
	int val = 0;
	ret = sem_getval( semid, &val);
	if(ret != 0 )
	{
		 ret = -3;
		 printf("func sem_getval error :%d\n",ret );
		 return ret;
	}
	printf("sem val:%d\n",val);
	mul_fork();//创建多进程
	sleep(5);
/*	int mypid = 0;
	  while ( (mypid= waitpid(-1, NULL, WNOHANG)) > 0)
	  {
	  	//printf("退出的子进程pid为:mypid:%d \n", mypid);
	  	;
	  }*/
	printf("father fork  exit \n");
    return 0;	
}

