#include "../internal.h"

static const char FMT_PH = '%';
static const int PLACEHOLDER_LEN = 1;

str expand_float(FmtHeader* header)
{
    FloatFormat* format = (FloatFormat*)header;
    str expanded =
        format_float(&String_Mem.trans_pool, format->value, format->decimals);
    return expanded;
}

inline str format_valist(str formatter, va_list args)
{
    char* stringStart = pool_use(&String_Mem.pool, STR_SIZE(formatter.len));
    char* curWritePointer = stringStart;
    int stringLen = formatter.len;

    for (int i = 0; i < formatter.len; i++)
    {
        char c = formatter.chars[i];

        if (c == FMT_PH)
        {
            FmtHeader* h = va_arg(args, FmtHeader*);
            str expanded = h->function(h);
            stringLen += expanded.len - PLACEHOLDER_LEN;

            // TODO: gracefull handling of out of memory
            pool_use(&String_Mem.pool, expanded.len - PLACEHOLDER_LEN);
            memcpy(curWritePointer, expanded.chars, expanded.len);

            // NOTE: we allocated 1 less for this buffer, but we have +1
            //  from the original buffer, so we move the whole length of the
            //  strign we just combine all the sub buffers together, to greate
            //  one big one
            curWritePointer += expanded.len;
        }
        else
        {
            *curWritePointer++ = c;
        }
    }

    *curWritePointer = 0;
    pool_reset(&String_Mem.trans_pool);
    va_end(args);

    return (str){stringStart, stringLen, true};
}

str format(const char* formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    return format_valist(static_str(formatter), args);
}

// use header type -> declare size
// -> Then read args by size
// => Just read bytes after the first passed argument
str format_tmpl(str formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    return format_valist(formatter, args);
}
