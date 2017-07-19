#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

void my_sigaction (int num, siginfo_t *st, void *dat)
{
    int ret = 0;
    if (SIGQUIT == num)
    {
        printf("func myhandler num = %d\n", num);
        sigset_t set;
        sigemptyset(&set);
        ret = sigaddset(&set, SIGINT);
        assert(-1 != ret);
        ret = sigprocmask(SIG_UNBLOCK, &set, NULL);
        assert(-1 != ret);
    }
	else
	{
		printf("signal is  no SIGQUIT\n");
	}
}

int main(void)
{
    int ret = 0;
    
    /* 阻塞信号  */
    sigset_t set;
    sigemptyset(&set);
    ret = sigaddset(&set, SIGINT);
    assert(-1 != ret);
    ret = sigprocmask(SIG_BLOCK, &set, NULL);
	assert(-1 != ret);
    
    /* 注册信号 */
    struct sigaction act;
    act.sa_sigaction = my_sigaction;
    ret = sigemptyset(&(act.sa_mask));
    assert(-1 != ret);
    ret = sigaction(SIGQUIT, &act, NULL);
    assert(-1 != ret);
	
    
	/* 发送信号 */
	int flag = 0;
	while (1)
	{
		printf("input flag:");
		scanf("%d", &flag);
		if (0 == flag)
		{
			union sigval value;
			value.sival_int = 10;
			ret = sigqueue(getpid(), SIGQUIT, value);
			assert(-1 != ret);
			break;
		}
	}
	
    while (1)
    {
        pause();
    }
	return 0;
}
