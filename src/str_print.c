#include "internal.h"
#include "types.h"
#include <string.h>

void print(str string)
{
    printf("%.*s", string.len, string.chars);
}

// STFO: needs explicit fflush(stdout) ?
// - handle fully buffered / piped mode
void println(str string)
{
    if (string.len == 0)
    {
        fputs("<empty>\n", stdout);
    }
    else
    {
        fwrite(string.chars, 1, string.len, stdout);
        fputs("\n", stdout);
    }
}

void printstr(const char* cstr)
{
    println(from_cstr(cstr));
}

void debug_print_pool(StringPool pool, int stride)
{
    printf("-------------------------------------------\n");
    for (int i = 0; i < pool.max_size; i++)
    {
        unsigned char c = pool.memory[i];

        if (c == 0) c = '|';

        if ((i + 1) % stride == 0)
        {

            printf("%c\n", c);
        }
        else
        {
            printf("%c", c);
        }
    }
    printf("-------------------------------------------\n");
}

static const char HEX_DIGITS[] = "0123456789abcdef";

// STFO: add null terminators?
//  -> or is this redundant?
//  => Because it supposed to happend in the expand buffer?
str fmt_p(void* ptr)
{
    // 2 hex values for each byte
    int bytes = sizeof(void*);
    int printLen = bytes * 2;
    // '0x' prefix + null-term
    int size = printLen + 3;

    // printf("Pointer %p: %i bytes, len: %i, total: %i\n", ptr, bytes,
    // printLen, size);
    char* curC = pool_check_next(&Strings.pool, size);

    uintptr_t ptrVal = (uintptr_t)ptr;

    *curC++ = '0';
    *curC++ = 'x';
    int actualLen = 2;

    bool stringStarted = false;

    // print from high to lo
    for (int i = bytes - 1; i >= 0; i--)
    {
        char hiVal = (ptrVal >> (i * 8 + 4)) & 0x0F;
        char loVal = (ptrVal >> (i * 8)) & 0x0F;

        if (stringStarted || (hiVal || loVal))
        {
            *curC++ = HEX_DIGITS[hiVal];
            *curC++ = HEX_DIGITS[loVal];
            actualLen += 2;
        }
    }

    *curC = 0;
    actualLen++;

    char* actualStr = pool_use(&Strings.pool, actualLen);

    return (str){.chars = actualStr,
                 .len = actualLen - 1,
                 .null_terminated = true,
                 .is_slice = false};
}

static const char TRUE_STR[] = "true";
static const char FALSE_STR[] = "false";

str fmt_bool(bool boolean)
{
    // 'false'
    int maxLen = 5;
    int maxSize = maxLen + 1;

    pool_check_next(&Strings.pool, maxSize);
    if (boolean)
    {
        return str_alloc(&Strings.pool, TRUE_STR);
    }
    else
    {
        return str_alloc(&Strings.pool, FALSE_STR);
    }
}

str fmt_b(StringPool* pool, int num)
{
    // 1 char ber bit - 8 bits/byte
    int maxLen = sizeof(num) * 8;
    // null terminator
    int maxSize = maxLen + 1;

    char* curC = pool_check_next(pool, maxSize);

    // print from left to right
    bool stringStarted = false;
    int actualLength = 0;
    for (int i = maxLen - 1; i >= 0; i--)
    {
        int bit = (num >> i) & 0x1;

        if (bit == 0x1)
        {
            *curC++ = '1';
            stringStarted = true;
            actualLength++;
        }
        else if (stringStarted)
        {
            *curC++ = '0';
            actualLength++;
        }
    }

    *curC = 0;
    actualLength++;

    char* actualStr = pool_use(pool, actualLength);

    return (str){.chars = actualStr,
                 .len = actualLength - 1,
                 .null_terminated = true,
                 .is_slice = false};
}

#define INT_SIGN (1U << 31)
#define ASCII_NUM_OFFSET 48

str fmt_i(int num)
{
    bool isNeg = num & INT_SIGN;
    // 2 Bil + sign
    int maxLen = 11;
    // + null term
    int maxSize = 12;

    int actualLen = 0;

    char* curC = pool_check_next(&Strings.pool, maxSize);
    char* startC = curC;

    if (isNeg)
    {
        num = -num;
    }

    int next = 1;
    while (next)
    {
        int remainder = num % 10;
        *curC++ = remainder + ASCII_NUM_OFFSET;
        actualLen++;

        num = num / 10;
        next = num;
    }

    if (isNeg)
    {
        *curC++ = '-';
        actualLen++;
    }

    // swap indices
    int lastIdx = actualLen - 1;
    for (int i = 0; i < actualLen / 2; i++)
    {
        int first = i;
        int last = lastIdx - i;

        if (last > first)
        {
            char tmp = startC[first];
            startC[first] = startC[last];
            startC[last] = tmp;
        }
    }

    // TODO: reverse string

    *curC = 0;
    actualLen++;

    char* actualStr = pool_use(&Strings.pool, actualLen);

    return (str){.chars = actualStr,
                 .len = actualLen - 1,
                 .null_terminated = true,
                 .is_slice = false};
}

#define FMT_STR_SIZE 9
/*
 * testing a custom formatter
 */
str fmt_str(str string)
{
    // doing it the bad way first
    str ptr = fmt_p((void*)string.chars);
    str len = fmt_i(string.len);
    str nullT = fmt_bool(string.null_terminated);
    str isSlice = fmt_bool(string.is_slice);

    // TODO: this sucks, this is way to hard to define
    //  -> we need some good way to expand this right into the final buffer i
    //  think
    //  => But then i don't quite know the end size is the problem ...
    //  => Well, i mean i could know the max size again ...
    str format[FMT_STR_SIZE] = {
        from_cstr("str{ "), ptr,   from_cstr(", len: "), len,
        from_cstr(", "),    nullT, from_cstr(", "),      isSlice,
        from_cstr(" }")};

    int totalLen = 1;
    for (int i = 0; i < FMT_STR_SIZE; i++)
    {
        totalLen += format[i].len;
    }

    // We certainly want to add formating here, else it doesn't make much sense
    // -> but it would make sense to expand in place also?
    // => Becaues else we copy data twice ...

    char* curC = pool_check_next(&Strings.pool, totalLen);

    for (int i = 0; i < FMT_STR_SIZE; i++)
    {
        str cur = format[i];
        memcpy(curC, cur.chars, cur.len);
        curC += cur.len;
    }
    *curC = 0;

    char* actualStr = pool_use(&Strings.pool, totalLen);

    return (str){.chars = actualStr,
                 .len = totalLen - 1,
                 .null_terminated = true,
                 .is_slice = false};
}
