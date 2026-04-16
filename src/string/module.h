#pragma once

#include "types.h"

extern StringMemory String_Mem;

void strings_init(uint64_t poolSize, uint64_t printBuffer,
                  uint64_t expandBufferSize);

void println(str string, ...);
void printstr(const char* cstr, ...);

str str_alloc(const char* cstr);
void str_append(str* src, str extension);
str static_str(const char* cstr);
str substr(str parent, int from, int toExclusive);

str fmt_bin(int num);
str fmt_ptr(void* ptr);
str fmt_b(bool boolean);
str fmt_i(int num);
str pad_left(str original, char c, int n);
str fmt_f(float f, int decimals);
str fmt_str(str string);

str format(const char* formatter, ...);
str format_tmpl(str tmplStr, ...);
