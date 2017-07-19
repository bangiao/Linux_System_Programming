#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct msgbuf
{
	long mtype;       /* message type, must be > 0 */
	char mtext[1024];    /* message data */
};

int main(int argc, char *argv[])
{
	int ret = 0;
	int msqid = 0;
	char *str = "2033445917 --- 2133445917";

	msqid = msgget(0x1234, 0666 | IPC_CREAT);
	if (msqid == -1)
	{
		perror("func err msgget: ");
		return -1;
	}

	struct msgbuf sendbuf;
	memset(&sendbuf, 0, sizeof(sendbuf));
	sendbuf.mtype = 1;
	strcpy(sendbuf.mtext, str);

	ret = msgsnd(msqid, &sendbuf, 10, IPC_NOWAIT);
	if (ret < 0)
	{
		perror("func msgsnd err: ");
		return -2;
	}
	return 0;
}

