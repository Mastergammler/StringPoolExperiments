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
