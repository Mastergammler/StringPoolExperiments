#include "../internal.h"

// copy string into own pool
// -> including null terminator
str string_alloc(StringPool* pool, const char* cstr, int len)
{
    char* strStart = pool_use(pool, STR_SIZE(len));
    memcpy(strStart, cstr, STR_SIZE(len));
    strStart[len] = 0;

    return (str){.chars = strStart,
                 .len = len,
                 .null_terminated = true,
                 .is_slice = false,
                 .is_static = false};
}

/*
 * Actually moves the used pointer forward
 */
void* pool_use(StringPool* pool, int size)
{
    // NOTE: if we don't fit in the end section, we just start over
    // To be able to easily memcpy or print contiguous memory
    // without having to write a extra function that supports
    // wrapping
    if (pool->ring_buffer && pool->cursor_idx + size >= pool->capacity)
    {
        pool->cursor_idx = 0;
    }

    assert(pool->cursor_idx + size < pool->capacity);
    void* useStart = pool->memory + pool->cursor_idx;
    pool->cursor_idx += size;
    return useStart;
}

void pool_reset(StringPool* pool)
{
    pool->cursor_idx = 0;
}

/*
 * Just checks if enough space is in principle available
 */
void* pool_check_next(StringPool* pool, int maxSize)
{
    assert(pool->cursor_idx + maxSize < pool->capacity);
    return pool->memory + pool->cursor_idx;
}
