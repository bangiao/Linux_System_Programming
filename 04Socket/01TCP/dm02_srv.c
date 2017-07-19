#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/*
 int socket(int domain, int type, int protocol);
 */

/*
 int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);

 struct sockaddr {
 sa_family_t sa_family;
 char        sa_data[14];
 }


 struct sockaddr_in {
 sa_family_t    sin_family; // address family: AF_INET
 u_int16_t      sin_port;   // port in network byte order
 struct in_addr sin_addr;   // internet address
 };

 //Internet address.
 struct in_addr {
 u_int32_t      s_addr;     //address in network byte order
 };

 */
int main()
{
	//定义socket描述符
	int sockfd = 0;
	//创建socket PE_INET协议族 IPV4   SOCK_STREAM 使用TCP协议  第三个参数一般为0 ，选择默认的子协议
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		//如果创建失败
		perror("fun socket\n");
		exit(0);
	}
	//定义socket结构体
	struct sockaddr_in srvaddr;
	//设置地址协议
	srvaddr.sin_family = AF_INET;
	//设置接口
	srvaddr.sin_port = htons(8001);
	//设置ip
	//inet_addr()的功能是将一个点分十进制的IP转换成一个长整数型数（u_long类型）
	srvaddr.sin_addr.s_addr = inet_addr("192.168.6.249"); //127.0.0.1
	if (bind(sockfd, (struct sockaddr *) &srvaddr, sizeof(srvaddr)) < 0)
	{
		perror("fun socket\n");
		exit(0);
	}
	return 0;
}

