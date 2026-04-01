#include "types.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static access to the string pools
// for easy formatter access
static StringPooling Strings;

void pool_init(StringPool* pool, int size);
str str_alloc(StringPool* pool, const char* cstr);
str from_cstr(const char* cstr);
int clamp_min(int value, int floorInc);
int clamp_max(int value, int ceilingEx);
int clamp(int value, int min, int max);
void println(str string);
void debug_print_pool(StringPool pool, int stride);
str substr(str parent, int from, int toExclusive);
void append(StringPool* pool, str* src, str extension);
void* pool_use(StringPool* pool, int size);
str fmt_b(StringPool* pool, int num);
void* pool_check_next(StringPool* pool, int maxSize);
str fmt_p(void* ptr);
str fmt_bool(bool boolean);
str fmt_str(str string);
str fmt_i(int num);
