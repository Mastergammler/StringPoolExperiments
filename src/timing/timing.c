#include "module.h"
#include <bits/time.h>

void timer_start(Timer* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->start);
    timer->last = timer->start;
}

float timer_elapsed_ms(Timer* timer)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    long nanos = now.tv_nsec - timer->last.tv_nsec;
    long seconds = now.tv_sec - timer->last.tv_sec;
    timer->last = now;

    return seconds * 1000. + nanos / 1000000.f;
}

float timer_ms_since_start(Timer* timer)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    long nanos = now.tv_nsec - timer->start.tv_nsec;
    long seconds = now.tv_sec - timer->last.tv_sec;

    return seconds * 1000. + nanos / 1000000.f;
}
