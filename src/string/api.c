#include "internal.h"

inline void println(str string, ...)
{
    va_list args;
    va_start(args, string);
    print_ln(string, stdout, args);
}

inline void printstr(const char* cstr, ...)
{
    va_list args;
    va_start(args, cstr);
    print_str(cstr, stdout, args);
}

inline str str_alloc(const char* cstr)
{
    return string_alloc(&String_Mem.persistent, cstr);
}

inline void str_append(str* src, str extension)
{
    string_append(&String_Mem.persistent, src, extension);
}

// STFO: do i even want to expose these functions?
//  -> this should all just be handeled by format, no?
//  => This is a bit strange, or do i need it for my own type expansion?
//  => Probably? But then i would need the base ones, and not this ones
//  => Writing in to persisten memory is not that usefull here!
inline str fmt_bin(int num)
{
    return format_binary(&String_Mem.persistent, num);
}
inline str fmt_ptr(void* ptr)
{
    return format_ptr(&String_Mem.persistent, ptr);
}
inline str fmt_b(bool boolean)
{
    return format_bool(&String_Mem.persistent, boolean);
}
inline str fmt_i(int num)
{
    return format_int(&String_Mem.persistent, num);
}
inline str pad_left(str original, char c, int n)
{
    return format_pad_left(&String_Mem.persistent, original, c, n);
}
inline str fmt_f(float f, int decimals)
{
    return format_float(&String_Mem.persistent, f, decimals);
}
inline str fmt_str(str string)
{
    return format_str(&String_Mem.persistent, string);
}
