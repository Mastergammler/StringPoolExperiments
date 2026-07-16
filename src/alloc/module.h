#ifndef MGLIBC_ALLOC_MODULE
#define MGLIBC_ALLOC_MODULE

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "array.h"

typedef struct
{
    uint64_t capaticy;
    uint64_t cursor;
    void* memory;
} Arena;

void arena_init(Arena* arena, uint64_t capacity);
void* arena_use(Arena* arena, uint64_t bytes);
void arena_reset(Arena* arena);
void arena_revert(Arena* arena, uint64_t size);

static void* mem_use_cb(void* alloc, uint64_t size)
{
    return arena_use((Arena*)alloc, size);
}
static void mem_revert_cb(void* alloc, uint64_t size)
{
    return arena_revert((Arena*)alloc, size);
}

#endif // MGLIBC_ALLOC_MODULE
