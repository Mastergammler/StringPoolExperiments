#include "internal.h"

typedef size_t u64;

int main(int argc, char* argv[])
{
    StringPool* mem = &Strings.pool;
    pool_init(mem, 1024 * 1);

    str hello = str_alloc(mem, "Hello, world!");
    str second = str_alloc(mem, "This is just another test string");
    str third = str_alloc(mem, "This is the third one we have there");
    str staticAlloc =
        from_cstr("The world is static, and static is the world!");

    str sub = substr(hello, -5, 5);
    str sub2 = substr(hello, 2000, 5);
    str sub3 = substr(hello, -10, -5);
    str sub4 = substr(hello, 20, 40);

    str printBin2 = fmt_b(mem, 2715030);

    println(hello);
    println(sub);
    println(sub2);
    println(sub3);
    println(sub4);
    println(fmt_b(mem, 0b11001));
    println(printBin2);
    println(fmt_b(mem, -12));
    println(fmt_p(&Strings.pool));
    println(fmt_bool(12));
    println(fmt_bool(false));
    println(fmt_i(12));
    println(fmt_i(-17));
    println(fmt_i(134775));

    append(mem, &hello, staticAlloc);

    // TODO: need a format list -> not just a expand
    append(mem, &second, printBin2);

    println(fmt_str(hello));

    debug_print_pool(Strings.pool, 64);
    return 0;
}
