#pragma once

#include "types.h"

extern StringMemory StrMemory;

void str_init(uint64_t poolSize, uint64_t printBuffer,
              uint64_t expandBufferSize);

str str_alloc(const char* cstr);
str str_sub(str parent, int from, int toExclusive);
str str_static(const char* cstr);
void str_append(str* src, str extension);
void str_appendc(str* src, const char* extension);
void str_print(str string, ...);
void str_printc(const char* cstr, ...);

str str_formatc(const char* formatter, ...);
str str_format(str tmplStr, ...);

str str_fmt_bin(int num);
str str_fmt_ptr(void* ptr);
str str_fmt_b(bool boolean);
str str_fmt_i(int num);
str str_pad_left(str original, char c, int n);
str str_fmt_f(float f, int decimals);
str str_fmt_str(str string);
