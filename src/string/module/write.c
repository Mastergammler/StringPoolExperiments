#include "../internal.h"
#include "../macros.h"

str write_num_strategy(StrPoolOptions opt, FmtHeader* header)
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

[[deprecated("use write_num instead")]]
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
    int minLen = fmt->pad_left_count;
    char c = fmt->pad_char;

    if (original.len >= minLen) return original;

    int padLen = minLen - original.len;
    int strLen = minLen;

    char* text = pool_use(opt, memory_size(opt, minLen));
    char* curC = text;

    for (int pad = 0; pad < padLen; pad++)
    {
        *curC++ = c;
    }

    for (int i = 0; i < original.len; i++)
    {
        *curC++ = original.chars[i];
    }

    strLen += finalize_str(opt, text, strLen);
    return (str){.chars = text, .len = strLen, POOL_DEFAULTS(opt)};
}

str write_binary(StrPoolOptions opt, FmtHeader* header)
{
    NumFormat* fmt = (NumFormat*)header;
    int64_t num = fmt->value;

    // 1 char ber bit - 8 bits/byte
    int maxLen = sizeof(num) * 8;
    int actualLen = 1; // we always want to print '0'

    for (int i = maxLen - 1; i >= 0; i--)
    {
        bool bitFound = num & (0x1L << i);
        if (bitFound)
        {
            actualLen = i + 1;
            break;
        }
    }

    char* text = pool_use(opt, memory_size(opt, actualLen));
    char* cur = text;

    // print from left to right
    for (int i = actualLen - 1; i >= 0; i--)
    {
        bool bitActive = num & (0x1L << i);
        *cur++ = bitActive ? '1' : '0';
    }

    actualLen += finalize_str(opt, text, actualLen);
    return (str){.chars = text, .len = actualLen, POOL_DEFAULTS(opt)};
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
    uintptr_t ptrVal = (uintptr_t)fmt->value;
    int bytes = sizeof(void*);

    int byteLen = 1;
    for (int i = bytes - 1; i >= 0; i--)
    {
        char hiVal = (ptrVal >> (i * 8 + 4)) & 0x0F;
        char loVal = (ptrVal >> (i * 8)) & 0x0F;

        if (hiVal || loVal)
        {
            byteLen = i + 1;
            break;
        }
    }

    // 2 hex digits per byte + '0x' prefix
    int strLen = byteLen * 2 + 2;
    char* text = pool_use(opt, memory_size(opt, memory_size(opt, strLen)));
    char* cur = text;

    *cur++ = '0';
    *cur++ = 'x';

    for (int i = byteLen - 1; i >= 0; i--)
    {
        uint8_t hiVal = (ptrVal >> (i * 8 + 4)) & 0x0F;
        uint8_t loVal = (ptrVal >> (i * 8)) & 0x0F;
        *cur++ = HEX_DIGITS[hiVal];
        *cur++ = HEX_DIGITS[loVal];
    }

    strLen += finalize_str(opt, text, strLen);
    return (str){.chars = text, .len = strLen, POOL_DEFAULTS(opt)};
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
