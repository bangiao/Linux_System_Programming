#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// _exit ϵͳ����
// ���ܻ�����ֱ�ӽ����ں�, û�и����������������������Ķ�������Ļ

// exit c�⺯��
// �й����»�����

// \n �����þ���ˢ��������

int main11(void)
{
	// ֤���� \n ������ 
	printf("hello world");
	//exit(0);
	fflush(stdout);// ���������
	_exit(0);
	//return 0;
}

void bye1(void)
{
	printf("that was all, bye1\n");
}

void bye2(void)
{
	printf("that was all, bye2\n");
}

int main(void)
{
	printf("main begin\n");
	
	// ��ע���˺�����  atexit�� �Ļ�  ���������(�����ǲ���  return����exit ����)ʱ����ȥִ�� 
	// atexit �ڲ��ĺ���ָ��ĺ��� ��ִ����ֹ����
	atexit(bye1);
	atexit(bye2);
	
	printf("main end\n");
	
	// c lib ����
	//exit(0);
	
	//ϵͳ����
	//_exit(0);
	
	return 0;
}