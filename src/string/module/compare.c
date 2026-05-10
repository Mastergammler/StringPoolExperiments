#include "../internal.h"

bool str_equals_options(str a, str b, StrCompareOptions opt)
{
    if (a.len != b.len) return false;
    if (a.chars == b.chars) return true;

    if (opt.case_insensitive)
    {
        for (int i = 0; i < a.len; i++)
        {
            char lowerA = tolower(a.chars[i]);
            char lowerB = tolower(b.chars[i]);

            if (lowerA != lowerB) return false;
        }

        return true;
    }
    else
    {
        return strncmp(a.chars, b.chars, a.len) == 0;
    }
}

bool str_ends_with_options(str cmp, str ending, StrCompareOptions opt)
{
    if (ending.len > cmp.len) return false;

    for (int i = 1; i <= ending.len; i++)
    {
        char a = cmp.chars[cmp.len - i];
        char b = ending.chars[ending.len - i];

        if (opt.case_insensitive)
        {
            a = tolower(a);
            b = tolower(b);
        }

        if (a != b) return false;
    }

    return true;
}
