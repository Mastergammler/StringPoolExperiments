#include "../internal.h"

str expand_float(StringPool* pool, FmtHeader* header)
{
    FloatFormat* format = (FloatFormat*)header;
    str expanded = format_float(pool, format->value, format->decimals);
    return expanded;
}

str expand_int(StringPool* pool, FmtHeader* header)
{
    IntFormat* format = (IntFormat*)header;

    if (format->binary)
    {
        return format_binary(pool, format->value);
    }
    else
    {
        return format_int(pool, format->value);
    }
}

str expand_bool(StringPool* pool, FmtHeader* header)
{
    BoolFormat* format = (BoolFormat*)header;

    return format_bool(pool, format->value);
}

str expand_str(StringPool* pool, FmtHeader* header)
{
    StrFormat* format = (StrFormat*)header;
    return format_str(pool, format->value);
}

str use_str(StringPool* pool, FmtHeader* header)
{
    StrFormat* format = (StrFormat*)header;
    return format->value;
}

str expand_ptr(StringPool* pool, FmtHeader* header)
{
    PtrFormat* format = (PtrFormat*)header;
    return format_ptr(pool, format->value);
}

/*
 * NOTE: Not thread safe, becaues of the use of FmtArgBuffer
 * -> This buffer can only be used by one thread at a time
 * => There is no mechanism to manage access of multiple threads!
 */
str format_valist(StorageOptions opt, str formatter, va_list args)
{
    int stringLen = formatter.len;
    int argCount = 0;
    int bufferIdx = get_and_reserve_arg_buf();
    FmtArg* buffer = FmtArgBuffers.buf[bufferIdx];

    for (int i = 0; i < formatter.len; i++)
    {
        if (formatter.chars[i] == FMT_PH)
        {
            assert(argCount < FMT_ARG_BUF_MAX);

            FmtHeader* h = va_arg(args, FmtHeader*);
            buffer[argCount].header = h;
            buffer[argCount].placholder_idx = i;
            buffer[argCount].expanded = h->fmt_fn(&String_Mem.transient, h);
            stringLen += buffer[argCount].expanded.len - PLACEHOLDER_LEN;
            argCount++;
        }
    }

    char* stringStart = pool_use(opt.pool, STR_SIZE(stringLen));
    char* writePos = stringStart;
    int formatterPos = 0;
    for (int arg = 0; arg < argCount; arg++)
    {
        int fmtArgPos = buffer[arg].placholder_idx;
        str expanded = buffer[arg].expanded;
        int fmtSectionLen = fmtArgPos - formatterPos;
        memcpy(writePos, formatter.chars + formatterPos, fmtSectionLen);
        writePos += fmtSectionLen;

        // skip placeholder
        formatterPos += fmtSectionLen + PLACEHOLDER_LEN;
        memcpy(writePos, expanded.chars, expanded.len);
        writePos += expanded.len;
    }

    if (formatterPos < formatter.len)
    {
        int fmtSectionLen = formatter.len - formatterPos;
        memcpy(writePos, formatter.chars + formatterPos, fmtSectionLen);
        writePos += fmtSectionLen;
    }

    *writePos = 0;

    release_arg_buf(bufferIdx);
    // NOTE: sometimes we need to keep the transient buffer
    // because the call is a SUB formatting of another string
    // so we can't clear it yet, else strings get overwritten
    if (!opt.keep_transients)
    {
        pool_reset(&String_Mem.transient);
    }
    va_end(args);

    return (str){stringStart, stringLen, true};
}

/*
 * For internal use, might need to keep the transient buffer
 *
 * PERF: is the opt object with it's own static pool too much overhead?
 * Or is that fine?
 */
str format_pool(StorageOptions opt, const char* formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    return format_valist(opt, staticstr(formatter), args);
}

// for external use -> clears transient buffer
str format(const char* formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    StorageOptions opt = {&String_Mem.persistent};
    return format_valist(opt, staticstr(formatter), args);
}

// for external use -> clears transient buffer
str formatstr(str formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    StorageOptions opt = {&String_Mem.persistent};
    return format_valist(opt, formatter, args);
}
