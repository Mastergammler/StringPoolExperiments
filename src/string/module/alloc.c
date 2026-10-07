#include "../internal.h"

// copy string into own pool
// -> including null terminator
str string_alloc(StrPoolOptions opt, const char* cstr, int len)
{
    char* strStart = pool_use(opt, memory_size(opt, len));
    memcpy(strStart, cstr, memory_size(opt, len));
    len += finalize_str(opt, strStart, len);

    return (str){.chars = strStart,
                 .len = len,
                 .null_terminated =
                     !POOL(opt.pool_idx)->neglect_null_termination,
                 .is_slice = false,
                 .is_static = false};
}

// NOTE: if we don't fit in the end section, we just start over
// To be able to easily memcpy or print contiguous memory
// without having to write a extra function that supports
// wrapping
// This is especially relevant, that a printed string stays
// contiguous within memory, else all using functions would need
// to hanlde ring buffers themselves
void manage_ring_buffer(StrPool* pool, uint64_t maxSize)
{
    if (pool->cursor_idx + maxSize >= pool->capacity)
    {
        pool->cursor_idx = 0;
    }
}

/*
 * Actually moves the used pointer forward
 */
void* pool_use(StrPoolOptions opt, uint64_t size)
{
    StrPool* pool = POOL(opt.pool_idx);
    if (pool->ring_buffer) manage_ring_buffer(pool, size);

    ASSERT(pool->cursor_idx + size < pool->capacity,
           "Pool %i has not enough space left (%" PRIu64 "/%" PRIu64
           " taken %" PRIu64 " requested)",
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
    StrPool* pool = POOL(opt.pool_idx);
    ASSERT(pos <= pool->cursor_idx,
           "Cursor must be reset forward (overflow?) actual %" PRIu64
           ", requested %" PRIu64,
           pool->cursor_idx, pos);

    pool->cursor_idx = pos;
}

bool pool_fits_till_end(StrPoolOptions opt, uint64_t size)
{
    StrPool* pool = POOL(opt.pool_idx);
    return (pool->capacity - size) > pool->cursor_idx;
}
