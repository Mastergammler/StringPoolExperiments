#include "module.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_SIZE(n) (n + 1)
#define SENTINEL 0xf8

int clamp(int value, int min, int max);
int clamp_max(int value, int ceilingEx);
int clamp_min(int value, int floorInc);

str string_alloc(StringPool* pool, const char* cstr);
void string_append(StringPool* pool, str* src, str extension);

void debug_print_pool(StringPool pool, int stride);
void pool_init(StringPool* pool, uint64_t size);
void pool_reset(StringPool* pool);
void* pool_check_next(StringPool* pool, int maxSize);
void* pool_use(StringPool* pool, int size);
void print_ln(str string, FILE* stream);
void print_str(const char* cstr, FILE* stream);

str format_binary(StringPool* pool, int num);
str format_ptr(StringPool* pool, void* ptr);
str format_bool(StringPool* pool, bool boolean);
str format_int(StringPool* pool, int num);
str format_pad_left(StringPool* pool, str original, char c, int n);
str format_float(StringPool* pool, float f, int decimals);
str format_str(StringPool* pool, str string);
