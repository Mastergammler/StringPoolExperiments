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
    StringPool trans_pool;
} StringMemory;

#endif
