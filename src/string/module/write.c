#include "../internal.h"
#include "../macros.h"

str write_int_strategy(StrPoolOptions opt, FmtHeader* header)
{
    NumFormat* format = (NumFormat*)header;

    if (format->mode == pxB)
    {
        return write_binary(opt, header);
    }
    else
    {
        return write_num(opt, header);
    }
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

str write_int(StrPoolOptions opt, FmtHeader* header)
{
    IntFormat* fmt = (IntFormat*)header;
    int num = fmt->value;
    int places = fmt->places;

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
    char* text = pool_use(opt, memory_size(opt, textLen));

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

    textLen += finalize_str(opt, text, textLen);
    return (str){.chars = text, .len = textLen, POOL_DEFAULTS(opt)};
}

str write_num(StrPoolOptions opt, FmtHeader* header)
{
    NumFormat* fmt = (NumFormat*)header;

    int64_t num = fmt->value;
    bool isUnsigned = fmt->mode == pxU;
    int minPlaces = fmt->places;
    int paddingCount = 0;
    int specialCount = 0;

    bool negative = !isUnsigned && num & (1LU << 63);
    if (negative) specialCount++;

    uint64_t val = negative ? labs(num) : num;
    uint64_t cmp = 10;
    uint64_t lastCmp = 0;
    int digitCount = 1;

    while (val >= cmp && cmp > lastCmp)
    {
        digitCount++;
        lastCmp = cmp;
        cmp *= 10;
    }

    if (minPlaces > digitCount)
    {
        paddingCount = minPlaces - digitCount;
    }

    int textLen = digitCount + paddingCount + specialCount;
    char* text = pool_use(opt, memory_size(opt, textLen));

    char* cur = text;

    if (negative) *cur++ = '-';
    for (int i = 0; i < paddingCount; i++)
        *cur++ = '0';

    for (int i = 1; i <= digitCount; i++)
    {
        uint64_t cmp = pow(10, digitCount - i);
        int leader = val / cmp;
        val -= leader * cmp;

        *cur++ = leader + ASCII_NUM_OFFSET;
    }

    textLen += finalize_str(opt, text, textLen);
    return (str){.chars = text, .len = textLen, POOL_DEFAULTS(opt)};
}

/*
 * for small strings, padding hughe strings might be inefficient!
 */
str write_pad_left(StrPoolOptions opt, FmtHeader* header)
{
    StrFormat* fmt = (StrFormat*)header;
    str original = fmt->value;
    int n = fmt->pad_left_count;
    char c = fmt->pad_char;

    if (original.len >= n) return original;

    int padAmount = n - original.len;
    int memoryLen = n + 1;
    char* curC = pool_check_next(opt, memoryLen);

    for (int pad = 0; pad < padAmount; pad++)
    {
        *curC++ = c;
    }
    for (int i = 0; i < original.len; i++)
    {
        *curC++ = original.chars[i];
    }
    *curC = 0;

    char* newStr = pool_use(opt, memoryLen);
    return (str){
        .chars = newStr, .len = memoryLen - 1, .null_terminated = true};
}

str write_binary(StrPoolOptions opt, FmtHeader* header)
{
    NumFormat* fmt = (NumFormat*)header;
    int64_t num = fmt->value;

    // 1 char ber bit - 8 bits/byte
    int maxLen = sizeof(num) * 8;
    char* curC = pool_check_next(opt, memory_size(opt, maxLen));

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

    char* actualStr = pool_use(opt, actualLength);
    actualLength += finalize_str(opt, actualStr, actualLength);

    return (str){.chars = actualStr, .len = actualLength, POOL_DEFAULTS(opt)};
}
str write_float(StrPoolOptions opt, FmtHeader* header)
{
    FloatFormat* fmt = (FloatFormat*)header;
    double f = fmt->value;
    int decimals = fmt->decimals;

    str sign = f < 0 ? str_static("-") : (str){};
    double abs = fabs(f);
    int64_t high = abs;
    int64_t factor = pow(10, decimals);
    double decShifted = (abs - high) * factor;
    int64_t roundedNum = fabs(round(decShifted));

    if (decimals == 0)
    {
        // it's either 0 or 1 then
        // -> we might have a carry
        high += roundedNum;
        return str_formatc_opt(opt, "%%.", fmt_s(sign), fmt_n(high));
    }

    if (roundedNum == factor)
    {
        // when we have carry then after comma is always 0
        // -> In that case we only have a single used value
        high++;
        roundedNum -= factor;
    }

    str res = str_formatc_opt(opt, "%%.%", fmt_s(sign), fmt_n(high),
                              fmt_n(roundedNum, .places = decimals));
    return res;
}

str write_bool(StrPoolOptions opt, FmtHeader* header)
{
    BoolFormat* fmt = (BoolFormat*)header;

    str source = fmt->value ? str_static(TRUE_STR) : str_static(FALSE_STR);
    char* cur = pool_use(opt, memory_size(opt, source.len));
    memcpy(cur, source.chars, source.len);

    int actualLen = source.len + finalize_str(opt, cur, source.len);
    return (str){.chars = cur, .len = actualLen, POOL_DEFAULTS(opt)};
}

str write_str(StrPoolOptions opt, FmtHeader* header)
{
    StrFormat* fmt = (StrFormat*)header;

    str source = fmt->value;
    char* cur = pool_use(opt, memory_size(opt, source.len));
    memcpy(cur, source.chars, source.len);

    int actualLen = source.len + finalize_str(opt, cur, source.len);
    return (str){.chars = cur, .len = actualLen, POOL_DEFAULTS(opt)};
}

str write_char(StrPoolOptions opt, FmtHeader* header)
{
    CharFormat* fmt = (CharFormat*)header;
    int textLen = 1;
    char* text = pool_use(opt, memory_size(opt, textLen));
    text[0] = fmt->value;
    textLen += finalize_str(opt, text, textLen);

    return (str){.chars = text, .len = textLen, POOL_DEFAULTS(opt)};
}

str write_ptr(StrPoolOptions opt, FmtHeader* header)
{
    PtrFormat* fmt = (PtrFormat*)header;
    // 2 hex values for each byte
    int bytes = sizeof(void*);
    int printLen = bytes * 2;
    // '0x' prefix + null-term
    int size = printLen + 3;

    char* curC = pool_check_next(opt, size);

    uintptr_t ptrVal = (uintptr_t)fmt->value;

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

    char* actualStr = pool_use(opt, actualLen);

    return (str){
        .chars = actualStr, .len = actualLen - 1, .null_terminated = true};
}

/*
 * Example for a custom formatter
 */
str write_str_obj(StrPoolOptions opt, FmtHeader* header)
{
    StrFormat* fmt = (StrFormat*)header;
    str string = fmt->value;

    str dataPreview = str_sub(string, 0, STR_PREVIEW_LEN);
    str previewDots = {0};
    if (string.len > STR_PREVIEW_LEN)
    {
        previewDots = str_static("...");
    }

    str formatted = str_formatc_opt(
        opt, "str{ %: '%%', len: %, %, %, % }", fmt_p(string.chars),
        fmt_s(dataPreview), fmt_s(previewDots), fmt_n(string.len),
        fmt_b(string.null_terminated), fmt_b(string.is_slice),
        fmt_b(string.is_static));
    return formatted;
}
