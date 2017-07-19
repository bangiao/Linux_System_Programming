#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// ��ʾ vfork �� fork ������
// vfork �������̵����ݶ� ��ջ�� ����� ...�ȵ�
// ���ӽ��̵� pid ��ͬ
// vfork ��������� execle ��һ�ຯ�� ��һ�� ����ʹ�� exit _exit
// ������ʹ�� vfork  

int main(void)
{
    pid_t  pid;
    
    printf("before fork pid = %d\n", getpid());
    
    int num;
    
    signal(SIGCHLD, SIG_IGN);
    
    pid = vfork();
    if (pid == -1)
    {
        perror("fork err");
        return -1;
    }
    else if (pid == 0)
    {
        printf("child: %d\n", getpid());
    }
    else if (pid > 0)
    {
        printf("parent: %d\n", getpid());
        // exec �庯��
        //return 0;  err 
        //exit(0);  
        // �����漰�� ϵͳ������ c lib����������
        _exit(0);
    }
    
    printf("after fork\n");
	return 0;
}
