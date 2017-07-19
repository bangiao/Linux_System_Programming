#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
	in_addr_t myint = inet_addr("127.0.0.1");
	printf("inet_addr ip = %d\n", (int)myint);
	struct in_addr inp;
	
	
	inet_aton("127.0.0.1", &inp);
	printf("inet_aton ip = %d\n", (int )inp.s_addr);
	
	printf("%s\n", inet_ntoa(inp));
	
	return 0;
}

// struct in_addr {
               // uint32_t       s_addr;
           // };