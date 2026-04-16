#pragma once

#include "types.h"

extern StringMemory String_Mem;

void strings_init(uint64_t poolSize, uint64_t printBuffer,
                  uint64_t expandBufferSize);

void printstr(str string, ...);
void print(const char* cstr, ...);
str substr(str parent, int from, int toExclusive);
str staticstr(const char* cstr);
void appendstr(str* src, str extension);
str allocstr(const char* cstr);

str format(const char* formatter, ...);
str formatstr(str tmplStr, ...);

str fmt_bin(int num);
str fmt_ptr(void* ptr);
str fmt_b(bool boolean);
str fmt_i(int num);
str pad_left(str original, char c, int n);
str fmt_f(float f, int decimals);
str fmt_str(str string);
