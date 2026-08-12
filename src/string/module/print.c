#include "../internal.h"
#include "../macros.h"

// STFO: needs explicit fflush(stdout) ?
// - handle fulljy buffered / piped mode
void print_ln(str tmpl, FILE* stream, va_list args)
{
    // writing into the print buffer, which will be reused
    // on 'overflow' it will just overwrite previous strings
    // For async handling, the buffer needs to be large enough,
    // that new strings don't override previous ones and lead to garbage
    // print output
    StrPoolOptions opt = {StrMem->print_pool};
    str string = format_valist(opt, tmpl, args);
    if (string.len == 0)
    {
        fputs("<empty>\n", stream);
    }
    else
    {
        fwrite(string.chars, 1, string.len, stream);
    }
}

void print_str(const char* cstr, FILE* stream, va_list args)
{
    print_ln(str_static(cstr), stream, args);
}
