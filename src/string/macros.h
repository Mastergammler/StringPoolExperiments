#include "internal.h"

// NOTE: were creating compound literals as lvalues here
// -> so these remain valid for the function call duration

#define STR_HEADER(fn) fn, NULL
#define PRINT_HEADER(fn) NULL, fn

#define FLOAT(f, dec)                                                          \
    &(FloatFormat)                                                             \
    {                                                                          \
        STR_HEADER(expand_float), f, dec                                       \
    }

#define PTR(val)                                                               \
    &(PtrFormat)                                                               \
    {                                                                          \
        STR_HEADER(expand_ptr), val                                            \
    }

#define BOO(val)                                                               \
    &(BoolFormat)                                                              \
    {                                                                          \
        PRINT_HEADER(bool_formatter), val                                      \
    }

#define NUM(val, ...)                                                          \
    &(NumFormat)                                                               \
    {                                                                          \
        PRINT_HEADER(num_formatter), val, 0, false, ##__VA_ARGS__              \
    }

#define NUM_PAD(val, pad)                                                      \
    &(IntFormat)                                                               \
    {                                                                          \
        STR_HEADER(expand_int), val, pad, false                                \
    }

#define INT_BIN(val)                                                           \
    &(IntFormat)                                                               \
    {                                                                          \
        STR_HEADER(expand_int), val, 0, true                                   \
    }

#define STR_OBJ(val)                                                           \
    &(StrFormat)                                                               \
    {                                                                          \
        STR_HEADER(expand_str), val                                            \
    }

#define STR(val)                                                               \
    &(StrFormat)                                                               \
    {                                                                          \
        STR_HEADER(use_str), val                                               \
    }
#define CH(val)                                                                \
    &(CharFormat)                                                              \
    {                                                                          \
        STR_HEADER(use_char), val                                              \
    }

#define LIT(val)                                                               \
    &(StrFormat)                                                               \
    {                                                                          \
        STR_HEADER(use_str), str_static(val)                                   \
    }
