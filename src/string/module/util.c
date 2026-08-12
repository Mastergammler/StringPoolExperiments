#include "../internal.h"

int get_and_reserve_arg_buf()
{
    for (int i = 0; i < FMT_ARG_BUF_COUNT; i++)
    {
        if (FmtArgBuffers.in_use[i] == false)
        {
            FmtArgBuffers.in_use[i] = true;
            return i;
        }
    }

    // no buffer available, running out of buffer space
    // -> to many recursive usages
    ASSERT(false, "No buffer left available, too many nested calls!");
    return -1;
}

void release_arg_buf(int index)
{
    ASSERT(index < FMT_ARG_BUF_COUNT, "Index out of range: %i/%i", index,
           FMT_ARG_BUF_COUNT);
    FmtArgBuffers.in_use[index] = false;
}

int clamp_min(int value, int floorInc)
{
    return value < floorInc ? floorInc : value;
}

int clamp_max(int value, int ceilingEx)
{
    return value >= ceilingEx ? ceilingEx : value;
}

int clamp(int value, int min, int max)
{
    int minned = clamp_min(value, min);
    return clamp_max(minned, max);
}

uint64_t memory_size(StrPoolOptions opt, uint64_t charCount)
{
    StringPool* pool = POOL(opt.pool_idx);

    int nullTermSpace = 0;
    int lfSpace = 0;

    if (!opt.substring_mode)
    {
        nullTermSpace = pool->neglect_null_termination ? 0 : 1;
        lfSpace = pool->add_linefeed ? 1 : 0;
    }

    uint64_t memorySize = charCount + nullTermSpace + lfSpace;
    ASSERT(memorySize >= charCount,
           "Overflow detected! Char count %ld, memory size %ld", charCount,
           memorySize);

    return memorySize;
}

/*
 * We assume that enough buffer has been allocated,
 * with the method memory_size
 */
int finalize_str(StrPoolOptions opt, char* strStart, uint64_t charCount)
{
    StringPool* pool = POOL(opt.pool_idx);
    int addStrLen = 0;

    if (pool->add_linefeed && !opt.substring_mode)
    {
        addStrLen++;
        // TODO: we donn't care about CR for now
        strStart[charCount++] = '\n';
    }

    // null terminator doesn't count for string length
    if (!pool->neglect_null_termination && !opt.substring_mode)
    {
        strStart[charCount] = 0;
    }

    return addStrLen;
}
