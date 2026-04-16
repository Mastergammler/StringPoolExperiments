#include "../internal.h"

void pool_init(StringPool* pool, uint64_t size)
{
    pool->memory = malloc(size);
    pool->cursor_idx = 0;
    pool->capacity = size;

    memset(pool->memory, SENTINEL, size);
}

void strings_init(uint64_t poolSize, uint64_t printBuffer,
                  uint64_t expandBufferSize)
{
    String_Mem.print_buffer.ring_buffer = true;

    pool_init(&String_Mem.persistent, poolSize);
    pool_init(&String_Mem.print_buffer, printBuffer);
    pool_init(&String_Mem.transient, expandBufferSize);
}
