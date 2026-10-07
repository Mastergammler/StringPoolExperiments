#include "internal.h"
#include "module.h"
#include "types.h"

/*
 * In order to use the posix api correctly, we need to
 * ensure the string we're passing is null terminated
 */
const char* str_cstr(str str)
{
    ASSERT(str.chars, "String is not valid: %p :: len %i", (void*)str.chars,
           str.len);

    if (str.null_terminated) return str.chars;
    StrPool defaultPool = StrMem->pools[StrMem->default_pool];

    ASSERT(!defaultPool.neglect_null_termination,
           "Default pool negelcts null termination and can therefore not be "
           "used to ensure null termination! If another pool is null "
           "terminated, use str_cstr_opt instead");

    return str_allocn(str.chars, str.len).chars;
}

const char* str_cstr_opt(StrPoolOptions opt, str str)
{
    if (str.null_terminated) return str.chars;
    StrPool pool = StrMem->pools[opt.pool_idx];

    ASSERT(!pool.neglect_null_termination,
           "Pool must not neglect null termination, because we are trying to "
           "ensure it! (Pool id was %i)",
           opt.pool_idx);

    return str_allocn_opt(opt, str.chars, str.len).chars;
}

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
    return string_alloc((StrPoolOptions){StrMem->default_pool}, cstr,
                        strlen(cstr));
}

str str_allocn(const char* cstr, int len)
{
    return string_alloc((StrPoolOptions){StrMem->default_pool}, cstr, len);
}

str str_alloc_opt(StrPoolOptions opt, const char* cstr)
{
    return string_alloc(opt, cstr, strlen(cstr));
}

str str_allocn_opt(StrPoolOptions opt, const char* cstr, int len)
{
    return string_alloc(opt, cstr, len);
}

void str_append(str* src, str extension)
{
    string_append((StrPoolOptions){StrMem->default_pool}, src, extension);
}

void str_append_opt(StrPoolOptions opt, str* src, str extension)
{
    string_append(opt, src, extension);
}

str str_concat(str a, str b)
{
    string_append((StrPoolOptions){StrMem->default_pool}, &a, b);
    return a;
}

str str_concat_opt(StrPoolOptions opt, str a, str b)
{
    string_append(opt, &a, b);
    return a;
}

str str_concatc(str a, const char* cstr)
{
    string_append((StrPoolOptions){StrMem->default_pool}, &a, str_static(cstr));
    return a;
}

StrSplitResult str_split_last(str src, char splitAt)
{
    return string_split_last(src, splitAt, (StrSplitOpt){});
}

str str_join(str* strings, int count, str delim)
{
    return string_join((StrPoolOptions){StrMem->default_pool}, strings, count,
                       delim);
}
str str_join_opt(StrPoolOptions opt, str* strings, int count, str delim)
{
    return string_join(opt, strings, count, delim);
}
str str_joinc(str* strings, int count, const char* delim)
{
    return string_join((StrPoolOptions){StrMem->default_pool}, strings, count,
                       str_static(delim));
}

str str_joinc_opt(StrPoolOptions opt, str* strings, int count,
                  const char* delim)
{
    return string_join(opt, strings, count, str_static(delim));
}

void str_appendc(str* src, const char* extension)
{
    string_append((StrPoolOptions){StrMem->default_pool}, src,
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
    IntFormat fmt = {.value = num, .binary = true};
    return write_binary((StrPoolOptions){StrMem->default_pool}, &fmt.header);
}
str str_fmt_ptr(void* ptr)
{
    return write_ptr((StrPoolOptions){StrMem->default_pool}, ptr);
}
str str_fmt_b(bool boolean)
{
    BoolFormat fmt = {.value = boolean};
    return write_bool((StrPoolOptions){StrMem->default_pool}, &fmt.header);
}

/*
 * pass 0 to keep the default places
 * else we'll pad zeros
 */
__attribute__((deprecated("use write_n instead"))) str str_fmt_i(int num,
                                                                 int places)
{
    IntFormat fmt = {.value = num, .places = places};
    return write_int((StrPoolOptions){StrMem->default_pool}, &fmt.header);
}
str str_pad_left(str original, char c, int n)
{
    StrFormat fmt = {.value = original, .pad_left_count = n, .pad_char = c};
    return write_pad_left((StrPoolOptions){StrMem->default_pool}, &fmt.header);
}
str str_fmt_f(float f, int decimals)
{
    FloatFormat fmt = {.value = f, .decimals = decimals};
    return write_float((StrPoolOptions){StrMem->default_pool}, &fmt.header);
}
str str_fmt_str(str string)
{
    StrFormat fmt = {.value = string};
    return write_str((StrPoolOptions){StrMem->default_pool}, &fmt.header);
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
