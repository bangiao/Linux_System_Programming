#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


typedef struct Teacher 
{
	char name[64];
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
	Teacher *p = NULL;
	p = shmat(shmid, NULL, 0);
	Teacher t1 = { "aaaa", 12 };

	printf("p->name = %s\n", p->name);
	printf("p->age = %d\n", p->age);
	printf("p->name = %s\n", p[1].name);
	printf("p->age = %d\n", p[1].age);


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