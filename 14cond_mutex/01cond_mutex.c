#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

/*
	这个程序要注意, 先启动的线程一定要先 调用 pthread_cond_wait 
	后启动的线程要再 调用 pthread_cond_signal 
	因为 pthread_cond_signal 这个函数不是阻塞的函数
*/

#define CONSUME_COUNT 2
#define PRODUCE_COUNT 2

int g_count = 0;

typedef struct threadID
{
	int numID;
}threadID;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// 消费者
void * consume(void *arg)
{
	int inum = 0;
	inum = (int)arg;
	// 因为他是死循环的, 所以看不出来有变化
	while (1)
	{
		pthread_mutex_lock(&mutex);
		while (g_count <= 0)
		{
			printf("%d 号消费者开始等待消费 g_count = %d\n", inum, g_count);
			pthread_cond_wait(&cond, &mutex);
			printf("%d 号消费者开始消费 g_count = %d\n", inum, g_count);
		}
		g_count--;
		printf("%d 号消费者消费完毕...g_count = %d\n", inum, g_count);
		pthread_mutex_unlock(&mutex);
	}
	
	pthread_exit(0);
}

// 生产者
void * produce(void *arg)
{
	int inum = 0;
	inum = (int)arg;
	while (1)
	{
		pthread_mutex_lock(&mutex);
		if (g_count > 20)
		{
			printf("生产的产品太多需要控制下 休眠....\n");
			sleep(1);
			continue;
		}
		pthread_mutex_unlock(&mutex);
		sleep(1);
		pthread_mutex_lock(&mutex);
		g_count++;
		printf("%d 号生产者生产完毕... g_count = %d\n", inum, g_count);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

int main(void)
{
	int				ret = 0;
	int				i = 0;
	pthread_t		tidArray[CONSUME_COUNT + PRODUCE_COUNT];

	// 创建两个生产者
	for (i = 0; i < PRODUCE_COUNT; i++)
	{
		ret = pthread_create(&tidArray[i + CONSUME_COUNT], NULL, produce, (void *)i);
	}
	
	// 创建两个消费者
	for (i = 0; i < CONSUME_COUNT; i++)
	{
		ret = pthread_create(&tidArray[i], NULL, consume, (void *)i);
	}

	for (i = 0; i < CONSUME_COUNT + PRODUCE_COUNT; i++)
	{
		pthread_join(tidArray[i], NULL);
	}
	printf("父进程结束\n");

	return 0;
}

