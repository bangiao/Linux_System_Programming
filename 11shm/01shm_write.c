#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


int main01(void)
{
	int ret = 0;
	key_t key = ftok("./key", 'a');
	ret = shmget(key, 0x400 * 12, 0666 | IPC_CREAT);
	if (ret < 0)
	{
		ret = errno;
		perror("func err shmget: ");
		return ret;
	}

	return 0;
}


int main02(void)
{
	int ret = 0;
	key_t key = ftok("./key", 'a');
	ret = shmget(key, 0x400 * 12, 0666 | IPC_CREAT | IPC_EXCL);
	if (ret < 0)
	{
		ret = errno;
		if (errno == EEXIST)
		{
			printf("file is eexist\n");
		}
		perror("func err shmget: ");
		return ret;
	}

	return 0;
}


typedef struct Teacher 
{
	char shmbuf[64];
	int age;
}Teacher;


int main(void)
{
	int ret = 0;
	int i = 0;
	int shmid = 0;
	key_t key = ftok("./key", 'a');
	// 这句话类似于创建了一个共享内存, malloc 类似
	shmid = shmget(key,sizeof(Teacher), 0666 | IPC_CREAT);
	if (shmid < 0)
	{
		ret = errno;
		perror("func err shmget: ");
		return ret;
	}

	// 这句话的意思就是连接共享内存将地址抛出, 让我们去读
	void *p = NULL;
	p = shmat(shmid, NULL, 0);
	Teacher t1 = { "aaaa", 12 };

	for (i = 0; i < 5; i++)
	{
		memcpy(p, &t1, sizeof(t1));
		memcpy(p + sizeof(t1) * i, &t1, sizeof(t1));
	}

	printf("input 1 is del, 0 is pause and other is exit\n");
	int num;
	scanf("%d", &num);
	if (1 == num)
	{
		shmdt(p);
		shmctl(shmid, IPC_RMID, NULL);
	}
	else if (0 == num)
	{
		pause();
	}
	else
	{
		shmdt(p);
		//shmctl(shmid, IPC_RMID, NULL);
	}

	return 0;
}
