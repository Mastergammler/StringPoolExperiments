#include "../internal.h"

void debug_print_pool(StringPool pool, int stride)
{
    printf("-------------------------------------------\n");
    for (int i = 0; i < pool.max_size; i++)
    {
        unsigned char c = pool.memory[i];

        if (c == 0) c = '|';

        if ((i + 1) % stride == 0)
        {

            printf("%c\n", c);
        }
        else
        {
            printf("%c", c);
        }
    }
    printf("-------------------------------------------\n");
}
