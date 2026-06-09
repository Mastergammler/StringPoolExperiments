#ifndef MGLIBC_TIMING
#define MGLIBC_TIMING

#include <time.h>

typedef struct
{
    struct timespec start;
    struct timespec last;
} Timer;

void timer_start(Timer* timer);
float timer_elapsed_ms(Timer* timer);
float timer_ms_since_start(Timer* timer);

#endif
