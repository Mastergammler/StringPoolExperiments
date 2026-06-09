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

str string_join(StringPool* pool, str* strings, int count, str delim)
{
    int joinedLen = 0;
    int lastEl = count - 1;
    for (int i = 0; i < count; i++)
    {
        joinedLen += strings[i].len;
        if (i != lastEl) joinedLen += delim.len;
    }

    char* strStart = pool_use(pool, memory_size(pool, joinedLen));
    char* strPos = strStart;

    for (int i = 0; i < count; i++)
    {
        str cur = strings[i];

        memcpy(strPos, cur.chars, cur.len);
        strPos += cur.len;
        if (i != lastEl)
        {
            memcpy(strPos, delim.chars, delim.len);
            strPos += delim.len;
        }
    }

    joinedLen += finalize_str(pool, strStart, joinedLen);

    str joined = {};
    joined.len = joinedLen;
    joined.chars = strStart;
    joined.null_terminated = !pool->neglect_null_termination;
    joined.is_slice = false;
    joined.is_static = false;

    return joined;
}

void string_append(StringPool* pool, str* src, str extension)
{
    int combinedLen = src->len + extension.len;
    char* strStart = pool_use(pool, memory_size(pool, combinedLen));

    memcpy(strStart, src->chars, src->len);
    memcpy(strStart + src->len, extension.chars, extension.len);

    combinedLen += finalize_str(pool, strStart, combinedLen);

    src->len = combinedLen;
    src->chars = strStart;
    src->null_terminated = !pool->neglect_null_termination;
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

    char* mem = pool_use(pool, memory_size(pool, len));
    char* writePos = mem;

    for (int i = 0; i < num; i++)
    {
        for (int c = 0; c < src.len; c++)
        {
            *writePos++ = src.chars[c];
        }
    }

    len += finalize_str(pool, mem, len);

    return (str){mem, len, !pool->neglect_null_termination};
}

StrSplitResult string_split_last(str src, char splitAt, StrSplitOpt opt)
{
    StrSplitResult res = {};
    res.head = src;
    res.head.null_terminated = false;
    res.head.is_slice = true;
    res.last.null_terminated = src.null_terminated;
    res.last.is_slice = true;

    for (int i = src.len - 1; i >= 0; i--)
    {
        if (src.chars[i] == splitAt)
        {
            res.head.len = i;
            res.last.chars = src.chars + (i + 1);
            res.last.len = src.len - (i + 1);

            break;
        }
    }

    return res;
}
