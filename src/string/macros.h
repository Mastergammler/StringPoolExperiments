#include "internal.h"

// NOTE: were creating compound literals as lvalues here
// -> so these remain valid for the function call duration

#define fmt_n(val, ...)                                                        \
    &(NumFormat)                                                               \
    {                                                                          \
        (FmtHeader){write_num_strategy}, val, ##__VA_ARGS__                    \
    }

#define fmt_u(val, ...)                                                        \
    &(NumFormat)                                                               \
    {                                                                          \
        (FmtHeader){write_num_strategy}, val, pxU, ##__VA_ARGS__               \
    }

#define fmt_f(f, dec)                                                          \
    &(FloatFormat)                                                             \
    {                                                                          \
        (FmtHeader){write_float}, f, dec                                       \
    }

#define fmt_b(val)                                                             \
    &(BoolFormat)                                                              \
    {                                                                          \
        (FmtHeader){write_bool, val ? 4 : 5}, val                              \
    }

#define fmt_p(val)                                                             \
    &(PtrFormat)                                                               \
    {                                                                          \
        (FmtHeader){write_ptr, 18}, (void*)val                                 \
    }

#define STR_OBJ(val)                                                           \
    &(StrFormat)                                                               \
    {                                                                          \
        (FmtHeader){write_str_obj}, val                                        \
    }

#define fmt_npres(val)                                                         \
    &(NpResFormat)                                                             \
    {                                                                          \
        (FmtHeader){format_num_parse_res}, val                                 \
    }

#define fmt_s(val)                                                             \
    &(StrFormat)                                                               \
    {                                                                          \
        (FmtHeader){write_str, val.len}, val                                   \
    }

#define fmt_c(val)                                                             \
    &(CharFormat)                                                              \
    {                                                                          \
        (FmtHeader){write_char, 1}, val                                        \
    }

// PERF: i'm now measuring the len of the literal twice
//  dunno how to optimize this, without having a temprorary value save?
//  -> Would need a scope here
#define fmt_l(val)                                                             \
    &(StrFormat)                                                               \
    {                                                                          \
        (FmtHeader){write_str, strlen(val)}, str_static(val)                   \
    }
