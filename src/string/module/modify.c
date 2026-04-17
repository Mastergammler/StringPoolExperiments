#include "../internal.h"

str str_sub(str parent, int from, int toExclusive)
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
                 .is_slice = true,
                 .is_static = parent.is_static};
}

void string_append(StringPool* pool, str* src, str extension)
{
    int combinedLen = src->len + extension.len;
    char* strStart = pool_use(pool, STR_SIZE(combinedLen));

    memcpy(strStart, src->chars, src->len);
    memcpy(strStart + src->len, extension.chars, extension.len);
    strStart[combinedLen] = 0;

    src->len = combinedLen;
    src->chars = strStart;
    src->null_terminated = true;
    src->is_slice = false;
    src->is_static = false;

    // TODO:
    //  SLOT MANAGEMENT
    //  mark first one as empty slot
    //  add print slot to debug print
}

str string_repeat(StringPool* pool, str src, int num)
{
    int len = src.len * num;
    if (len <= 0) return (str){0};

    char* mem = pool_use(pool, STR_SIZE(len));
    char* writePos = mem;

    for (int i = 0; i < num; i++)
    {
        for (int c = 0; c < src.len; c++)
        {
            *writePos++ = src.chars[c];
        }
    }

    *writePos = 0;

    return (str){mem, len, true};
}
