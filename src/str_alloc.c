#include "internal.h"
#include "types.h"

void pool_init(StringPool* pool, int size)
{
    pool->memory = malloc(size);
    pool->used = 0;
    pool->max_size = size;

    memset(pool->memory, 0xf8, size);
}

// copy string into own pool
// -> including null terminator
str str_alloc(StringPool* pool, const char* cstr)
{
    int len = strlen(cstr);
    // printf("Allocating %p with len %i\n", cstr, len);
    assert(pool->used + len + 1 < pool->max_size);

    char* strStart = pool->memory + pool->used;
    pool->used += len + 1;
    memcpy(strStart, cstr, len + 1);

    return (str){.chars = strStart,
                 .len = len,
                 .null_terminated = true,
                 .is_slice = false};
}

/*
 * Actually moves the used pointer forward
 */
void* pool_use(StringPool* pool, int size)
{
    assert(pool->used + size < pool->max_size);
    void* useStart = pool->memory + pool->used;
    pool->used += size;

    return useStart;
}

/*
 * Just checks if enough space is in principle available
 */
void* pool_check_next(StringPool* pool, int maxSize)
{
    // printf("Checking pool for %i available bytes. %i/%i used\n", maxSize,
    // pool->used, pool->max_size);
    assert(pool->used + maxSize < pool->max_size);
    return pool->memory + pool->used;
}

str from_cstr(const char* cstr)
{
    return (str){.chars = cstr,
                 .len = strlen(cstr),
                 .null_terminated = true,
                 .is_slice = false};
}
