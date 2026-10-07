#ifndef MGLIBC_UTIL
#define MGLIBC_UTIL

#ifdef __cplusplus
extern "C"
{
#endif

    int min_i(int a, int b);
    int max_i(int a, int b);
    int clamp(int value, int min, int max);
    int clamp_upper(int value, int ceilingEx);
    int clamp_lower(int value, int floorInc);

#ifdef __cplusplus
}
#endif

#endif // MGLIBC_UTIL
