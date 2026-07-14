#include "../internal.h"

bool cmp_from_ptr(const char* str1, const char* str2, uint32_t len,
                  bool caseInsensitive)
{
    for (int i = 0; i < len; i++)
    {
        char a = *str1++;
        char b = *str2++;

        if (caseInsensitive)
        {
            a = tolower(a);
            b = tolower(b);
        }

        if (a != b) return false;
    }

    return true;
}

bool str_equals_options(str a, str b, StrCompareOptions opt)
{
    if (a.len != b.len) return false;
    if (a.chars == b.chars) return true;

    return cmp_from_ptr(a.chars, b.chars, a.len, opt.case_insensitive);
}

bool str_starts_with_options(str cmp, str start, StrCompareOptions opt)
{
    if (start.len > cmp.len) return false;

    return cmp_from_ptr(cmp.chars, start.chars, start.len,
                        opt.case_insensitive);
}

bool str_ends_with_options(str cmp, str ending, StrCompareOptions opt)
{
    if (ending.len > cmp.len) return false;

    // just start at the last ending.len chars
    const char* cmpPos = cmp.chars + cmp.len - ending.len;
    return cmp_from_ptr(cmpPos, ending.chars, ending.len, opt.case_insensitive);
}
