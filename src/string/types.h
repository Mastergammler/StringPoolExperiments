#ifndef STRING_TYPES
#define STRING_TYPES

#define FMT_ARG_BUF_MAX 16
#define FMT_ARG_BUF_COUNT 4

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    const char* chars;
    uint32_t len;

    /* if the null terminator is present in the storage location */
    bool null_terminated;
    /*
     * child of another string (substring)
     * so lifetime can not be guaranteed
     * */
    bool is_slice;

    /*
     * Points to a string in static memory / literals etc
     */
    bool is_static;
} str;

typedef struct
{
    char* memory;
    uint64_t capacity;
    /*
     * cursor = used size for non ring buffer
     */
    uint64_t cursor_idx;
    uint8_t pool_idx;
    bool ring_buffer;
    /* do not add null terminator at the end */
    bool neglect_null_termination;
    /* add newline at the end */
    bool add_linefeed;

} StringPool;

typedef struct
{
    StringPool* pools;
    uint8_t pool_count;

    /*
     * pool used by default for allocations etc
     * when no StrPoolOptions are given
     */
    uint8_t default_pool;

    /**
     * pool used as the print buffer
     * Usually a ring buffer without null termination
     */
    uint8_t print_pool;

    /**
     * Transient pool used for symbol expansion
     * Can usually be quite small, but needed for expanding things
     */
    uint8_t transient_pool;
} StringMemory;

typedef struct
{
} StrSplitOpt;
typedef struct
{
    str head;
    str last;
} StrSplitResult;

typedef struct FmtHeader FmtHeader;
typedef str (*Formatter)(StringPool* pool, FmtHeader*);

// STFO: this wouldn't allow me to create custom formatters
// that i can just put into the function ...
// -> unless i put in a function pointer to the expander function?
struct FmtHeader
{
    Formatter fmt_fn;
};

typedef struct
{
    bool case_insensitive;

} StrCompareOptions;

typedef struct
{
    FmtHeader header;
    float value;
    int decimals;
} FloatFormat;

typedef struct
{
    FmtHeader header;
    int value;
    int places;
    bool binary;

} IntFormat;

typedef struct
{
    FmtHeader header;
    void* value;
} PtrFormat;

typedef struct
{
    FmtHeader header;
    bool value;
} BoolFormat;

typedef struct
{
    FmtHeader header;
    str value;
} StrFormat;

typedef struct
{
    FmtHeader* header;
    int placholder_idx;
    str expanded;
} FmtArg;

typedef struct
{
    FmtArg buf[FMT_ARG_BUF_COUNT][FMT_ARG_BUF_MAX];
    bool in_use[FMT_ARG_BUF_COUNT];

} FmtArgBufferStore;

typedef struct
{
    uint8_t pool_idx;
    bool keep_transients;
} StrPoolOptions;

typedef struct
{
    bool is_default;
    bool is_print;
    bool is_transient;
    /* omits setting defaults for some pools (print pool) */
    bool no_defaults;

} StrPoolInitOptions;

#endif
