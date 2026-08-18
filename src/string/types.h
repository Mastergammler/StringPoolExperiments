#ifndef STRING_TYPES
#define STRING_TYPES

#define FMT_ARG_BUF_MAX 16
#define FMT_ARG_BUF_COUNT 4

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    /* decimal */
    pxD,
    /* unsigned */
    pxU,
    /* binary */
    pxB,
    /* hex */
    pxH,
    /* object */
    pxO
} PrintMode;

typedef enum
{
    FMT_ANY,
    FMT_STR,
    FMT_count
} HeaderType;

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
    int64_t val;
    bool dirty;
    bool was_empty;
    bool overflow;
} ParseNRes;

typedef struct
{
    uint64_t val;
    bool dirty;
    bool was_empty;
    bool overflow;
} ParseURes;

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

} StrPool;

typedef struct
{
    StrPool* pools;
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
    uint8_t pool_idx;

    /*
     * printing without defined endings, used for substrings
     * when a format 'concats' a string out of multiple substrings,
     * then the defined pool endings are skipped, since we're inside the string
     * this affects how the length of the string is calculated, so use with
     * caution
     */
    bool substring_mode;
} StrPoolOptions;

typedef struct
{
    bool is_default;
    bool is_print;
    /* omits setting defaults for some pools (print pool) */
    bool no_defaults;

} StrPoolInitOptions;

typedef struct
{
} StrSplitOpt;
typedef struct
{
    str head;
    str last;
} StrSplitResult;

typedef struct FmtHeader FmtHeader;
typedef str (*Formatter)(StrPoolOptions opt, FmtHeader*);

typedef struct PrintHeader PrintHeader;
typedef PrintHeader* (*PrintInfo)(FmtHeader*);
typedef str (*Printer)(char* writeCursor, PrintHeader*);

struct PrintHeader
{
    Printer print_fn;
    uint8_t str_len;
};

typedef struct
{
    PrintHeader header;

    /* hi-lo encoded decimal numbers, starting from big to small
     * (biggest decimal / leader at index 0)
     */
    uint8_t digits[10];
    uint8_t digit_count;

    uint8_t pad_left_count;
    uint8_t pad_right_count;
    char pad_char;
    bool negative;

} NumDesc;

struct FmtHeader
{
    Formatter fmt_fn;
    uint32_t est_len;
};

typedef struct
{
    FmtHeader header;
    bool value;
} BoolFormat;

typedef struct
{
    FmtHeader header;
    double value;
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
    int64_t value;
    PrintMode mode;
    int places;
} NumFormat;

typedef struct
{
    FmtHeader header;
    ParseNRes val;
} NpResFormat;

typedef struct
{
    FmtHeader header;
    void* value;
} PtrFormat;

typedef struct
{
    FmtHeader header;
    str value;
    int pad_left_count;
    char pad_char;
} StrFormat;

typedef struct
{
    FmtHeader header;
    char value;
} CharFormat;

typedef struct
{
    FmtHeader* header;
    int placholder_pos;
} FmtArg;

typedef struct
{
    FmtArg buf[FMT_ARG_BUF_COUNT][FMT_ARG_BUF_MAX];
    bool in_use[FMT_ARG_BUF_COUNT];

} FmtArgBufferStore;

typedef struct
{
    bool case_insensitive;

} StrCompareOptions;

#endif
