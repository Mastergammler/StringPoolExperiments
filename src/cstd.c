#include <stdio.h>
int main()
{
#ifdef __STDC_VERSION__
    printf("%ld\n", __STDC_VERSION__);
#else
    printf("C89/C90\n");
#endif
    return 0;
}
