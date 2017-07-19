#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

int main(void)
{
    int ret = 0;
    ret = kill(-1, SIGKILL);
    if (-1 == ret)
    {
        perror("func kill err \n");
        return -1;
    }
    return 0;
}
