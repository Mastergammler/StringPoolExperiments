#include "../internal.h"

// STFO: what should the parsing rules be?
//  - kind of like just return 0 on invalid?
//  - should i have a parse result here? I think that makes sense
//  - do I just ignore invalid values, or parse up to a point?
//  - I could integrate something like 10-000-000 still getting parsed
//  -> Support things like 10_000_000, which would be cool
//  -> Or even like 10.000.000, because it would not impact parsing anyway?
//  => I mean it's very fleixble ten like my10muber20 would be parsed to
//     1020 -> i mean i could just track this in the result object?
//     => I guess that would be fine
//

/**
 * DECISION: Parsing rules
 * - non number chars are just ignored, number seen as one
 * - on non ' ', '.' , ',' chars, the number will be flagged as dirty
 * - parsing will always succeed
 */
ParseNRes str_parse_number(str str, bool unsignd)
{
    ParseNRes res = {};
    if (str.len == 0)
    {
        res.was_empty = true;
        return res;
    }

    uint64_t NUM_MAX = unsignd ? UINT64_MAX : INT64_MAX;

    bool isNegative = false;
    const char* cur = str.chars + str.len - 1;

    uint64_t factor = 1;
    uint64_t num = 0;
    uint64_t prev = 0;
    for (int i = 0; i < str.len; i++)
    {
        char c = *cur--;

        if (c == '-' && !unsignd)
        {
            isNegative = true;
            continue;
        }
        if (c == '.' || c == ',' || c == ' ' || c == '_')
        {
            // do nothing
            continue;
        }
        else if (c < '0' || c > '9')
        {
            res.dirty = true;
            continue;
        }

        // we just need to check if it's a negative number still
        if (res.overflow) continue;

        // every valid number resets the flag
        isNegative = false;
        int digitVal = c - ASCII_NUM_OFFSET;

        uint64_t add = factor * digitVal;
        if (add < num && digitVal > 0) res.overflow = true;

        num += add;
        if (!unsignd && num > INT64_MAX) res.overflow = true;
        if (num < prev) res.overflow = true;

        factor *= 10;
        prev = num;
    }

    if (isNegative)
        res.val = res.overflow ? INT64_MIN : ~num + 1;
    else
        res.val = res.overflow ? NUM_MAX : num;

    return res;
}

ParseNRes str_parse_n(str str)
{
    return str_parse_number(str, false);
}

ParseURes str_parse_u(str str)
{
    // we just cast this, because we merely change the interpretation
    // of the value, the bytes stay the same
    ParseNRes res = str_parse_number(str, true);
    return *(ParseURes*)&res;
}

// STFO: So the parsing rules for float would need to be more strict
// -> because of the . or , having more semantic meaning
// => That makes it a bit more complicated, or at least restrictive
double str_parse_f(str str)
{
    // TODO: IMPLEMENT

    // NOTE: i thought i could use the other two implementations at first,
    //  but then decided against it, since the precision would be very different
    //  then, even though it would probably be fine for most cases, it should
    //  be done properly for the purpose of this library

    return 0;
}
