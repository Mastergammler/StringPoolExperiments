#include "../internal.h"

// copy string into own pool
// -> including null terminator
str string_alloc(StringPool* pool, const char* cstr, int len)
{

    char* strStart = pool_use(pool, memory_size(pool, len));
    memcpy(strStart, cstr, memory_size(pool, len));
    len += finalize_str(pool, strStart, len);

    return (str){.chars = strStart,
                 .len = len,
                 .null_terminated = !pool->neglect_null_termination,
                 .is_slice = false,
                 .is_static = false};
}

/*
 * Actually moves the used pointer forward
 */
void* pool_use(StringPool* pool, uint64_t size)
{
    // NOTE: if we don't fit in the end section, we just start over
    // To be able to easily memcpy or print contiguous memory
    // without having to write a extra function that supports
    // wrapping
    if (pool->ring_buffer && pool->cursor_idx + size >= pool->capacity)
    {
        pool->cursor_idx = 0;
    }

    ASSERT(pool->cursor_idx + size < pool->capacity,
           "Pool %i has not enough space left (%lu/%lu taken %lu requested)",
           pool->pool_idx, pool->cursor_idx, pool->capacity, size);
    void* useStart = pool->memory + pool->cursor_idx;
    pool->cursor_idx += size;
    return useStart;
}

void str_pool_reset(StrPoolOptions opt)
{
    POOL(opt.pool_idx)->cursor_idx = 0;
}

/*
 * Resetting to last cursor index
 */
void str_pool_reset_to(StrPoolOptions opt, uint64_t pos)
{
    StringPool* pool = POOL(opt.pool_idx);
    ASSERT(pos <= pool->cursor_idx,
           "Cursor must be reset forward (overflow?) actual %lu, requested %lu",
           pool->cursor_idx, pos);

    pool->cursor_idx = pos;
}

/*
 * Just checks if enough space is in principle available
 */
void* pool_check_next(StringPool* pool, int maxSize)
{
    ASSERT(pool->cursor_idx + maxSize < pool->capacity,
           "Pool %i has not enough space left (%lu/%lu taken %i reuqested)",
           pool->pool_idx, pool->cursor_idx, pool->capacity, maxSize);

    return pool->memory + pool->cursor_idx;
}
