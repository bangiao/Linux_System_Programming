#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void myHandler(int num)
{
	int ret = 0;
	
	if (SIGUSR1 == num)
	{
		sigset_t set;
		ret = sigemptyset(&set);
		assert(!(-1 == ret));
		ret = sigaddset(&set, SIGINT);
		assert(!(-1 == ret));
		ret = sigaddset(&set, SIGRTMIN);
		assert(!(-1 == ret));
		ret = sigprocmask(SIG_UNBLOCK, &set, NULL);
		assert(!(-1 == ret));
		printf("解除阻塞 recv sig num: %d\n", num);
	}
	else if (num == SIGINT || num == SIGRTMIN)
	{
		printf("recv sig num: %d\n", num);
	}
	else
	{
		printf(" 其他信号recv sig num: %d\n", num);
	}
}

int main(void)
{
	pid_t pid;
	int ret = 0;
	// 设置回调函数
	struct sigaction act;
	act.sa_handler = myHandler;
	act.sa_flags = SA_SIGINFO;	// 千万别忘记了
	// 注册非实时信号的处理函数
	ret = sigaction(SIGINT, &act, NULL);
	assert(!(-1 == ret));
	// 注册实时信号的处理函数
	ret = sigaction(SIGRTMIN, &act, NULL);
	assert(!(-1 == ret));
	// 注册用户自定义信号
	ret = sigaction(SIGUSR1, &act, NULL);
	assert(!(-1 == ret));
	
	// 把 SIGINT  SIGRTMIN 军添加到阻塞状态字中
	sigset_t set;
	ret = sigemptyset(&set);
	assert(!(-1 == ret));
	ret = sigaddset(&set, SIGINT);
	assert(!(-1 == ret));
	ret = sigaddset(&set, SIGRTMIN);
	assert(!(-1 == ret));
	ret = sigprocmask(SIG_BLOCK, &set, NULL);
	assert(!(-1 == ret));
	
	pid = fork();
	assert(!(-1 == ret));
	if (0 == pid)
	{
		union sigval value;
		value.sival_int = 10;
		int i = 0;
		// 发三次不稳定信号
		// for (i = 0; i < 3; i++)
		// {
			// ret = sigqueue(getppid(), SIGINT, value);
			// assert(!(-1 == ret));
			// printf("发送不可靠信号 ok\n");
		// }
		
		// 发三次稳定信号
		value.sival_int = 20;
		// Linux 内核 实时信号压力测试
		for (i = 0; i < 30*1024; i++)
		{
			ret = sigqueue(getppid(), SIGRTMIN, value);
			assert(!(-1 == ret));
			printf("发送可靠信号ok i = %d\n", i);
		}
		// 向父进程发送 SIGUSR1 解除阻塞
		ret = kill(getppid(), SIGUSR1);
		assert(!(-1 == ret));
	}
	while (1)
	{
		sleep(1);
	}
	return 0;
}

/*
 #include <signal.h>

       int sigqueue(pid_t pid, int sig, const union sigval value);
The value argument is used to specify an accompanying item  of  data  (either  an
       integer  or  a  pointer  value) to be sent with the signal, and has the following
       type:

           union sigval {
               int   sival_int;
               void *sival_ptr;
           };
*/


/*
#include <signal.h>

       int sigemptyset(sigset_t *set);

       int sigfillset(sigset_t *set);

       int sigaddset(sigset_t *set, int signum);

       int sigdelset(sigset_t *set, int signum);

       int sigismember(const sigset_t *set, int signum);

*/





















