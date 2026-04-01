#include "internal.h"

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
