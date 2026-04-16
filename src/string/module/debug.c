#include "../internal.h"

void debug_print_pool(StringPool pool, int stride)
{
    if (pool.ring_buffer)
    {
        printf(">>--------------------------------------->>\n");
    }
    else
    {
        printf("-------------------------------------------\n");
    }

    for (int i = 0; i < pool.capacity; i++)
    {
        unsigned char c = pool.memory[i];

        if (c == 0) c = '|';
        if (c == '\n') c = '\\';
        if (c == SENTINEL) c = '`';

        if ((i + 1) % stride == 0)
        {

            printf("%c\n", c);
        }
        else
        {
            printf("%c", c);
        }
    }

    if (pool.ring_buffer)
    {
        printf(">>--------------------------------------->>\n");
    }
    else
    {
        printf("-------------------------------------------\n");
    }
}
