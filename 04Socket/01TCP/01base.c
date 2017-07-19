#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <arpa/inet.h>

/*
	测试主机是大端还是小端
*/

// 在 x86平台是小端模式, 在 IBM 在大端模式 
// arm 是可设置的

int main(void)
{
	// 12345678 从高到低 所以 78 是低
	int a = 0x12345678;
	char *p = (char *)&a;
	
	printf("%x, %x, %x, %x\n", p[0], p[1], p[2], p[3]);
	if (0x12 == p[0])
	{
		printf("大端\n");
	}
	else if (0x78 == p[0])
	{
		printf("小端\n");
	}
	
	uint32_t ret = htonl(a);
	p = (char *)&ret;
	if (0x12 == p[0])
	{
		printf("大端\n");
	}
	else if (0x78 == p[0])
	{
		printf("小端\n");
	}
	return 0;
}















