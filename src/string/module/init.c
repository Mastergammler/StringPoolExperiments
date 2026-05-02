#include "../internal.h"

void pool_init(StringPool* pool, uint64_t size)
{
    pool->memory = malloc(size);
    pool->cursor_idx = 0;
    pool->capacity = size;

    memset(pool->memory, SENTINEL, size);
}

/*
 * NOTE: because of build order problems, we just take a pointer here to the
 * struct, so that we don't need to depend on a 'extern' value
 * Because else we might run into trouble during linking (catch 22)
 */
void str_init(StringMemory* allocation, uint64_t poolSize, uint64_t printBuffer,
              uint64_t frameBuffer, uint64_t expandBufferSize)
{
    StrMemory = allocation;
    StrMemory->print_buffer.ring_buffer = true;
    StrMemory->print_buffer.neglect_null_termination = true;
    StrMemory->print_buffer.add_linefeed = true;

    pool_init(&StrMemory->persistent, poolSize);
    pool_init(&StrMemory->print_buffer, printBuffer);
    pool_init(&StrMemory->frame_buffer, frameBuffer);
    pool_init(&StrMemory->transient, expandBufferSize);
}
