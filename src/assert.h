#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG

#define JUST_ASSERT(expr)                                                      \
    do                                                                         \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            fprintf(stderr,                                                    \
                    "Assertion failed: %s\n"                                   \
                    "  File: %s\n"                                             \
                    "  Line: %d\n"                                             \
                    "  Function: %s\n",                                        \
                    #expr, __FILE__, __LINE__, __func__);                      \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define ASSERT(expr, fmt, ...)                                                 \
    do                                                                         \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            fprintf(stderr,                                                    \
                    "Assertion failed: %s\n"                                   \
                    "  File: %s\n"                                             \
                    "  Line: %d\n"                                             \
                    "  Function: %s\n"                                         \
                    "  Message: " fmt "\n",                                    \
                    #expr, __FILE__, __LINE__, __func__, ##__VA_ARGS__);       \
            abort();                                                           \
        }                                                                      \
    } while (0)

#else

#define ASSERT(expr) ((void)0)
#define ASSERT_MSG(expr, ...) ((void)0)

#endif
