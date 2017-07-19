#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

int g_num = 1;

void *pthread_routine(void *arg)
{
	int i = 0;
	
	pthread_detach(pthread_self());
	printf("\ng_num = %d 显示线程 ID = %d\n", g_num, (int)pthread_self());
	printf("I am pthread ... \n");
	for (i = 0; i < 20; i++)
	{
		printf("B");
		fflush(stdout);
	}
	puts("");
	printf ("子线程睡眠3秒后....退出了\n");
	//exit(0);
    pthread_exit(NULL);
	return NULL;
}

int main(void)
{
	int ret = 0;
	int i = 0;
	pthread_t tid;

	ret = pthread_create(&tid, NULL, pthread_routine, NULL);
	if (ret < 0)
	{
		ret = -1;
		printf("func err pthread_create ... \n");
		return ret;
	}
	for (i = 0; i < 20; i++)
	{
		printf("A");
		fflush(stdout);
	}
	puts("");
	pthread_join(tid, NULL);
    sleep(1);
	printf("主进程也结束\n");
	return 0;
}
