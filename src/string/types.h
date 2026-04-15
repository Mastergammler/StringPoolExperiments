#ifndef STRING_TYPES
#define STRING_TYPES

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
    uint64_t max_size;
    uint64_t used;

} StringPool;

typedef struct
{
    StringPool pool;
    StringPool print_pool;
    StringPool trans_pool;
} StringMemory;

typedef struct FmtHeader FmtHeader;
typedef str (*Formatter)(FmtHeader*);

// STFO: this wouldn't allow me to create custom formatters
// that i can just put into the function ...
// -> unless i put in a function pointer to the expander function?
struct FmtHeader
{
    Formatter function;
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

} IntFormat;

typedef struct
{
    FmtHeader header;
    str value;
} StrFormat;

#endif
