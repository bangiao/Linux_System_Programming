#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>


struct msgbuf
{
	long mtype;
	char mtext[1024];
};

int main(void)
{
	int ret = 0;
	struct msgbuf sendbuf;
	struct msgbuf recvbuf;

	//  ./ipc 这个文件必须要有字
	key_t msqkey = ftok("./ipc", 'z');

	int msqid = msgget(msqkey, 0666 | IPC_CREAT);
	if (msqid < 0)
	{
		perror("func err msgget: ");
		return msqid;
	}


	bzero(&sendbuf, sizeof(sendbuf));
	sendbuf.mtype = 1;
	memcpy(sendbuf.mtext, "zhazha", 6);

	ret = msgsnd(msqid, &sendbuf, 6, IPC_NOWAIT);
	if (0 != ret)
	{
		perror("func err msgsnd: ");
		return ret;
	}

	int recvlen = 0;
	bzero(&recvbuf, sizeof(recvbuf));
	do
	{
		recvlen = msgrcv(msqid, &recvbuf, 6, 0, IPC_NOWAIT);
	} while (errno == EAGAIN);
	if (recvlen < 0)
	{
		perror("func msgrcv err : ");
		return recvlen;
	}
	printf("recvbuf = %s\n", recvbuf.mtext);

	struct msqid_ds ds_buf;
	bzero(&ds_buf, sizeof(ds_buf));
	ret = msgctl(msqid, IPC_RMID, &ds_buf);
	if (ret < 0)
	{
		perror("func err msgctl: ");
		return ret;
	}

	return 0;
}
