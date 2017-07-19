#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* alarm �źž���  cpu ���� n ��֮��ִ�з�����Ϣ SIGALRM */
// CPU �ں�  ���� alarm �ź�, �� �źž���ͨ���ص������ķ���ʵ�ֵ�

typedef struct teather
{
	int age ;
	int num;
}teather;

teather g_t; 

void printGlobTeacher()
{
	printf("g_t.age = %d\n", g_t.age);
	printf("g_t.num = %d\n", g_t.num);
}

void myhandler(int num)
{
	printf("func myhandler()\n");
	
	printGlobTeacher();
	
	// һ���ӷ���һ�� alarm �ź�
	alarm(1); // ��仰����˼����֪ͨ �ں� һ�����Ժ��ٴλص� alarm �ź�
}

int main(void)
{
	printf("main ... begin\n");
	
	teather t1, t2;
	t1.age = 30;
	t1.num = 30;
	t2.age = 40;
	t2.num = 40;
	
	
	if (SIG_ERR == signal(SIGALRM, myhandler))
	{
		perror("func signal err \n");
		return -1;
	}
	alarm (1);
	
	
	while (1)
	{
		// ������� �൱�� window ϵͳ�����  system("pause");
		// ���� pause �ǿ��жϵ���ͣ
		//pause();
		//printf("pause return \n");
		
		g_t = t1;
		g_t = t2;
		
	}
	return 0;
}