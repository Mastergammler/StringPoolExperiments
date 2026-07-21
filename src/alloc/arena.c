#include "module.h"

void arena_init(Arena* arena, uint64_t capacity)
{
    arena->cursor = 0;
    arena->capaticy = capacity;
    arena->memory = malloc(capacity);

    JUST_ASSERT(arena->memory);
}

void* arena_use(Arena* arena, uint64_t bytes)
{
    uint64_t newCursor = arena->cursor + bytes;
    JUST_ASSERT(newCursor < arena->capaticy);
    // overflow
    JUST_ASSERT(newCursor >= arena->cursor);

    void* sectionStart = arena->memory + arena->cursor;
    arena->cursor = newCursor;

    return sectionStart;
}

void arena_reset(Arena* arena)
{
    arena->cursor = 0;
}

void arena_revert(Arena* arena, uint64_t size)
{
    if (arena->cursor >= size) arena->cursor -= size;
    arena->cursor = 0;
}

void* mglibc_mem_use_cb(void* alloc, uint64_t size)
{
    return arena_use((Arena*)alloc, size);
}
void mglibc_mem_revert_cb(void* alloc, uint64_t size)
{
    return arena_revert((Arena*)alloc, size);
}

AllocContext ctx_init_default(Arena* defaultAlloc)
{
    return (AllocContext){.alloc = (void*)defaultAlloc,
                          .alloc_fn = mglibc_mem_use_cb,
                          .revert_fn = mglibc_mem_revert_cb};
}
