#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef size_t u64;

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

void pool_init(StringPool* pool, int size)
{
    pool->memory = malloc(size);
    pool->used = 0;
    pool->max_size = size;

    memset(pool->memory, 0xf8, size);
}

// copy string into own pool
// -> including null terminator
str str_alloc(StringPool* pool, const char* cstr)
{
    int len = strlen(cstr);
    assert(pool->used + len + 1 < pool->max_size);

    char* strStart = pool->memory + pool->used;
    pool->used += len + 1;
    memcpy(strStart, cstr, len + 1);

    return (str){.chars = strStart,
                 .len = len,
                 .null_terminated = true,
                 .is_slice = false};
}

// STFO: do we wanna do this?
//-> copy things from cstrings?
//=> Usually probably not as much
str from_cstr(const char* cstr)
{
    return (str){.chars = cstr,
                 .len = strlen(cstr),
                 .null_terminated = true,
                 .is_slice = false};
}

int clamp_min(int value, int floorInc)
{
    return value < floorInc ? floorInc : value;
}

int clamp_max(int value, int ceilingEx)
{
    return value >= ceilingEx ? ceilingEx : value;
}

int clamp(int value, int min, int max)
{
    int minned = clamp_min(value, min);
    return clamp_max(minned, max);
}

str substr(str parent, int from, int toExclusive)
{
    if (from > toExclusive)
    {
        int tmp = from;
        from = toExclusive;
        toExclusive = tmp;
    }

    int fromClamped = clamp(from, 0, parent.len);
    int toClamped = clamp(toExclusive, 0, parent.len);
    int newLen = toClamped - fromClamped;

    return (str){.chars = parent.chars + fromClamped,
                 .len = newLen,
                 .null_terminated =
                     toExclusive == true && parent.null_terminated,
                 .is_slice = true};
}

void append(StringPool* pool, str* src, str extension)
{
    int newLen = src->len + extension.len;
    assert(pool->used + newLen + 1 < pool->max_size);

    char* strStart = pool->memory + pool->used;
    pool->used += newLen + 1;

    memcpy(strStart, src->chars, src->len);
    memcpy(strStart + src->len, extension.chars, extension.len);
    strStart[newLen] = 0;

    src->len = newLen;
    src->chars = strStart;
    src->null_terminated = true;
    src->is_slice = false;

    // TODO:
    //  SLOT MANAGEMENT
    //  mark first one as empty slot
    //  add print slot to debug print
}

void print(str string)
{
    printf("%.*s", string.len, string.chars);
}

void println(str string)
{
    if (string.len == 0)
    {
        printf("<empty>\n");
    }
    else
    {
        printf("%.*s\n", string.len, string.chars);
    }
}

void debug_print_pool(StringPool pool, int stride)
{
    printf("-------------------------------------------\n");
    for (int i = 0; i < pool.max_size; i++)
    {
        unsigned char c = pool.memory[i];

        if (c == 0) c = '|';

        if ((i + 1) % stride == 0)
        {

            printf("%c\n", c);
        }
        else
        {
            printf("%c", c);
        }
    }
    printf("-------------------------------------------\n");
}

int main(int argc, char* argv[])
{
    StringPool mem = {};
    pool_init(&mem, 1024 * 1);

    str hello = str_alloc(&mem, "Hello, world!");
    str second = str_alloc(&mem, "This is just another test string");
    str third = str_alloc(&mem, "This is the third one we have there");
    str staticAlloc =
        from_cstr("The world is static, and static is the world!");

    str sub = substr(hello, -5, 5);
    str sub2 = substr(hello, 2000, 5);
    str sub3 = substr(hello, -10, -5);
    str sub4 = substr(hello, 20, 40);

    println(hello);
    println(sub);
    println(sub2);
    println(sub3);
    println(sub4);

    append(&mem, &hello, staticAlloc);

    debug_print_pool(mem, 64);
    return 0;
}
