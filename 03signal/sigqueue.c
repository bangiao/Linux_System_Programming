#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

// 这个函数 中 signum 就是消息编号, act 就是附带数据
// void *data POSIX 没有定义
void my_sigaction(int signum, siginfo_t *act, void *data)
{
	int myint = 0;
	printf("recv signum : %d\n", signum);
	
	//siginfo_t {
    //           int      si_signo;    /* Signal number */
    //           int      si_errno;    /* An errno value */
    //           int      si_code;     /* Signal code */
    //           int      si_trapno;   /* Trap number that caused
    //                                    hardware-generated signal
    //                                    (unused on most architectures) */
    //           pid_t    si_pid;      /* Sending process ID */
    //           uid_t    si_uid;      /* Real user ID of sending process */
    //           int      si_status;   /* Exit value or signal */
    //           clock_t  si_utime;    /* User time consumed */
    //           clock_t  si_stime;    /* System time consumed */
    //			这个参数是旧的标准遗留下来的用处和 si_int 一样
    //           sigval_t si_value;    /* Signal value */
    // 			这个类似于 si_int 整数
    //           int      si_int;      /* POSIX.1b signal */
    //			这个就是	数组
    //           void    *si_ptr;      /* POSIX.1b signal */
    //           int      si_overrun;  /* Timer overrun count; POSIX.1b timers */
    //           int      si_timerid;  /* Timer ID; POSIX.1b timers */
    //           void    *si_addr;     /* Memory location which caused fault */
    //           long     si_band;     /* Band event (was int in
    //                                    glibc 2.3.2 and earlier) */
    //           int      si_fd;       /* File descriptor */
    //           short    si_addr_lsb; /* Least significant bit of address
    //                                    (since Linux 2.6.32) */
    //       }
    
	myint = act->si_int;
	printf("myint = %d\n", myint);
}

int main(void)
{
	int ret = 0;
	pid_t pid;
	
	/*
	union sigval {
               int   sival_int;
               void *sival_ptr;
           };
	*/
	union sigval value;
	
	value.sival_int = 3;
	
	/*
	struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t   sa_mask;
               int        sa_flags;
               void     (*sa_restorer)(void);
           };
           */
    
    // 注册信号
	struct sigaction act;
	act.sa_sigaction = my_sigaction;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	ret = sigaction(SIGINT, &act, NULL);
	if (-1 == ret)
	{
		perror("func err sigaction()\n");
		return -1;
	}
	
	pid = fork();
	if (-1 == pid)
	{
		perror("func err fork()\n");
		return -1;
	}
	else if (0 == pid)
	{
		// 发送消息
		// sigqueue 将会在内核中查找到 pid 对应的 PCB 进程控制块
		// 并发送 SIGINT 消息到这个 pid 对应的进程(PCB)中, 并附带数据 value
		ret = sigqueue(getppid(), SIGINT, value);
		if (-1 == ret)
		{
			perror("func err sigqueue()\n");
			return -2;
		} 
	}
	else if (pid > 0)
	{
		while (1)
		{
			pause();
		}
	}
	
	return 0;
}