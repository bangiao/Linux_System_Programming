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

/*

The msgp argument is a pointer to caller-defined structure of  the  fol©\
lowing general form:
struct msgbuf 
{
	long mtype;       // message type, must be > 0
	char mtext[1];    // message data
};

*/
//ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);


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

	if (argc > 2)
	{
		str = argv[1];
	}

	struct msgbuf recvbuf;
	memset(&recvbuf, 0, sizeof(recvbuf));
	ssize_t recvlen = 0;
	do
	{
		recvlen = msgrcv(msqid, &recvbuf, 10, 0, IPC_NOWAIT);
	} while (errno == ENOMSG || errno == EINTR);
	if (recvlen < 0)
	{
		perror("func msgrcv err: ");
		return -5;
	}
	printf("msg_buf.mtext = %s\n", recvbuf.mtext);

	return 0;
}

