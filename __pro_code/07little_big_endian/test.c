#include <stdio.h>

union
{
    char c[4];
    unsigned long l;
}_type = {{'l', '?', '?', 'b'}};

// lttlie endian --> l
// big endian --> b
int main(void)
{
    printf("%c\n", (char)_type.l);
    return 0;
}
