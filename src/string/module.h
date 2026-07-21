#ifndef MGLIBC_STRINGS
#define MGLIBC_STRINGS

#include "../alloc/module.h"
#include "types.h"

#ifndef FMT_PH
#define FMT_PH '%'
#define FMT_PH_LEN 1
#endif

#define define_pool(index, size, ...)                                          \
    str_pool_define(index, size, (StrPoolInitOptions){__VA_ARGS__})

// TASKLIST:
//  - write expander to hanlde hresults (maybe wrap into struct)
//  ✔ write a string join function
//  - join: ignore empty by default? -> join options
//  - write a print hex function (for ints / bytes maybe?)

#ifdef __cplusplus
extern "C"
{
#endif
    void str_pool_prepare(AllocContext ctx, StringMemory* memory,
                          uint8_t poolCount);
    void str_pool_finalize(AllocContext ctx);
    void str_pool_define(uint8_t index, uint64_t size, StrPoolInitOptions opt);
    void str_pool_reset(StrPoolOptions opt);
    void str_pool_reset_to(StrPoolOptions opt, uint64_t pos);

    str str_alloc(const char* cstr);
    str str_alloc_opt(StrPoolOptions opt, const char* cstr);
    str str_allocn(const char* cstr, int len);
    str str_allocn_opt(StrPoolOptions opt, const char* cstr, int len);
    str str_sub(str parent, int from, int toExclusive);
    str str_trim(str parent);
    StrSplitResult str_split_last(str src, char splitAt);
    str str_static(const char* cstr);
    str str_concat(str a, str b);
    str str_concat_opt(StrPoolOptions opt, str a, str b);
    str str_concatc(str a, const char* cstr);
    str str_join(str* strings, int count, str delim);
    str str_join_opt(StrPoolOptions opt, str* strings, int count, str delim);
    str str_joinc(str* strings, int count, const char* delim);
    str str_joinc_opt(StrPoolOptions opt, str* strings, int count,
                      const char* delim);
    void str_append(str* src, str extension);
    void str_append_opt(StrPoolOptions opt, str* src, str extension);
    void str_appendc(str* src, const char* extension);
    void str_print(str string, ...);
    void str_printc(const char* cstr, ...);

    str str_formatc(const char* formatter, ...);
    str str_format(str tmplStr, ...);
    str str_formatc_opt(StrPoolOptions opt, const char* formatter, ...);
    str str_format_opt(StrPoolOptions opt, str formatter, ...);

    str str_fmt_bin(int num);
    str str_fmt_ptr(void* ptr);
    str str_fmt_b(bool boolean);
    str str_fmt_i(int num, int places);
    str str_pad_left(str original, char c, int n);
    str str_fmt_f(float f, int decimals);
    str str_fmt_str(str string);

    bool str_equals(str a, str b);
    bool str_equals_opt(str a, str b, StrCompareOptions opt);
    bool str_ends_with(str a, str ending);
    bool str_ends_with_opt(str a, str ending, StrCompareOptions opt);
    bool str_starts_with(str a, str start);
    bool str_starts_with_opt(str a, str start, StrCompareOptions opt);

#ifdef __cplusplus
}
#endif
#endif // MGLIB_STRINGS
