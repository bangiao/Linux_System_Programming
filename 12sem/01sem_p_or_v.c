#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun 
{
	int              val;
	struct semid_ds *buf;
	unsigned short  *array;
	struct seminfo  *__buf;
};

int sem_creat(key_t key)
{
	int ret = 0;
	int semid = 0;
	semid = semget(key, 1, 0666 | IPC_CREAT);
	if (ret < 0)
	{
		ret = -1;
		perror("func semget err: ");
		return ret;
	}
	return semid;
}

// �����е��ź�
int sem_open(key_t key)
{
	int ret = 0;
	int semid = 0;
	semid = semget(0x11111, 1, 0666);
	if (ret < 0)
	{
		ret = -1;
		perror("func semget err: ");
		return ret;
	}
	return semid;
}

// ���õ�ǰ�ź���Դ������
int sem_setval(int semid, int val)
{
	int ret = 0;
	union semun su;
	su.val = val;
	ret = semctl(semid, 0, SETVAL, su);
	if (ret < 0)
	{
		perror("func err semctl");
		return -1;
	}
	return ret;
}

// ��ȡ��ǰ�ź���Դ������
int sem_getval(int semid, int *val)
{
	int ret = 0;
	union semun su;
	ret = semctl(semid, 0, GETVAL, su);
	if (ret < 0)
	{
		perror("func err semctl");
		return ret;
	}
	*val = su.val;
	return ret;
}

// �źŵ� P �������� �����ź�����val --
int sem_p(int semid)
{
	int ret = 0;
	struct sembuf sops = { 0, -1, 0 };
	ret = semop(semid, &sops, 1);
	return ret;
}

// �źŵ� V ��������, �����ź�����val ++
int sem_v(int semid)
{
	int ret = 0;
	struct sembuf sops = { 0, 1, 0 };
	ret = semop(semid, &sops, 1);
	return ret;
}

int main(void)
{
	int ret = 0;
	int semid = 0;
	int val = 0;
	
	key_t key = ftok(".", 'a');
	sem_creat(key);
	//semid = sem_open(0x11111);
	sem_setval(semid, 1);	// �ź�������Դ���� 
	sem_getval(semid, &val);
	printf("val = %d\n", val);
	
	pid_t pid = fork();

	sem_p(semid);
	printf("pid = %d, val = %d\n", pid, val);
	sleep(3);
	sem_v(semid);

	return 0;
}
