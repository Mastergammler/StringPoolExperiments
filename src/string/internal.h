#ifndef STRING_INTERNAL
#define STRING_INTERNAL

#include "../alloc/module.h"
#include "module.h"
#include "types.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SENTINEL 0xf8
#define ASCII_SPACE 32
#define INT_SIGN (1U << 31)
#define ASCII_NUM_OFFSET 48

#define FMT_STR_SIZE 14
#define FMT_F_SIZE 3
#define STR_PREVIEW_LEN 13

#define AVERAGE_PH_LEN 8

static const char TRUE_STR[] = "true";
static const char FALSE_STR[] = "false";
static const char HEX_DIGITS[] = "0123456789abcdef";

/*
 * NOTE: INTERNAL ONLY -> Dependency on StrMem which is TU dependant
 * -> Because macro code will printed on caller site
 */
#define POOL(index)                                                            \
    ({                                                                         \
        ASSERT(index < StrMem->pool_count, "Index out of range: %i/%i", index, \
               StrMem->pool_count);                                            \
        &StrMem->pools[index];                                                 \
    })

#define POOL_DEFAULTS(opt)                                                     \
    .is_slice = opt.substring_mode,                                            \
    .null_terminated =                                                         \
        !opt.substring_mode && !POOL(opt.pool_idx)->neglect_null_termination

#ifdef __cplusplus
extern "C"
{
#endif

    // STFO: Keeep this or is this akward?
    // -> I guess it's fine, it's just internal access
    // => It's just the pointer there
    extern StringMemory* StrMem;
    static FmtArgBufferStore FmtArgBuffers;

    int clamp(int value, int min, int max);
    int clamp_max(int value, int ceilingEx);
    int clamp_min(int value, int floorInc);

    StrSplitResult string_split_last(str src, char splitAt, StrSplitOpt opt);

    str string_alloc(StrPoolOptions opt, const char* cstr, int len);
    str string_join(StrPoolOptions opt, str* strings, int count, str delim);
    void string_append(StrPoolOptions opt, str* src, str extension);
    str string_repeat(StrPoolOptions opt, str src, int num);

    void debug_print_pool(StringPool pool, int stride);
    void pool_init(StringPool* pool, uint64_t size);
    void* pool_use(StrPoolOptions opt, uint64_t size);
    bool pool_fits_till_end(StrPoolOptions opt, uint64_t size);

    str format_expand(str formatter, ...);
    str format_cstr(const char* formatter, ...);

    void print_ln(str string, FILE* stream, va_list args);
    void print_str(const char* cstr, FILE* stream, va_list args);

    str write_int(StrPoolOptions opt, FmtHeader* header);
    str write_num(StrPoolOptions opt, FmtHeader* header);
    str write_int_strategy(StrPoolOptions opt, FmtHeader* header);
    str write_float(StrPoolOptions opt, FmtHeader* header);
    str write_str_obj(StrPoolOptions opt, FmtHeader* header);
    str write_str(StrPoolOptions opt, FmtHeader* header);
    str write_char(StrPoolOptions opt, FmtHeader* header);
    str write_ptr(StrPoolOptions opt, FmtHeader* header);
    str write_bool(StrPoolOptions opt, FmtHeader* header);
    str write_binary(StrPoolOptions opt, FmtHeader* header);
    str write_pad_left(StrPoolOptions opt, FmtHeader* header);

    str format_valist(StrPoolOptions opt, str formatter, va_list args);

    int get_and_reserve_arg_buf();
    void release_arg_buf(int index);
    bool str_equals_options(str a, str b, StrCompareOptions opt);
    bool str_ends_with_options(str cmp, str ending, StrCompareOptions opt);
    bool str_starts_with_options(str cmp, str start, StrCompareOptions opt);

    uint64_t memory_size(StrPoolOptions opt, uint64_t charCount);
    int finalize_str(StrPoolOptions opt, char* strStart, uint64_t charCount);

    // TODO: NOT USED AFTER ALL
    NumDesc extract_signed(int64_t num, int minDigits);
    str print_signed(char* writeCursor, PrintHeader* header);

#ifdef __cplusplus
}
#endif

#endif
