#ifndef MGLIBC_ALLOC
#define MGLIBC_ALLOC

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint64_t capaticy;
    uint64_t cursor;
    void* memory;
} Arena;

void arena_init(Arena* arena, uint64_t capacity);
void* arena_use(Arena* arena, uint64_t bytes);
void arena_reset(Arena* arena);

#endif // MGLIBC_ALLOC
