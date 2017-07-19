#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

int g_num = 1;

int nNum = 0;
int nLoop = 0;

typedef struct threadID
{
	int numID;
}threadID;

// 这里传输给线程的值的属于同一块内存空间
// 在我们将值发送过去的时候, 会发生值的改变, 在我们执行线程时值还是会改变
// 而我们创建线程的速度是很快的并且线程很多, 这时候这个值就会随着线程的创建直接给加上去
// 而这个值和多进程中的值不同, 线程的变量是相同的. 而进程的变量只是名字相同而已
// 并不在同一个内存空间中
// 这个和我们当初的信号可重入和不可重入的问题相似

void *pthread_routine_forkmalloc(void *arg)
{
	int i = 0;
	threadID *tmp = (threadID *)arg;
	for (i = 0; i < nLoop; i++)
	{
		printf("线程tid = %d, 第 %d 圈 numID = %d\n", (int)pthread_self(), i + 1, tmp->numID);
	}
	if (tmp != NULL)
	{
		free(tmp);
	}
	pthread_exit(0);
}


int main01(void)
{
	int ret = 0;
	int i = 0;
	pthread_t tidArray[1024 * 6];


	printf("请输入线程的数量: ");
	scanf("%d", &nNum);
	printf("请输入线程循环的个数: ");
	scanf("%d", &nLoop);

	// 这里传输给线程的值的属于同一块内存空间
	// 在我们将值发送过去的时候, 会发生值的改变, 在我们执行线程时值还是会改变
	// 而我们创建线程的速度是很快的并且线程很多, 这时候这个值就会随着线程的创建直接给加上去
	// 而这个值和多进程中的值不同, 线程的变量是相同的. 而进程的变量只是名字相同而已
	// 并不在同一个内存空间中
	// 这个和我们当初的信号可重入和不可重入的问题相似

	//memset(&tmp, 0, sizeof(tmp));
	//tmp.numID = i + 1;
	//ret = pthread_create(&tidArray[i], NULL, pthread_routine, &tmp);

	// 避免这种错误的方法 1 

	for (i = 0; i < nNum; i++)
	{
		threadID *p = malloc(sizeof(threadID));
		p->numID = i + 1;

		ret = pthread_create(&tidArray[i], NULL, pthread_routine_forkmalloc, p);
		if (ret < 0)
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


void *pthread_routine(void *arg)
{
	int i = 0;
	threadID *tmp = (threadID *)arg;
	for (i = 0; i < nLoop; i++)
	{
		printf("线程tid = %d, 第 %d 圈 numID = %d\n", (int)pthread_self(), i + 1, tmp->numID);
	}
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
		// 这里传输给线程的值的属于同一块内存空间
		// 在我们将值发送过去的时候, 会发生值的改变, 在我们执行线程时值还是会改变
		// 而我们创建线程的速度是很快的并且线程很多, 这时候这个值就会随着线程的创建直接给加上去
		// 而这个值和多进程中的值不同, 线程的变量是相同的. 而进程的变量只是名字相同而已
		// 并不在同一个内存空间中
		// 这个和我们当初的信号可重入和不可重入的问题相似

		//memset(&tmp, 0, sizeof(tmp));
		//tmp.numID = i + 1;
		//ret = pthread_create(&tidArray[i], NULL, pthread_routine, &tmp);

		// 避免这种错误的方法 2

		tmparr[i].numID = i + 1;
		
		ret = pthread_create(&tidArray[i], NULL, pthread_routine, &tmparr[i]);
		if (ret < 0)
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

