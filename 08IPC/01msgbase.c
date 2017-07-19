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
	// ������д��ʾ���ڴ��ж�ȡ��Ϣ����, ������������ʾ����
	msgid = msgget(0x1234, 0666);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("��Ϣ���ж�ȡʧ��, ��Ϣ���в�����\n");
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
	// ������д��ʾ���ڴ��ж�ȡ���ߴ�����Ϣ����
	// �����Ϣ���в����ڵĻ��ʹ���һ����Ϣ����
	// ������ڵĻ��Ͷ�ȡ��Ϣ����
	msgid = msgget(0x1234, 0666 | IPC_CREAT);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("��Ϣ���ж�ȡʧ��, ��Ϣ���в�����\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("��Ϣ���д����ɹ�\n");

	return 0;
}

// IPC_CREAT and IPC_EXCL
// ��������ͬ�´��ڵ�ʱ�����, �������ڴ���ڵ�ʱ�� ���ļ��Ѿ�����
// IPC_EXCL ��������û����
int main03(void)
{
	int msgid = 0;
	msgid = msgget(0x1234, 0666 | IPC_CREAT | IPC_EXCL);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("��Ϣ���ж�ȡʧ��, ��Ϣ���в�����\n");
		}
		if (errno == EEXIST)
		{
			printf("��Ϣ�����Ѿ�����\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("msgid = %d\n", msgid);
	printf("��Ϣ���д����ɹ�\n");

	return 0;
}

// IPC_PRIVATE �����������Ϣ����ֻ�����Լ��Ľ��̼�������, 
// ���ҿ��Զ�ε���, ÿ�ε���Ϣ���е� id ����һ��
// IPC_PRIVATE �Ǹ� �� ����ֵΪ 0
int main04(void)
{
	int msgid = 0;
	msgid = msgget(IPC_PRIVATE, 0666);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("��Ϣ���ж�ȡʧ��, ��Ϣ���в�����\n");
		}
		if (errno == EEXIST)
		{
			printf("��Ϣ�����Ѿ�����\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("msgid = %d\n", msgid);
	printf("��Ϣ���д����ɹ�\n");

	return 0;
}

// Ȩ��λ��ʹ��
int main(void)
{
	int msgid = 0;
	msgid = msgget(0x1234, 0444 | IPC_CREAT);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("��Ϣ���ж�ȡʧ��, ��Ϣ���в�����\n");
		}
		if (errno == EEXIST)
		{
			printf("��Ϣ�����Ѿ�����\n");
		}
		if (errno == EACCES)
		{
			printf("EACCES err\n");
		}
		perror("func err msgget: ");
		return -1;
	}
	printf("msgid = %d\n", msgid);
	printf("��Ϣ���д����ɹ�\n");

	msgid = msgget(0x1234, 0644);
	if (msgid == -1)
	{
		if (errno == ENOENT)
		{
			printf("��Ϣ���ж�ȡʧ��, ��Ϣ���в�����\n");
		}
		if (errno == EEXIST)
		{
			printf("��Ϣ�����Ѿ�����\n");
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

