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
    assert(false);
    return -1;
}

void release_arg_buf(int index)
{
    assert(index < FMT_ARG_BUF_COUNT);
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

uint64_t memory_size(StringPool* pool, uint64_t charCount)
{
    int nullTermSpace = pool->neglect_null_termination ? 0 : 1;
    int lfSpace = pool->add_linefeed ? 1 : 0;

    uint64_t memorySize = charCount + nullTermSpace + lfSpace;
    assert(memorySize >= charCount);

    return memorySize;
}

/*
 * We assume that enough buffer has been allocated,
 * with the method memory_size
 */
int finalize_str(StringPool* pool, char* strStart, uint64_t charCount)
{
    int addStrLen = 0;

    if (pool->add_linefeed)
    {
        addStrLen++;
        // TODO: we donn't care about CR for now
        strStart[charCount++] = '\n';
    }

    // null terminator doesn't count for string length
    if (!pool->neglect_null_termination)
    {
        strStart[charCount] = 0;
    }

    return addStrLen;
}
