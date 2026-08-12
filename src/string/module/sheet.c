#include "../internal.h"
#include "../macros.h"

// TODO: Pad right not implemented yet
// -> Is it needed?
// TODO: implement unsigned handling
// -> It should almost be already supported just need a flag
// => And the rest should work basically the same, maybe some boundary issues
//    for pow -> would need to check for a max there
// NOTE: for binary printing, we only need to determine the
// amount of digits
NumDesc extract_signed(int64_t num, int minDigits)
{
    NumDesc desc = {print_signed};
    desc.pad_char = '0';
    desc.negative = num & (1LU << 63);

    int64_t val = labs(num);
    int64_t cmp = 10;

    // we always print at least one ('0' if it's 0)
    desc.digit_count = 1;

    // OPTIMIZE: can we unify the check-length &
    //  get number alg, so that we only need a single iteration?
    while (val >= cmp && cmp > 0)
    {
        desc.digit_count++;
        cmp *= 10;
    }

    // biggest (most left num) to smallest
    for (int i = 1; i <= desc.digit_count; i++)
    {
        int64_t cmp = pow(10, desc.digit_count - i);
        int leader = val / cmp;
        val -= leader * cmp;

        int digit = i - 1;
        int arrIdx = digit / 2;
        int shift = digit % 2;

        // printf("[in] Leader %i, index %i, shift %i\n", leader, arrIdx,
        // shift);
        desc.digits[arrIdx] |= leader << (shift * 4);
    }

    if (minDigits > desc.digit_count)
    {
        desc.pad_left_count = minDigits - desc.digit_count;
    }

    desc.header.str_len = desc.digit_count + (desc.negative ? 1 : 0) +
                          desc.pad_left_count + desc.pad_right_count;

    return desc;
}

BoolDesc extract_bool(bool val)
{
    // TODO: print fn = just print str?
    BoolDesc desc = {print_static};

    desc.static_name = val ? str_static(TRUE_STR) : str_static(FALSE_STR);
    desc.header.str_len = desc.static_name.len;

    return desc;
}
