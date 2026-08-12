#include "../internal.h"

void count_realloc(StrPoolOptions opt, str* printed, str tail)
{
    if (!tail.len) return;

    // buffer wrapped, need to realloc
    if (tail.chars < printed->chars)
    {
        // we need to reset again, because we want to start over
        str_pool_reset(opt);
        uint64_t currentLen = printed->len + tail.len;
        char* newLoc = pool_use(opt, currentLen);

        // copy the tail first, because it also exists at the new location,
        // so we need to copy it before overriding it with the 'printed' data
        if (tail.len > printed->len)
        {
            // STFO: It says, it is copied 'AS IF' it would be moved to a tmp
            // array
            //  -> So how does it work actually then? It would be sufficient to
            //  copy the region from back to front here with no trouble, just
            //  would need to do it sequentially (no SIMD etc)
            // OPTIMIZE: Maybe this can be optimize
            // But probably YAGNI -> it would be better to prevent realloc
            // all together by better str len tracking
            memmove(newLoc + printed->len, tail.chars, tail.len);
        }
        else
        {
            memcpy(newLoc + printed->len, tail.chars, tail.len);
        }

        memcpy(newLoc, printed->chars, printed->len);

        printed->chars = newLoc;
    }
    printed->len += tail.len;
}

/*
 * NOTE: Not thread safe, becaues of the use of FmtArgBuffer
 * -> This buffer can only be used by one thread at a time
 * => There is no mechanism to manage access of multiple threads!
 *
 * I guess since there is a locking mechanism, it could work with multiple
 * threads as well, because bools should be atomic, therefore it might work
 * but it's not tested or designed with this case in mind
 */
str format_valist(StrPoolOptions opt, str formatter, va_list args)
{
    bool isParentFormatter = !opt.substring_mode;

    uint32_t stringLen = formatter.len;
    uint32_t estExpandedLen = 0;
    int argCount = 0;
    int bufferIdx = get_and_reserve_arg_buf();
    FmtArg* buffer = FmtArgBuffers.buf[bufferIdx];

    for (int i = 0; i < formatter.len; i++)
    {
        if (formatter.chars[i] == FMT_PH)
        {
            ASSERT(argCount < FMT_ARG_BUF_MAX,
                   "Not supported: Exceeded max format arg count!");

            // TODO: VALIDATION:
            // could i not validate the arg count right here?
            //  -> I guess i would need a magic value for it?
            //  => Or is there a way to get the args count?
            FmtHeader* h = va_arg(args, FmtHeader*);
            buffer[argCount].header = h;
            buffer[argCount].placholder_pos = i;
            ASSERT(h->fmt_fn, "Missing formatting function! Unable to format!");

            // substract the placeholder
            stringLen--;
            argCount++;
            estExpandedLen = h->est_len ? h->est_len : AVERAGE_PH_LEN;
        }
    }

    // PERF: this is a preemtive check, if we already know the string will not
    // fit. Then we can avoid the 'realloc' of the string we already written
    // till now, and just write from the beginning anyway
    if (!pool_fits_till_end(opt, stringLen + estExpandedLen))
    {
        // just sets the cursor to 0 -> so we start writing from the start
        str_pool_reset(opt);
    }

    // substring mode is used here, because we're actually building a single
    // string, but using multiple print (pool_use) operations for it
    if (isParentFormatter) opt.substring_mode = true;

    char* stringStart = pool_use(opt, 0);
    str resultStr = {stringStart, 0};

    int curFmtPos = 0;
    int writtenLen = 0;
    for (int arg = 0; arg < argCount; arg++)
    {
        int fmtArgPos = buffer[arg].placholder_pos;

        // section before the insertion
        int fmtSectionLen = fmtArgPos - curFmtPos;

        str beforePh = {.chars = formatter.chars + curFmtPos,
                        .len = fmtSectionLen};

        str written =
            write_str(opt, (FmtHeader*)&(StrFormat){.value = beforePh});
        count_realloc(opt, &resultStr, written);

        // place cursor after placeholder
        curFmtPos += fmtSectionLen + FMT_PH_LEN;

        // write replacement into final string
        str expanded = buffer[arg].header->fmt_fn(opt, buffer[arg].header);
        stringLen += expanded.len;
        count_realloc(opt, &resultStr, expanded);
    }

    if (curFmtPos < formatter.len)
    {
        int fmtSectionLen = formatter.len - curFmtPos;
        str beforePh = {.chars = formatter.chars + curFmtPos,
                        .len = fmtSectionLen};
        str written =
            write_str(opt, (FmtHeader*)&(StrFormat){.value = beforePh});
        count_realloc(opt, &resultStr, written);
    }

    if (isParentFormatter)
    {
        opt.substring_mode = false;
        char* strEnd = pool_use(opt, memory_size(opt, 0));
        int strLen = finalize_str(opt, strEnd, 0);
        count_realloc(opt, &resultStr, (str){strEnd, strLen});
    }

    release_arg_buf(bufferIdx);
    va_end(args);

    return (str){resultStr.chars, resultStr.len, POOL_DEFAULTS(opt)};
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
