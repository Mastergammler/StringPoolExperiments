#pragma once

#include "types.h"

extern StringMemory String_Mem;

void strings_init(uint64_t poolSize, uint64_t bufferSize);

void println(str string);
void printstr(const char* cstr);

str str_alloc(const char* cstr);
void str_append(str* src, str extension);
str static_str(const char* cstr);
str substr(str parent, int from, int toExclusive);

inline str fmt_bin(int num);
inline str fmt_ptr(void* ptr);
inline str fmt_b(bool boolean);
inline str fmt_i(int num);
inline str pad_left(str original, char c, int n);
inline str fmt_f(float f, int decimals);
inline str fmt_str(str string);
