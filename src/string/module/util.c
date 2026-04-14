#include "../internal.h"

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
