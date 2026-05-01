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
    StrPoolOptions opt = {&StrMemory->print_buffer};
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
    print_ln(str_static(cstr), stream, args);
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
            *curC++ = HEX_DIGITS[(uint8_t)hiVal];
            *curC++ = HEX_DIGITS[(uint8_t)loVal];
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
        return str_static(TRUE_STR);
    }
    else
    {
        return str_static(FALSE_STR);
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

/*
 * Length excluding sign
 */
static int num_len(int num)
{
    int val = abs(num);
    int cmp = 10;
    int len = 1;

    while (val >= cmp && cmp > 0)
    {
        len++;
        cmp *= 10;
    }

    return len;
}

str format_int(StringPool* pool, int num, int places)
{
    bool isNeg = num & INT_SIGN;

    int numLen = num_len(num);
    // extra space for sign
    int extraSpace = isNeg ? 1 : 0;
    int padding = 0;

    if (places > numLen)
    {
        padding = places - numLen;
    }

    // NOTE : I know the length beforehand, so i could defer printing
    int textLen = numLen + extraSpace + padding;
    char* text = pool_use(pool, STR_SIZE(textLen));

    char* cur = text;
    if (isNeg)
    {
        num = abs(num);
        *cur++ = '-';
    }

    for (int i = 0; i < padding; i++)
    {
        *cur++ = '0';
    }

    for (int i = 1; i <= numLen; i++)
    {
        int cmp = pow(10, numLen - i);
        int leader = num / cmp;
        num -= leader * cmp;
        *cur++ = leader + ASCII_NUM_OFFSET;
    }

    *cur = 0;

    return (str){.chars = text, .len = textLen, .null_terminated = true};
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
    StrPoolOptions opt = {pool, true};

    str sign = f < 0 ? str_static("-") : (str){};
    // this is the reason for an expander pool
    // -> Because like this i would just pollute the actual string pool
    float abs = fabs(f);
    int high = abs;
    int factor = pow(10, decimals);
    float decShifted = (abs - high) * factor;
    int roundedNum = fabs(round(decShifted));

    if (decimals == 0)
    {
        // it's either 0 or 1 then
        // -> we might have a carry
        high += roundedNum;
        return str_formatc_opt(opt, "%%.", STR(sign), NUM(high));
    }

    if (roundedNum == factor)
    {
        // when we have carry then after comma is always 0
        // -> In that case we only have a single used value
        high++;
        roundedNum -= factor;
    }

    return str_formatc_opt(opt, "%%.%", STR(sign), NUM(high),
                           NUM_PAD(roundedNum, decimals));
}

/*
 * Example for a custom formatter
 */
str format_str(StringPool* pool, str string)
{
    str dataPreview = str_sub(string, 0, STR_PREVIEW_LEN);
    str previewDots = {0};
    if (string.len > STR_PREVIEW_LEN)
    {
        previewDots = str_static("...");
    }

    StrPoolOptions opt = {pool, true};
    str formatted = str_formatc_opt(
        opt, "str{ %: '%%', len: %, %, %, % }", PTR((void*)string.chars),
        STR(dataPreview), STR(previewDots), NUM(string.len),
        BOO(string.null_terminated), BOO(string.is_slice),
        BOO(string.is_static));
    return formatted;
}
