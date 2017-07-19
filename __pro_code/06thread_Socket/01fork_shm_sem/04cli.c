#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "comsock.h"


int main()
{	
    int 				ret = 0;
    void 				*handle = NULL;
    int 				i = 0, j = 0;
    char				*data = "aaaaaafffffffffffssssss";
    unsigned int 		datalen = 10;
    int 				procnum = 10;
	int 				loopnum = 100;
	char 				out[1024] = {0};
    unsigned int 		outlen = 1024;
	
	signal(SIGPIPE, SIG_IGN);

	printf("请输入要创建子进程的个数 : \n");
	scanf("%d", &procnum);

	printf("请输入让每个子进程测试多少次 :\n");
	scanf("%d", &loopnum);
	
    ret = sck_client_init(&handle, 15, 5, 5, 10);
    //ret = sck_client_init(&handle, 1, 0, 0, 10);		// connect 是需要时间的

    for (i = 0; i < procnum; i++)
    {
    	int pid = fork();
    	if (pid < 0)
    	{
    		ret = errno;
    		perror("func err fork: ");
    		return ret;
    	}
    	if (0 == pid)
    	{
    		ret = sck_client_getconn(handle, "127.0.0.1", 8001);
    		for (j = 0; j < loopnum; j++)
    		{
    			//客户端发送报文
		        ret = sck_client_send(handle, data, datalen);
		        if (ret >= 3000)
		        {
		            if (ret == Sck_ErrTimeOut)
		            {
		            	continue;
		            }
		            break;
		        }
		        
		        //客户端端接受报文
		        ret = sck_client_recv(handle, out, &outlen);
		        if (ret >= 3000)
		        {
		            if (ret == Sck_ErrTimeOut)
		            {
		            	continue;
		            }
		            break;
		        }
		        out[outlen] = '\0';
		        printf("data: %s \n", out);
    		}
    		sck_client_close(handle);
    		exit(0); // 浪子进程退出, 不让子进程参与fork
    	}
    }
    
    pid_t mypid = 0;
    while ((mypid = waitpid(-1, NULL, 0)) > 0)
	{
		printf("destroy child process pid = %d\n", mypid);
	}

    // 客户端环境释放 
    ret = sck_client_destroy(handle);
    return 0;
}

