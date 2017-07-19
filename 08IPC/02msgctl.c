#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	int ret = 0;
	int msgid = 0;
	msgid = msgget(0x1234, 0666 | IPC_CREAT);
	if (msgid == -1)
	{
		perror("func err msgget: ");
		return -1;
	}
	printf("msgid = %d\n", msgid);

	struct msqid_ds buf;

	ret = msgctl(msgid, IPC_STAT, &buf);

	printf("��ǰ���е�Ȩ��		access = %o\n", buf.msg_perm.mode);
	printf("��ǰ��Ϣ�������ж����ַ�		_msg_bypes = %o\n", (unsigned int)buf.__msg_cbytes);
	printf("��ǰ��Ϣ���еĸ���	num = %o\n", buf.msg_qnum);

	// ����
	buf.msg_perm.mode = 0644;
	ret = msgctl(msgid, IPC_SET, &buf);
	if (-1 == ret)
	{
		perror("msgget err");
		return -2;
	}

	// ɾ��  remove ID
	ret = msgctl(msgid, IPC_RMID, NULL);
	if (-1 == ret)
	{
		perror("msgget err");
		return -3;
	}
	else
	{
		printf("ɾ���ɹ�		del success \n");
	}

	printf("set signal queue ...\n");

	return 0;
}

