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
