#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

int g_num = 1;

typedef struct Teacher
{
	char name[64];
	int age;
}Teacher;

void *pthread_routine(void *arg)
{
	Teacher *p = (Teacher *)arg;
	pthread_detach(pthread_self());
	printf("p->name = %s, p->age = %d\n", p->name, p->age);
	(p->age)++;
	printf ("子线程睡眠3秒后....退出了\n");
    pthread_exit(arg);
	//return arg;
}

void *pthread_routine_err(void *arg)
{
	// 这种最怕的就是在这里面分配内存了
	Teacher t1 = { "zgazga", 222 };
	return &t1;
}

int main(void)
{
	int ret = 0;
	int i = 0;
	pthread_t tid;
	Teacher t1 = {"zhazha", 33};
	Teacher *p = NULL;
	
	ret = pthread_create(&tid, NULL, pthread_routine, &t1);
	//ret = pthread_create(&tid, NULL, pthread_routine_err, &t1);
	if (ret < 0)
	{
		ret = -1;
		printf("func err pthread_create ... \n");
		return ret;
	}
	

	pthread_join(tid, &p);
	printf("p->age = %d, p->name = %s\n", p->age, p->name);
    sleep(1);
	printf("主进程也结束\n");
	return 0;
}
