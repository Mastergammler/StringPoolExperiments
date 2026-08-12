#include "../internal.h"
#include "../macros.h"

/**
 * NOTE: this 'descriptor' approach works, but makes the data-flow
 * unnecessarly complex. It is way easier, to just print into the final buffer
 * on demand, without the expansion of sub fields within it,
 * and that removes the need for the transient pool alltogether
 */

/*PrintHeader* num_formatter(FmtHeader* header)
{
    NumFormat* fmt = (NumFormat*)header;
    if (fmt->binary)
    {
        ASSERT(false, "Not implemented");
    }
    else
    {
        fmt->desc = extract_signed(fmt->value, fmt->places);
        return (PrintHeader*)&fmt->desc;
    }
}
*/

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

// NOTE: we're creating kind a  sub-string here
// that is why we are not writing directly into a pool
// -> But just using the currently available cursor
//
// STFO: not sure if this is good design
// Because we need to know the length beforehand now
// Can i not just let it print directly into the pool?
// Shouldn't this be possible? Then i also don't need to do
// the whole cursor acrobatics there
str print_signed(char* writeCursor, PrintHeader* header)
{
    NumDesc* fmt = (NumDesc*)header;

    char* cur = writeCursor;

    if (fmt->negative)
    {
        *cur++ = '-';
    }

    for (int i = 0; i < fmt->pad_left_count; i++)
    {
        *cur++ = fmt->pad_char;
    }

    //  saved with the highest number value at the lowest index
    for (int i = 0; i < fmt->digit_count; i++)
    {
        int digit = i;
        int arrIdx = digit / 2;
        int shift = digit % 2;

        int leader = fmt->digits[arrIdx] >> (shift * 4) & 0b1111;
        // printf("[out] Leader %i, index %i, shift %i\n", leader, arrIdx,
        // shift);
        *cur++ = leader + ASCII_NUM_OFFSET;
    }

    for (int i = 0; i < fmt->pad_right_count; i++)
    {
        *cur++ = fmt->pad_char;
    }

    int textLen = fmt->header.str_len;

    return (str){
        .chars = writeCursor, .len = textLen, .null_terminated = false};
}
