#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

extern char **environ;

int main(void)
{
	int 	i = 0;
    printf("hello begin\n");
	printf("child hello: pid = %d\n", getpid());
	for (i = 0; environ[i] != NULL; i++)
	{
		printf("%s\n", environ[i]);
	}
    printf("hello end\n");
	return 0;
}
