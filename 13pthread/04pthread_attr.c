/*************************************************************************
	> File Name: 04pthread_attr.c
	> Author: 
	> Mail: 
	> Created Time: 2017年04月03日 星期一 11时06分28秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

int main(void)
{
	int ret = 0;
	pthread_attr_t pthread_attr;

	ret = pthread_attr_init(&pthread_attr);
	if (0 != ret)
	{
		printf("func err pthread_attr_init if (0 != ret) ret = %d\n", ret);
		return ret;
	}

	int state;
	
	 // 获得线程分离属性
	pthread_attr_getdetachstate(&pthread_attr, &state);
	if (state == PTHREAD_CREATE_JOINABLE)
	{
		printf("detachstate: PTHREAD_CREATE_JOINABLE\n");
	}
	else if (state == PTHREAD_CREATE_DETACHED)
	{
		printf("detachstate: PTHREAD_CREATE_DETACHED\n");
	}

	size_t size;
	pthread_attr_getstacksize(&pthread_attr, &size);
	printf("栈大小 size = %d\n", (int)size);

	pthread_attr_destroy(&pthread_attr);

	return 0;
}
