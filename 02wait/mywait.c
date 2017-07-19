#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* wait 的功能就是获得子进程退出的消息, 得知子进程是如何退出的 */

int main(void)
{
	pid_t pid = 0;
	int ret = 0;
	int status = 0;	
	
	pid = fork();
	if (-1 == pid)
	{
		printf("fork() 失败\n");
		return -1;
	}
	else if (0 == pid)		/* 子进程 */
	{
		printf("child pid = %d\n", getpid());
		//exit(100);
		abort();
	}	
	/* 父进程 */
	printf("parent pid = %d\n", getpid());
	/* wait 返回子进程的 id wait就是为了查看子进程的退出状态 */
	ret = wait(&status);
	printf("ret = %d, pid = %d\n", ret, pid);
	
	if (WIFEXITED(status))		// w if exit ed  wait if exited
	{
		printf("看process exit id : %d exit\n", WEXITSTATUS(status));
	}		
	/*
	WIFSIGNALED(status)
              returns true if the child process was terminated by a signal.
	*/
	else if (WIFSIGNALED(status))
	{
		printf ("进程 abort process id is = %d\n", WTERMSIG(status));
	}
	/*
	WIFSTOPPED(status)
              returns  true  if  the  child process was stopped by delivery of a signal;
              this is possible only if the call was done using  WUNTRACED  or  when  the
              child is being traced (see ptrace(2)).

       WSTOPSIG(status)
              returns  the  number  of  the signal which caused the child to stop.  This
              macro should be employed only if WIFSTOPPED returned true.
	*/
	else if (WIFSTOPPED(status))
	{
		printf ("进程停止 process id is = %d\n", WSTOPSIG(status));
	}
	else
	{
		printf("other\n");
	}
	return 0;
}