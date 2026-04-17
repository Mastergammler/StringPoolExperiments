#ifndef STRING_TYPES
#define STRING_TYPES

#define FMT_ARG_BUF_MAX 16

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const char* chars;
    int len;

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
    bool ring_buffer;

} StringPool;

typedef struct
{
    StringPool persistent;
    StringPool print_buffer;
    StringPool transient;
} StringMemory;

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
    FmtHeader header;
    float value;
    int decimals;
} FloatFormat;

typedef struct
{
    FmtHeader header;
    int value;
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
    StringPool* pool;
    bool keep_transients;
    FmtArg arg_buffer[FMT_ARG_BUF_MAX];
} StorageOptions;

#endif
