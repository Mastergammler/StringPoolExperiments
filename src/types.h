#pragma once

#include <stdbool.h>

typedef struct
{
    // TODO: const? Is that right
    //-> Can this work? What happens if we change the
    // underlying memory?
    const char* chars;
    int len;
    bool null_terminated;
    // = child of another string
    // -> So buffer can not be guaranteed
    bool is_slice;
} str;

typedef struct
{
    char* memory;
    int max_size;
    int used;

} StringPool;

typedef struct
{
    StringPool pool;
    // add expander buffer
} StringPooling;
