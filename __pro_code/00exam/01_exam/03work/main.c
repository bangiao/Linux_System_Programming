#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
	父进程创建子进程1和子进程2、子进程1向子进程2发送可靠信号，并传送额外数据为子进程1的pid*2;
	子进程2接受可靠信号的值，并发送给父进程，父进程把接受的值进行打印。
 */

void My_Sigaction(int signum, siginfo_t *st, void *dat)
{
	int ret = 0;
	// 这个是父进程的接收数据信号
	if (SIGRTMIN+1 == signum)
	{
		printf("parent rev signal\n");
		printf("parent rev pid2 signal data = %d\n", st->si_value.sival_int);
	}
	else if (SIGRTMIN == signum)	//并将数据发送给父进程
	{
		printf("child recv signal\n");
		union sigval value;
		printf("My_Sigaction : st->si_value.sival_int = %d\n", st->si_value.sival_int);
		value.sival_int = st->si_value.sival_int;
		ret = sigqueue(getppid(), SIGRTMIN+1, value);
		if (-1 == ret)
		{
			perror("pid2: SIGRTMIN func err sigqueue: ");
			return ;
		}
	}
}

int main(int argc, char** argv)
{
	int ret = 0;
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = My_Sigaction;
	act.sa_flags = SA_SIGINFO;
	ret = sigaction(SIGRTMIN+1, &act, NULL);
	if (-1 == ret)
	{
		perror("func err sigaction: ");
		return -1;
	}

	pid_t pid1, pid2;

	///////////////////////////////////////////////////////////////////////////////
	//									pid1
	///////////////////////////////////////////////////////////////////////////////
	
	pid1 = fork();
	if (-1 == pid1)
	{
		perror("func err fork() pid 1: ");
		return -1;
	}
	else if (0 == pid1) // 子进程 1
	{
		// 1 子进程1向子进程2发送可靠信号，并传送额外数据为子进程1的pid*2;
		// SIGRTMIN+1
		// pid.txt
			// 读取 子进程2 的 pid
			// 用文件读取
		FILE *fp = NULL;
		while (1)
		{
			fp = fopen("pid.txt", "a+");
			if (NULL == fp)
			{
				continue;
			}
			else
			{
				break;
			}
		}
		printf("pid1: 打开文件成功\n");
		// 这里需要字符串转数字
		char revbuf[32] = {0};
		
		while (1)
		{
			char *tmp = fgets(revbuf, sizeof(revbuf), fp);
			if (NULL == tmp)
			{
				printf("pid1: 读取 pid2 中 ...\n");
				continue;
			}
			else 
			{
				printf("pid1: 读取 pid2 成功pid2 = %s\n", revbuf);
				break;
			}
		}
		if (NULL == fp)
		{
			fclose(fp);
			fp = NULL;
		}
		ret = system("rm -rf pid.txt");
		if (-1 == ret)
		{
			printf("pid1: func err system\n");
			return -1;
		}
		
		pid2 = atoi(revbuf);
		printf("pid1: 转化 pid2 为整数成功 = %d\n", (int )pid2 );
		union sigval value;
		value.sival_int = ((int)pid2) * 2;
		printf("pid1: value.sival_int  = %d\n", value.sival_int);
		ret = sigqueue((pid_t)pid2, SIGRTMIN, value);
		if (-1 == ret)
		{
			perror("pid1: func err sigqueue:");
			return -7;
		}
		printf("子进程退出...\n");
		exit(0);
	}
	///////////////////////////////////////////////////////////////////////////////
	//									pid2
	///////////////////////////////////////////////////////////////////////////////
	pid2 = fork();
	if (-1 == pid2)
	{
		perror("func err fork pid2:");
		return -2;
	}
	else if (0 == pid2) // 子进程 2 
	{
		// 3 接收子进程的信号, 并将数据发送给父进程
		struct sigaction act;
		ret = sigemptyset(&act.sa_mask);
		if (-1 == ret)
		{
			printf("pid2: func err sigemptyset\n");
			return -5;
		}
		act.sa_sigaction = My_Sigaction;
		act.sa_flags = SA_SIGINFO;
		ret = sigaction(SIGRTMIN, &act, NULL);
		if (-1 == ret)
		{
			perror("pid2: func err sigaction:");
			return -6;
		}
		
		// 1 保存自己的pid, 给子进程使用
		// 这个用文件保存
		FILE *fp = NULL;
		char pid_buf[18] = {0};
		
		fp = fopen("pid.txt", "w+");
		if (NULL == fp)
		{
			printf("pid2: open file failed\n");
			return -3;
		}
		printf("pid2: 打开文件成功...pid2 = %d\n", (int)getpid());
		int pid = (int )getpid();
		sprintf(pid_buf, "%d", pid);
		printf("pid2: pid2 = %s\n", pid_buf);
		ret = fputs(pid_buf, fp);
		if (EOF == ret)
		{
			printf("pid2: write error\n");
			return -4;
		}
		if (NULL != fp)
		{
			fclose(fp);
			fp = NULL;
		}
		printf("pid2: child 2 wait for 5 sec ...\n");
		int n = 5;
		while (n)
		{
			n = sleep(n);
			printf("pid2: child 2 wait end\n");
		}
		printf("pid2: 子进程 2 退出\n");
		exit(0);
	}
	printf("parent wait for 10 sec\n");
	int i = 10;
	while (i)
	{
		i = sleep(i);
		printf("parent wait for 10 sec end \n");
	}
	wait(NULL);
	wait(NULL);
	return (EXIT_SUCCESS);
}

