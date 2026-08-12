#include "../internal.h"

PrintHeader* num_formatter(FmtHeader* header)
{
    NumFormat* fmt = (NumFormat*)header;
    if (fmt->binary)
    {
        ASSERT(false, "Not implemented");
    }
    else
    {
        fmt->desc = extract_signed(fmt->value, fmt->places);
        return (PrintHeader*)&fmt->desc;
    }
}

PrintHeader* bool_formatter(FmtHeader* header)
{
    BoolFormat* fmt = (BoolFormat*)header;
    fmt->desc = extract_bool(fmt->value);
    return (PrintHeader*)&fmt->desc;
}

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
        return format_int(pool, format->value, format->places);
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

// STFO: this seems a bit too convoluted?
//  -> is this the best way of doing it?
str use_char(StringPool* pool, FmtHeader* header)
{
    CharFormat* format = (CharFormat*)header;
    return format_char(pool, format->value);
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
str format_valist(StrPoolOptions opt, str formatter, va_list args)
{
    int stringLen = formatter.len;
    int argCount = 0;
    int bufferIdx = get_and_reserve_arg_buf();
    FmtArg* buffer = FmtArgBuffers.buf[bufferIdx];

    for (int i = 0; i < formatter.len; i++)
    {
        if (formatter.chars[i] == FMT_PH)
        {
            ASSERT(argCount < FMT_ARG_BUF_MAX,
                   "Not supported: Exceeded max format arg count!");

            FmtHeader* h = va_arg(args, FmtHeader*);
            buffer[argCount].header = h;
            buffer[argCount].placholder_idx = i;

            if (h->fmt_fn)
            {
                buffer[argCount].print_header = NULL;
                buffer[argCount].expanded =
                    h->fmt_fn(&StrMem->pools[StrMem->transient_pool], h);
                stringLen += buffer[argCount].expanded.len - FMT_PH_LEN;
            }
            else if (h->info_fn)
            {
                buffer[argCount].expanded = (str){};
                buffer[argCount].print_header = h->info_fn(h);
                stringLen +=
                    buffer[argCount].print_header->str_len - FMT_PH_LEN;
            }
            else
            {
                ASSERT(false, "Missing formatting function! Unable to format!");
            }
            argCount++;
        }
    }

    // TODO: change design to use pool print for the substrings
    // -> I can just wrap everything into a string
    // -> And then expand the substring into the buffer
    // => Should be ez, then i don't need to do this wiredness
    //
    // => this might not work, because then i would add the pool endings by
    //    default but since i'm building a single string that should not
    //    happen in this case!
    char* stringStart = pool_use(POOL(opt.pool_idx),
                                 memory_size(POOL(opt.pool_idx), stringLen));
    char* writeCursor = stringStart;
    int curFmtPos = 0;
    for (int arg = 0; arg < argCount; arg++)
    {
        int fmtArgPos = buffer[arg].placholder_idx;
        PrintHeader* header = buffer[arg].print_header;

        // section before the insertion
        int fmtSectionLen = fmtArgPos - curFmtPos;
        memcpy(writeCursor, formatter.chars + curFmtPos, fmtSectionLen);
        writeCursor += fmtSectionLen;

        // place cursor after placeholder
        curFmtPos += fmtSectionLen + FMT_PH_LEN;

        if (header)
        {
            // FIXME: these are kinda incompatible
            //  -> because this also calls: pool_use ...
            str printed = header->print_fn(writeCursor, header);
            writeCursor += printed.len;
        }
        else
        {
            str expanded = buffer[arg].expanded;
            memcpy(writeCursor, expanded.chars, expanded.len);
            writeCursor += expanded.len;
        }
    }

    if (curFmtPos < formatter.len)
    {
        int fmtSectionLen = formatter.len - curFmtPos;
        memcpy(writeCursor, formatter.chars + curFmtPos, fmtSectionLen);
        writeCursor += fmtSectionLen;
    }

    stringLen += finalize_str(POOL(opt.pool_idx), stringStart, stringLen);

    release_arg_buf(bufferIdx);
    // NOTE: sometimes we need to keep the transient buffer
    // because the call is a SUB formatting of another string
    // so we can't clear it yet, else strings get overwritten
    if (!opt.keep_transients)
    {
        str_pool_reset((StrPoolOptions){StrMem->transient_pool});
    }
    va_end(args);

    return (str){stringStart, stringLen,
                 !POOL(opt.pool_idx)->neglect_null_termination};
}

/*
 * For internal use, might need to keep the transient buffer
 *
 * PERF: is the opt object with it's own static pool too much overhead?
 * Or is that fine?
 */
str str_formatc_opt(StrPoolOptions opt, const char* formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    return format_valist(opt, str_static(formatter), args);
}

str str_format_opt(StrPoolOptions opt, str formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    return format_valist(opt, formatter, args);
}

// for external use -> clears transient buffer
str str_formatc(const char* formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    StrPoolOptions opt = {StrMem->default_pool};
    return format_valist(opt, str_static(formatter), args);
}

// for external use -> clears transient buffer
str str_format(str formatter, ...)
{
    va_list args;
    va_start(args, formatter);
    StrPoolOptions opt = {StrMem->default_pool};
    return format_valist(opt, formatter, args);
}
