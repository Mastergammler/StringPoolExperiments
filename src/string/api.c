#include "internal.h"
#include "module.h"

inline void str_print(str string, ...)
{
    va_list args;
    va_start(args, string);
    print_ln(string, stdout, args);
}

inline void str_printc(const char* cstr, ...)
{
    va_list args;
    va_start(args, cstr);
    print_str(cstr, stdout, args);
}

inline str str_alloc(const char* cstr)
{
    return string_alloc(&StrMemory->persistent, cstr);
}

inline void str_append(str* src, str extension)
{
    string_append(&StrMemory->persistent, src, extension);
}

inline void str_appendc(str* src, const char* extension)
{
    string_append(&StrMemory->persistent, src, str_static(extension));
}

// TODO: it would be good if i could validate that the char is static
//  -> like if i could determine the cdata section of the program,
//  and check if the pointer is within bounds
str str_static(const char* cstr)
{
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
inline str str_fmt_bin(int num)
{
    return format_binary(&StrMemory->persistent, num);
}
inline str str_fmt_ptr(void* ptr)
{
    return format_ptr(&StrMemory->persistent, ptr);
}
inline str str_fmt_b(bool boolean)
{
    return format_bool(&StrMemory->persistent, boolean);
}
inline str str_fmt_i(int num)
{
    return format_int(&StrMemory->persistent, num);
}
inline str str_pad_left(str original, char c, int n)
{
    return format_pad_left(&StrMemory->persistent, original, c, n);
}
inline str str_fmt_f(float f, int decimals)
{
    return format_float(&StrMemory->persistent, f, decimals);
}
inline str str_fmt_str(str string)
{
    return format_str(&StrMemory->persistent, string);
}
