#include "internal.h"

#define FLOAT(f, dec)                                                          \
    &(FloatFormat)                                                             \
    {                                                                          \
        expand_float, f, dec                                                   \
    }

#define PTR(val)                                                               \
    &(PtrFormat)                                                               \
    {                                                                          \
        expand_ptr, val                                                        \
    }

#define BOOL(val)                                                              \
    &(BoolFormat)                                                              \
    {                                                                          \
        expand_bool, val                                                       \
    }

#define INT(val)                                                               \
    &(IntFormat)                                                               \
    {                                                                          \
        expand_int, val, false                                                 \
    }
#define INT_BIN(val)                                                           \
    &(IntFormat)                                                               \
    {                                                                          \
        expand_int, val, true                                                  \
    }

#define STR_OBJ(val)                                                           \
    &(StrFormat)                                                               \
    {                                                                          \
        expand_str, val                                                        \
    }

#define STR(val)                                                               \
    &(StrFormat)                                                               \
    {                                                                          \
        use_str, val                                                           \
    }
