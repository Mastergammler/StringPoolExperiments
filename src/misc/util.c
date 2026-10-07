int clamp_lower(int value, int floorInc)
{
    return value < floorInc ? floorInc : value;
}

int clamp_upper(int value, int ceilingEx)
{
    return value >= ceilingEx ? ceilingEx : value;
}

int clamp(int value, int min, int max)
{
    int minned = clamp_lower(value, min);
    return clamp_upper(minned, max);
}

int min_i(int a, int b)
{
    return a < b ? a : b;
}
int max_i(int a, int b)
{
    return a > b ? a : b;
}
