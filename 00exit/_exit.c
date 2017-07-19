#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// _exit 系统函数
// 不管缓冲区直接进入内核, 没有给你机会让你输出到缓冲区的东西到屏幕

// exit c库函数
// 有管理下缓冲区

// \n 的作用具有刷屏的作用

int main11(void)
{
	// 证明了 \n 的作用 
	printf("hello world");
	//exit(0);
	fflush(stdout);// 清除缓冲区
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
	
	// 当注册了函数在  atexit中 的话  当程序结束(不论是不是  return还是exit 结束)时会先去执行 
	// atexit 内部的函数指针的函数 再执行终止进程
	atexit(bye1);
	atexit(bye2);
	
	printf("main end\n");
	
	// c lib 函数
	//exit(0);
	
	//系统函数
	//_exit(0);
	
	return 0;
}