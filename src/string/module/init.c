#include "../internal.h"

void pool_init(StringPool* pool, uint64_t size)
{
    pool->memory = malloc(size);
    pool->used = 0;
    pool->max_size = size;

    memset(pool->memory, SENTINEL, size);
}

void strings_init(uint64_t poolSize, uint64_t expandBufferSize)
{
    pool_init(&String_Mem.pool, poolSize);
    pool_init(&String_Mem.trans_pool, expandBufferSize);
}
