#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main01(void)
{
	int msgid = 0;
	// 这样子写表示在内存中读取消息队列, 如果不存在则表示错误
	msgid = msgget(0x1234, 0666);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("消息队列读取失败, 消息队列不存在\n");
		}
		perror("func err msgget: ");
		return -1;
	}

	system("pause");
	return 0;
}

int main02(void)
{
	int msgid = 0;
	// 这样子写表示在内存中读取或者创建消息队列
	// 如果消息队列不存在的话就创建一个消息队列
	// 如果存在的话就读取消息队列
	msgid = msgget(0x1234, 0666 | IPC_CREAT);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("消息队列读取失败, 消息队列不存在\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("消息队列创建成功\n");

	return 0;
}

// IPC_CREAT and IPC_EXCL
// 上面两个同事存在的时候便是, 当共享内存存在的时候 报文件已经存在
// IPC_EXCL 单独用它没有用
int main03(void)
{
	int msgid = 0;
	msgid = msgget(0x1234, 0666 | IPC_CREAT | IPC_EXCL);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("消息队列读取失败, 消息队列不存在\n");
		}
		if (errno == EEXIST)
		{
			printf("消息队列已经存在\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("msgid = %d\n", msgid);
	printf("消息队列创建成功\n");

	return 0;
}

// IPC_PRIVATE 这个创建的消息队列只能在自己的进程家族中用, 
// 而且可以多次调用, 每次的消息队列的 id 都不一样
// IPC_PRIVATE 是个 宏 而且值为 0
int main04(void)
{
	int msgid = 0;
	msgid = msgget(IPC_PRIVATE, 0666);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("消息队列读取失败, 消息队列不存在\n");
		}
		if (errno == EEXIST)
		{
			printf("消息队列已经存在\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("msgid = %d\n", msgid);
	printf("消息队列创建成功\n");

	return 0;
}

// 权限位的使用
int main(void)
{
	int msgid = 0;
	msgid = msgget(0x1234, 0444 | IPC_CREAT);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("消息队列读取失败, 消息队列不存在\n");
		}
		if (errno == EEXIST)
		{
			printf("消息队列已经存在\n");
		}
		if (errno == EACCES)
		{
			printf("EACCES err\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("msgid = %d\n", msgid);
	printf("消息队列创建成功\n");

	msgid = msgget(0x1234, 0644);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("消息队列读取失败, 消息队列不存在\n");
		}
		if (errno == EEXIST)
		{
			printf("消息队列已经存在\n");
		}
		if (errno == EACCES)
		{
			printf("EACCES err\n");
		}
		perror("func err msgget: ");
		return -1;
	}

	return 0;
}

