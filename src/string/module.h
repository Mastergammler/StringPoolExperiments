#pragma once
#ifndef STRING_MODULE
#define STRING_MODULE

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void str_init(StringMemory* allocation, uint64_t poolSize,
                  uint64_t printBuffer, uint64_t frameBuffer,
                  uint64_t expandBufferSize);
    void str_pool_reset(StringPool* pool);

    str str_alloc(const char* cstr);
    str str_allocn(const char* cstr, int len);
    str str_sub(str parent, int from, int toExclusive);
    str str_static(const char* cstr);
    void str_append(str* src, str extension);
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
    str str_fmt_i(int num);
    str str_pad_left(str original, char c, int n);
    str str_fmt_f(float f, int decimals);
    str str_fmt_str(str string);

    bool str_equals(str a, str b);
    bool str_equals_opt(str a, str b, StrCompareOptions opt);

#ifdef __cplusplus
}
#endif

#endif
