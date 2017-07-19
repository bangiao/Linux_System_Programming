#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>


int g_num = 0;
int nNum = 0;
int nLoop = 0;

typedef struct threadID
{
	int numID;
}threadID;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


/*
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
*/

void *pthread_routine(void *arg)
{
	threadID tmp = *(threadID *)arg;
	pthread_mutex_lock(&mutex); 
	++g_num;
	printf("tid = %d, g_num = %d, tmp.numID = %d\n", (int)pthread_self(), g_num, tmp.numID);
	pthread_mutex_unlock(&mutex);
	pthread_exit(0);
}

int main(void)
{
	int ret = 0;
	int i = 0;
	pthread_t tidArray[1024 * 6];
	threadID tmp;
	threadID tmparr[2048];


	printf("请输入线程的数量: ");
	scanf("%d", &nNum);
	printf("请输入线程循环的个数: ");
	scanf("%d", &nLoop);

	for (i = 0; i < nNum; i++)
	{
		tmparr[i].numID = i + 1;
		ret = pthread_create(&tidArray[i], NULL, pthread_routine, &tmparr[i]);
		if (ret != 0)
		{
			ret = -1;
			printf("func err pthread_create ... \n");
			return ret;
		}
	}

	for (i = 0; i < nNum; i++)
	{
		pthread_join(tidArray[i], NULL);
	}
	printf("父进程结束\n");

	return 0;
}

