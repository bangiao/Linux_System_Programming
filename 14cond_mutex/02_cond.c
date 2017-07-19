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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *my_routine(void *arg)
{
	int ret = 0;
	int num = (int)arg;
	printf("my_routine 线程ID = %d 线程互斥量准备启动...\n", num);
	ret = pthread_mutex_lock(&mutex);
	printf("线程ID = %d\n", num);
	printf("线程ID = %d pthread_cond_signal...\n", num);
	//ret = pthread_cond_wait(&cond, &mutex);
	ret = pthread_cond_signal(&cond);		// 这个函数是不阻塞的
	printf("线程ID = %d pthread_cond_signaled...ret = %d\n", num, ret);
	
	ret = pthread_mutex_unlock(&mutex);
	printf("线程ID = %d 线程互斥量解锁成功...\n", num);
	return NULL;
}

void *my_routine1(void *arg)
{
	int ret = 0;
	int num = (int)arg;
	printf("my_routine1 线程ID = %d 线程互斥量准备启动...\n", num);
	ret = pthread_mutex_lock(&mutex);
	printf("线程ID = %d\n", num);
	printf("线程ID = %d pthread_cond_wait...\n", num);
	// 这个函数有两个线程调用, 但是如果一个线程没有进入 wait 状态的话, 另一个线程
	// 没有机会进入 mutex 内部, 也就是说 cond wait 就是一把解开 mutex 的锁, 其中并不是简单的 
	// Sleep 那么简单, 因为 sleep 了, 另一个线程也只能在 mutex 外阻塞着, 但是 cond wait 是不同的
	
	// 现象
	// 	my_routine1 线程ID = 0 线程互斥量准备启动...
	//	线程ID = 0
	//	线程ID = 0 pthread_cond_wait...
	//	my_routine1 线程ID = 1 线程互斥量准备启动...
	//	线程ID = 1
	//	线程ID = 1 pthread_cond_wait...

	ret = pthread_cond_wait(&cond, &mutex);		// 这个函数可能被信号强制退出
	//ret = pthread_cond_signal(&cond);
	printf("线程ID = %d pthread_cond_waited...ret = %d\n", num, ret);
	ret = pthread_mutex_unlock(&mutex);
	printf("线程ID = %d 线程互斥量解锁成功...\n", num);
	return NULL;
}


int main(void)
{
	int ret = 0;
	pthread_t tid[4];
	int i = 0;
	
	
	// 先 wait 
	ret = pthread_create(&tid[i++], NULL, my_routine1, (void *)i);
	ret = pthread_create(&tid[i++], NULL, my_routine1, (void *)i);
	
	int n = 1;
	do
	{
		n = sleep(n);
	}while (n > 0);// 先要 wait 因为 pthread_cond_signal 不是阻塞函数
	
	// 后 signal
	ret = pthread_create(&tid[i++], NULL, my_routine, (void *)i);
	ret = pthread_create(&tid[i++], NULL, my_routine, (void *)i);

	for (i = 0; i < 4; i++)
	{
		ret = pthread_join(tid[i], NULL);
	}

	return 0;
}