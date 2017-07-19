#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <pthread.h>


#include "comsock.h"
#include "myipc_shm.h"
#include "myipc_sem.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
信号量相当于线程锁
共享内存存放 线程数 * 循环数量
*/

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)
        
void handle(int signum)
{
	int pid = 0;
	printf("recv signum:%d \n", signum);
	
	//避免僵尸进程
	while ((pid = waitpid(-1, NULL, WNOHANG) ) > 0)
	{
		printf("退出子进程pid%d \n", pid);
		fflush(stdout);
	} 
}


// 创建共享内存和信号量
int srv_init()
{
	//共享内存创建
	int		ret = 0;
	int 	shmhdl = 0;
	key_t 	key = 0;
	ret = IPC_CreatShm(".", sizeof(int), &shmhdl);
	if (ret != 0)
	{
		printf("func IPC_CreatShm() err:%d \n", ret);
		return ret;
	}
	
	key = ftok(".", 'z');
	//信号量的创建
 	int  	semid = 0;
	ret = sem_creat(key, &semid);
	if (ret != 0)
	{
		printf("func sem_creat() err:%d,重新按照open打开信号酿\n", ret);
		if (ret == SEMERR_EEXIST)
		{
			ret = sem_open(key, &semid);
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
	if (ret != 0 )
	{
		printf("func sem_getval() err:%d \n", ret);
		return ret;
	}
	printf("sem val:%d\n", val);
	return ret;
}

void TestFunc_sem()
{
	int ncount = 0;
	int ret = 0;
	int shmhdl = 0;
	int *addr = NULL;
	key_t 	key = 0;
	
	int semid = 0;
	
	key = ftok(".", 'z');
	sem_open(key, &semid);

 	sem_p(semid); //临界区开奿
	ret = IPC_CreatShm(".", 0, &shmhdl);
	ret =IPC_MapShm(shmhdl, (void **)&addr);
	*((int *)addr) =  *((int *)addr)  + 1;
	ncount = *((int *)addr);
	printf("ncount:%d\n", ncount);
	ret =IPC_UnMapShm(addr);
	sem_v(semid);  //临界区开奿
}	


void TestFunc_mutex()
{
	int ncount = 0;
	int ret = 0;
	int shmhdl = 0;
	int *addr = NULL;
	key_t 	key = 0;
	
	int semid = 0;
	
	key = ftok(".", 'z');
	sem_open(key, &semid);

 	pthread_mutex_lock(&mutex);
	ret = IPC_CreatShm(".", 0, &shmhdl);
	ret =IPC_MapShm(shmhdl, (void **)&addr);
	*((int *)addr) =  *((int *)addr)  + 1;
	ncount = *((int *)addr);
	printf("ncount:%d\n", ncount);
	ret =IPC_UnMapShm(addr);
	pthread_mutex_unlock(&mutex);
}	

void *Malloc_my_routine(void *arg)
{
	int ret = 0;
	ret = pthread_detach(pthread_self());
	if (NULL == arg)
	{
		printf("fucn err my_routine param err\n");
		return NULL;
	}
	int connfd = *((int*)arg);
	free(arg);


	char			recvbuf[1024];
	unsigned int  	recvbuflen = 1024;

	while(1)
	{
		memset(recvbuf, 0, sizeof(recvbuf));
		ret = sck_server_recv(connfd, 0, recvbuf, &recvbuflen);
		if (ret >= 3000)
		{
			printf("func sck_server_recv() err:%d \n", ret);
			break; 
		}
		
		//TestFunc_sem();
		TestFunc_mutex();
		printf("recvbuf = %s\n", recvbuf);
		
		ret = sck_server_send(connfd, 0, recvbuf, recvbuflen);
		if (ret >= 3000)
		{
			printf("func sck_server_send() err:%d \n", ret);
			break;
		}
	}
	close(connfd);
	
	return NULL;
}

// 使用 (void *)&connfd 不是不以的
// void *my_routine(void *arg)
// {
// 	int ret = 0;
// 	if (NULL == arg)
// 	{
// 		printf("fucn err my_routine param err\n");
// 		return NULL;
// 	}
// 	ret = pthread_detach(pthread_self());
// 	int connfd = *((int*)arg);
// 
// 	char			recvbuf[1024];
// 	unsigned int  	recvbuflen = 1024;
// 
// 	while(1)
// 	{
// 		memset(recvbuf, 0, sizeof(recvbuf));
// 		ret = sck_server_recv(connfd, 0, recvbuf, &recvbuflen);
// 		if (ret >= 3000)
// 		{
// 			printf("func sck_server_recv() err:%d \n", ret);
// 			break; 
// 		}
// 		
// 		TestFunc_sem();
// 		printf("recvbuf = %s\n", recvbuf);
// 		
// 		ret = sck_server_send(connfd, 0, recvbuf, recvbuflen);
// 		if (ret >= 3000)
// 		{
// 			printf("func sck_server_send() err:%d \n", ret);
// 			break;
// 		}
// 	}
// 	close(connfd);
// 	
// 	return NULL;
// }


int main(void)
{
	int		ret = 0;
	int 	listenfd = 0;
	char	ip[1024] = {0};
	int		port = 0;
	int		connfd = 0;
	
	signal(SIGCHLD, handle);
	signal(SIGPIPE, SIG_IGN);
	
	ret = srv_init();
	
	ret = sck_server_init(8001, &listenfd);
	if (ret != 0)
	{
		printf("sckServer_init() err:%d \n", ret);
		return ret;
	}
	
	while(1)
	{
		int ret = 0;
		int wait_seconds = 5;
		

		ret = sck_server_accept(listenfd, wait_seconds, &connfd, ip, &port);
		if (ret >= 3000)
		{
			printf("timeout....\n");
			continue;
		}
		printf("客户端连接成功...\n");
		
		pthread_t tid = 0;
		
		int *pCon = NULL;
		pCon = (int *)malloc(sizeof(int));
		*pCon = connfd;
		ret = pthread_create(&tid, NULL, Malloc_my_routine, (void *)pCon);
		// 使用下面这种方法的话, 这个时候 connfd 不稳定, 可能已经不是原本的值了
		//ret = pthread_create(&tid, NULL, my_routine, (void *)&connfd);
	}
	
	sck_server_close(listenfd);
	sck_server_close(connfd);
	return 0;
}
