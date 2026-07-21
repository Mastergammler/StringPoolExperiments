#include "../internal.h"

/*
 * This is a 3 phase init structure
 * Phase 1: Allocating the header structure and setting the internal pointer
 * Phase 2: Defining the pools, sizes and their access indices
 * Phase 3: Actually allocating the memory for the pools
 */
void str_pool_prepare(AllocContext ctx, StringMemory* memory, uint8_t poolCount)
{
    StrMem = memory;
    StrMem->pool_count = poolCount;
    StrMem->pools =
        ctx.alloc_fn(ctx.alloc, sizeof(StringPool) * StrMem->pool_count);

    ASSERT(StrMem->pools, "Allocation of %i pools failed!", StrMem->pool_count);
}

void str_pool_define(uint8_t index, uint64_t size, StrPoolInitOptions opt)
{
    ASSERT(index < StrMem->pool_count, "Pool index out of bounds: %i/%i", index,
           StrMem->pool_count);

    if (opt.is_default)
    {
        ASSERT(StrMem->default_pool == 0,
               "Default pool was already defined for index %i",
               StrMem->default_pool);

        StrMem->default_pool = index;
    }

    if (opt.is_print)
    {
        ASSERT(StrMem->print_pool == 0,
               "Print pool was already defined for index %i",
               StrMem->print_pool);
        StrMem->print_pool = index;
    }

    if (opt.is_transient)
    {
        ASSERT(StrMem->transient_pool == 0,
               "Transient pool was already defined for index %i",
               StrMem->transient_pool);
        StrMem->transient_pool = index;
    }

    StrMem->pools[index] = (StringPool){};
    StringPool* pool = &StrMem->pools[index];
    pool->capacity = size;
    pool->pool_idx = index;
    if (opt.is_print && !opt.no_defaults)
    {
        pool->ring_buffer = true;
        pool->neglect_null_termination = true;
        pool->add_linefeed = true;
    }
}

/**
 * Does the actual allocations for the defined pools
 * Usually 3 pools are expected, a default, a print and a transient pool.
 * The transient pool must be there, in order for string expansion
 * to work, the others are technically optional.
 *
 * Pools can also be used by for multiple things, but it's not recommended,
 * since because that would be hard to differentiate in lifetime.
 */
void str_pool_finalize(AllocContext ctx)
{
    for (int i = 0; i < StrMem->pool_count; i++)
    {
        StringPool* pool = &StrMem->pools[i];
        pool->memory = ctx.alloc_fn(ctx.alloc, pool->capacity);
        ASSERT(pool->memory, "Allocation failed for pool at idx: %i, size %lu",
               i, pool->capacity);
    }
}
