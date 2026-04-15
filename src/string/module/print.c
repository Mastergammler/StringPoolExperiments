#include "../internal.h"

static const char TRUE_STR[] = "true";
static const char FALSE_STR[] = "false";
static const char HEX_DIGITS[] = "0123456789abcdef";

#define INT_SIGN (1U << 31)
#define ASCII_NUM_OFFSET 48

#define FMT_STR_SIZE 14
#define FMT_F_SIZE 3
#define STR_PREVIEW_LEN 13

void print(str string)
{
    printf("%.*s", string.len, string.chars);
}

// STFO: needs explicit fflush(stdout) ?
// - handle fully buffered / piped mode
void print_ln(str tmpl, FILE* stream, va_list args)
{
    str string = format_valist(tmpl, args);
    if (string.len == 0)
    {
        fputs("<empty>\n", stream);
    }
    else
    {
        fwrite(string.chars, 1, string.len, stream);
        fputs("\n", stream);
    }
}

void print_str(const char* cstr, FILE* stream, va_list args)
{
    print_ln(static_str(cstr), stream, args);
}

// STFO: add null terminators?
//  -> or is this redundant?
//  => Because it supposed to happend in the expand buffer?
str format_ptr(StringPool* pool, void* ptr)
{
    // 2 hex values for each byte
    int bytes = sizeof(void*);
    int printLen = bytes * 2;
    // '0x' prefix + null-term
    int size = printLen + 3;

    // printf("Pointer %p: %i bytes, len: %i, total: %i\n", ptr, bytes,
    // printLen, size);
    char* curC = pool_check_next(pool, size);

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

    char* actualStr = pool_use(pool, actualLen);

    return (str){
        .chars = actualStr, .len = actualLen - 1, .null_terminated = true};
}

str format_bool(StringPool* pool, bool boolean)
{
    // 'false'
    int maxLen = 5;
    int maxSize = maxLen + 1;

    pool_check_next(pool, maxSize);
    if (boolean)
    {
        return string_alloc(pool, TRUE_STR);
    }
    else
    {
        return string_alloc(pool, FALSE_STR);
    }
}

str format_binary(StringPool* pool, int num)
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

    return (str){
        .chars = actualStr, .len = actualLength - 1, .null_terminated = true};
}

str format_int(StringPool* pool, int num)
{
    bool isNeg = num & INT_SIGN;
    // 2 Bil + sign
    int maxLen = 11;
    // + null term
    int maxSize = 12;

    int actualLen = 0;

    char* curC = pool_check_next(pool, maxSize);
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

    char* actualStr = pool_use(pool, actualLen);

    return (str){
        .chars = actualStr, .len = actualLen - 1, .null_terminated = true};
}

/*
 * for small strings, padding hughe strings might be inefficient!
 */
str format_pad_left(StringPool* pool, str original, char c, int n)
{
    if (original.len >= n) return original;

    int padAmount = n - original.len;
    int memoryLen = n + 1;
    char* curC = pool_check_next(pool, memoryLen);

    for (int pad = 0; pad < padAmount; pad++)
    {
        *curC++ = c;
    }
    for (int i = 0; i < original.len; i++)
    {
        *curC++ = original.chars[i];
    }
    *curC = 0;

    char* newStr = pool_use(pool, memoryLen);
    return (str){
        .chars = newStr, .len = memoryLen - 1, .null_terminated = true};
}

str format_float(StringPool* pool, float f, int decimals)
{
    // this is the reason for an expander pool
    // -> Because like this i would just pollute the actual string pool
    int high = f;
    str num = format_int(&String_Mem.trans_pool, high);
    int decimalNum = (f - high) * pow(10, decimals);
    str lo = format_int(&String_Mem.trans_pool, decimalNum);

    // TODO: needs to be variable
    //  -> per decimal point etc
    str format[FMT_F_SIZE] = {
        num, static_str("."),
        format_pad_left(&String_Mem.trans_pool, lo, '0', decimals)};

    int totalLen = 1;
    for (int i = 0; i < FMT_F_SIZE; i++)
    {
        totalLen += format[i].len;
    }

    char* curC = pool_check_next(pool, totalLen);

    for (int i = 0; i < FMT_F_SIZE; i++)
    {
        str cur = format[i];
        memcpy(curC, cur.chars, cur.len);
        curC += cur.len;
    }
    *curC = 0;

    char* actualStr = pool_use(pool, totalLen);
    pool_reset(&String_Mem.trans_pool);

    return (str){
        .chars = actualStr, .len = totalLen - 1, .null_terminated = true};
}

/*
 * testing a custom formatter
 */
str format_str(StringPool* pool, str string)
{
    // doing it the bad way first
    str ptr = format_ptr(&String_Mem.trans_pool, (void*)string.chars);
    str dataPreview = string;
    str previewDots = {0};
    if (string.len > STR_PREVIEW_LEN)
    {
        dataPreview.len = STR_PREVIEW_LEN;
        previewDots = static_str("...");
    }

    str len = format_int(&String_Mem.trans_pool, string.len);
    str nullT = format_bool(&String_Mem.trans_pool, string.null_terminated);
    str isSlice = format_bool(&String_Mem.trans_pool, string.is_slice);
    str isStatic = format_bool(&String_Mem.trans_pool, string.is_static);

    // TODO: this sucks, this is way to hard to define
    //  -> we need some good way to expand this right into the final buffer i
    //  think
    //  => But then i don't quite know the end size is the problem ...
    //  => Well, i mean i could know the max size again ...

    str format[FMT_STR_SIZE] = {static_str("str{ "),
                                ptr,
                                static_str(": '"),
                                dataPreview,
                                previewDots,
                                static_str("', len: "),
                                len,
                                static_str(", "),
                                nullT,
                                static_str(", "),
                                isSlice,
                                static_str(", "),
                                isStatic,
                                static_str(" }")};

    int totalLen = 1;
    for (int i = 0; i < FMT_STR_SIZE; i++)
    {
        totalLen += format[i].len;
    }

    // We certainly want to add formating here, else it doesn't make much sense
    // -> but it would make sense to expand in place also?
    // => Becaues else we copy data twice ...

    char* curC = pool_check_next(pool, totalLen);

    for (int i = 0; i < FMT_STR_SIZE; i++)
    {
        str cur = format[i];
        memcpy(curC, cur.chars, cur.len);
        curC += cur.len;
    }
    *curC = 0;

    char* actualStr = pool_use(pool, totalLen);
    pool_reset(&String_Mem.trans_pool);

    return (str){
        .chars = actualStr, .len = totalLen - 1, .null_terminated = true};
}