/*
 pid_t pidArray[10];

 void My_Sigaction(int signum, siginfo_t *st, void *dat)
 {
	 if (SIGRTMIN+1 == signum)
	 {
		 // 孩子1 收到了信号
		 
		 printf("孩子 1 收到信号, 他的值是 : %d\n", st->si_value.sival_int);
		 pidArray[1] = st->si_value.sival_int;
	 }
	 else if (SIGRTMIN+2 == signum)
	 {
		 // 进程2 收到信号 
		 
		 printf("孩子 2 收到信号, 他的值是 : %d\n", st->si_value.sival_int);
		 pidArray[3] = st->si_value.sival_int;
	 }
	 else if (SIGRTMIN+3 == signum)
	 {
		 // 父进程收到信号
		 printf("父进程收到信号, 他的大儿子的pid = %d, 他的二儿子的pid = %d\n", pidArray[0], pidArray[1]);
		 printf("二儿子发送的礼物是 = %d\n", st->si_value.sival_int);
	 }
	 else
	 {
		 printf("other signal recv\n");
	 }
 }


int main(void)
{
	int ret = 0;
	
	// 信号发送区
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = My_Sigaction;
	ret = sigaction(SIGRTMIN+1, &act, NULL);
	if (-1 == ret)
	{
		perror("func err sigaction: ");
		return -1;
	}
	
	ret = sigaction(SIGRTMIN+2, &act, NULL);
	if (-1 == ret)
	{
		perror("func err sigaction: ");
		return -2;
	}
	
	ret = sigaction(SIGRTMIN+3, &act, NULL);
	if (-1 == ret)
	{
		perror("func err sigaction: ");
		return -3;
	}
	
	int i = 0;
	pid_t pid = 0;
	for (i = 0; i < 2; i++)
	{
		pidArray[i] = pid = fork();
		if (-1 == pid)
		{
			perror("func err fork:");
			return -4;
		}
		else if (0 == pid)
		{
			break;
		}
	}
	
	// 父进程
	if (pid > 0)
	{
		printf("父进程启动\n");
		// 父进程向子进程 1 发送附加数据(子进程2 的 pid)
		union sigval value;
		value.sival_int = pidArray[1];
		ret = sigqueue(pidArray[0], SIGRTMIN+1, value);
	}
	// 子进程 1 
	if (0 == pid && 0 == i)
	{
		printf("子进程 1 启动\n");
		printf("子进程1 睡眠\n");
		sleep(5);
		printf("子进程1 睡眠醒来\n");
		union sigval value;
		pid_t pid;
		pid = getpid();
		value.sival_int = pid * 2;
		ret = sigqueue(pidArray[1], SIGRTMIN+2, value);
		exit(0);
	}
	// 子进程 2 
	if (0 == pid && 1 == i)
	{
		printf("子进程 2 启动\n");
		printf("子进程 2 睡眠\n");
		sleep(10);
		printf("子进程 2 睡眠醒来\n");
		// 子进程2 发送信号给父进程
		union sigval value;
		value.sival_int = pidArray[3];
		printf("子进程2 给父进程发送信号的数据 %d\n", pidArray[3]);
		ret = sigqueue(getppid(), SIGRTMIN+3, value);
		exit(0);
	}
	sleep(2);
	pid_t mypid = 0;
	while (1)
	{
		while ((mypid = waitpid(-1, NULL, WNOHANG)) > 0)
		{
			printf("pid = %d\n", mypid);
		}
	}
	
	return 0;
}
*/
