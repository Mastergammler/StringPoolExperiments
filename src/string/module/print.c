#include "../internal.h"
#include "../macros.h"

static const char TRUE_STR[] = "true";
static const char FALSE_STR[] = "false";
static const char HEX_DIGITS[] = "0123456789abcdef";

#define INT_SIGN (1U << 31)
#define ASCII_NUM_OFFSET 48

#define FMT_STR_SIZE 14
#define FMT_F_SIZE 3
#define STR_PREVIEW_LEN 13

// STFO: needs explicit fflush(stdout) ?
// - handle fully buffered / piped mode
void print_ln(str tmpl, FILE* stream, va_list args)
{
    // writing into the print buffer, which will be reused
    // on 'overflow' it will just overwrite previous strings
    // For async handling, the buffer needs to be large enough,
    // that new strings don't override previous ones and lead to garbage
    // print output
    StorageOptions opt = {&String_Mem.print_buffer};
    str string = format_valist(opt, tmpl, args);
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
    print_ln(staticstr(cstr), stream, args);
}

str format_ptr(StringPool* pool, void* ptr)
{
    // 2 hex values for each byte
    int bytes = sizeof(void*);
    int printLen = bytes * 2;
    // '0x' prefix + null-term
    int size = printLen + 3;

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
        return staticstr(TRUE_STR);
    }
    else
    {
        return staticstr(FALSE_STR);
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
    float decShifted = (f - high) * pow(10, decimals);
    int roundedNum = round(decShifted);

    int decimalPlacesUsed = decimals;
    for (int i = decimals - 1; i >= 0; i--)
    {
        int placesMin = pow(10, i);
        if (roundedNum >= placesMin)
        {
            decimalPlacesUsed = (i + 1);
            break;
        }
    }

    int padding =
        decimals > decimalPlacesUsed ? decimals - decimalPlacesUsed : 0;

    str padStr = string_repeat(&String_Mem.transient, staticstr("0"), padding);
    StorageOptions opt = {pool, true};
    return format_pool(opt, "%.%%", INT(high), STR(padStr), INT(roundedNum));
}

/*
 * Example for a custom formatter
 */
str format_str(StringPool* pool, str string)
{
    str dataPreview = substr(string, 0, STR_PREVIEW_LEN);
    str previewDots = {0};
    if (string.len > STR_PREVIEW_LEN)
    {
        previewDots = staticstr("...");
    }

    StorageOptions opt = {pool, true};
    str formatted = format_pool(opt, "str{ %: '%%', len: %, %, %, % }",
                                PTR((void*)string.chars), STR(dataPreview),
                                STR(previewDots), INT(string.len),
                                BOOL(string.null_terminated),
                                BOOL(string.is_slice), BOOL(string.is_static));
    return formatted;
}
