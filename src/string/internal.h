#ifndef STRING_INTERNAL
#define STRING_INTERNAL

#include "module.h"
#include "types.h"

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_SIZE(n) (n + 1)
#define SENTINEL 0xf8

StringMemory* StrMemory;
static FmtArgBufferStore FmtArgBuffers;

static const char FMT_PH = '%';
static const int PLACEHOLDER_LEN = 1;

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

str format_binary(StringPool* pool, int num);
str format_ptr(StringPool* pool, void* ptr);
str format_bool(StringPool* pool, bool boolean);
str format_int(StringPool* pool, int num);
str format_pad_left(StringPool* pool, str original, char c, int n);
str format_float(StringPool* pool, float f, int decimals);
str format_str(StringPool* pool, str string);
str format_expand(str formatter, ...);
str format_cstr(const char* formatter, ...);

void print_ln(str string, FILE* stream, va_list args);
void print_str(const char* cstr, FILE* stream, va_list args);

str expand_int(StringPool* pool, FmtHeader* header);
str expand_float(StringPool* pool, FmtHeader* header);
str expand_str(StringPool* pool, FmtHeader* header);
str use_str(StringPool* pool, FmtHeader* header);

str expand_ptr(StringPool* pool, FmtHeader* header);
str expand_bool(StringPool* pool, FmtHeader* header);
str string_repeat(StringPool* pool, str src, int num);
str format_pool(StrPoolOptions opt, const char* formatter, ...);
str format_valist(StrPoolOptions opt, str formatter, va_list args);

#endif
int get_and_reserve_arg_buf();
void release_arg_buf(int index);
