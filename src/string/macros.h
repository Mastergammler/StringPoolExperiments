#include "internal.h"

// NOTE: were creating compound literals as lvalues here
// -> so these remain valid for the function call duration

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

#define BOO(val)                                                               \
    &(BoolFormat)                                                              \
    {                                                                          \
        expand_bool, val                                                       \
    }

#define NUM(val)                                                               \
    &(IntFormat)                                                               \
    {                                                                          \
        expand_int, val, 0, false                                              \
    }

#define NUM_PAD(val, pad)                                                      \
    &(IntFormat)                                                               \
    {                                                                          \
        expand_int, val, pad, false                                            \
    }

#define INT_BIN(val)                                                           \
    &(IntFormat)                                                               \
    {                                                                          \
        expand_int, val, 0, true                                               \
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
