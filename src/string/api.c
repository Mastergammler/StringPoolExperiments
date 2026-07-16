#include "internal.h"
#include "module.h"
#include "types.h"

void str_print(str string, ...)
{
    va_list args;
    va_start(args, string);
    print_ln(string, stdout, args);
}

void str_printc(const char* cstr, ...)
{
    va_list args;
    va_start(args, cstr);
    print_str(cstr, stdout, args);
}

str str_alloc(const char* cstr)
{
    return string_alloc(POOL(StrMem->default_pool), cstr, strlen(cstr));
}

str str_allocn(const char* cstr, int len)
{
    return string_alloc(POOL(StrMem->default_pool), cstr, len);
}

str str_alloc_opt(StrPoolOptions opt, const char* cstr)
{
    return string_alloc(POOL(opt.pool_idx), cstr, strlen(cstr));
}

str str_allocn_opt(StrPoolOptions opt, const char* cstr, int len)
{
    return string_alloc(POOL(opt.pool_idx), cstr, len);
}

void str_append(str* src, str extension)
{
    string_append(&StrMem->pools[StrMem->default_pool], src, extension);
}

void str_append_opt(StrPoolOptions opt, str* src, str extension)
{
    string_append(POOL(opt.pool_idx), src, extension);
}

str str_concat(str a, str b)
{
    string_append(POOL(StrMem->default_pool), &a, b);
    return a;
}

str str_concat_opt(StrPoolOptions opt, str a, str b)
{
    string_append(POOL(opt.pool_idx), &a, b);
    return a;
}

str str_concatc(str a, const char* cstr)
{
    string_append(&StrMem->pools[StrMem->default_pool], &a, str_static(cstr));
    return a;
}

StrSplitResult str_split_last(str src, char splitAt)
{
    return string_split_last(src, splitAt, (StrSplitOpt){});
}

str str_join(str* strings, int count, str delim)
{
    return string_join(&StrMem->pools[StrMem->default_pool], strings, count,
                       delim);
}
str str_join_opt(StrPoolOptions opt, str* strings, int count, str delim)
{
    return string_join(POOL(opt.pool_idx), strings, count, delim);
}
str str_joinc(str* strings, int count, const char* delim)
{
    return string_join(&StrMem->pools[StrMem->default_pool], strings, count,
                       str_static(delim));
}

str str_joinc_opt(StrPoolOptions opt, str* strings, int count,
                  const char* delim)
{
    return string_join(POOL(opt.pool_idx), strings, count, str_static(delim));
}

void str_appendc(str* src, const char* extension)
{
    string_append(&StrMem->pools[StrMem->default_pool], src,
                  str_static(extension));
}

// TODO: it would be good if i could validate that the char is static
//  -> like if i could determine the cdata section of the program,
//  and check if the pointer is within bounds
str str_static(const char* cstr)
{
    // STFO: should i assert here?
    //  -> i dunno, it seems a bit strange, and not very usefull?
    // assert(cstr);

    if (!cstr) return (str){};

    return (str){.chars = cstr,
                 .len = strlen(cstr),
                 .null_terminated = true,
                 .is_slice = false,
                 .is_static = true};
}

// STFO: do i even want to expose these functions?
//  -> this should all just be handeled by format, no?
//  => This is a bit strange, or do i need it for my own type expansion?
//  => Probably? But then i would need the base ones, and not this ones
//  => Writing in to persisten memory is not that usefull here!
str str_fmt_bin(int num)
{
    return format_binary(&StrMem->pools[StrMem->default_pool], num);
}
str str_fmt_ptr(void* ptr)
{
    return format_ptr(&StrMem->pools[StrMem->default_pool], ptr);
}
str str_fmt_b(bool boolean)
{
    return format_bool(&StrMem->pools[StrMem->default_pool], boolean);
}

/*
 * pass 0 to keep the default places
 * else we'll pad zeros
 */
str str_fmt_i(int num, int places)
{
    return format_int(&StrMem->pools[StrMem->default_pool], num, places);
}
str str_pad_left(str original, char c, int n)
{
    return format_pad_left(&StrMem->pools[StrMem->default_pool], original, c,
                           n);
}
str str_fmt_f(float f, int decimals)
{
    return format_float(&StrMem->pools[StrMem->default_pool], f, decimals);
}
str str_fmt_str(str string)
{
    return format_str(&StrMem->pools[StrMem->default_pool], string);
}

bool str_equals(str a, str b)
{
    return str_equals_options(a, b, (StrCompareOptions){false});
}

bool str_equals_opt(str a, str b, StrCompareOptions opt)
{
    return str_equals_options(a, b, opt);
}

bool str_ends_with(str a, str ending)
{
    return str_ends_with_options(a, ending, (StrCompareOptions){false});
}

bool str_ends_with_opt(str a, str ending, StrCompareOptions opt)
{
    return str_ends_with_options(a, ending, opt);
}

bool str_starts_with(str a, str start)
{
    return str_starts_with_options(a, start, (StrCompareOptions){false});
}

bool str_starts_with_opt(str a, str start, StrCompareOptions opt)
{
    return str_starts_with_options(a, start, opt);
}
