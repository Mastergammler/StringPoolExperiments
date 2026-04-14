#include "string/internal.h"
#include "string/module.h"
#include "timing/module.h"

#include <stdio.h>

StringMemory String_Mem;

void test_simple_perf(int iterations)
{
    Timer perf = {};
    timer_start(&perf);

    float testNum = 198.773049;

    timer_elapsed_ms(&perf);
    for (int i = 0; i < iterations; i++)
    {
        printf("%.9f\n", testNum);
    }
    float printfTime = timer_elapsed_ms(&perf);
    for (int i = 0; i < iterations; i++)
    {
        println(fmt_f(testNum, 9));
    }
    float ownTime = timer_elapsed_ms(&perf);

    printf("%i iterations\nPrf: %.3f ms\nOwn: %.3f ms\n", iterations,
           printfTime, ownTime);
}

int main(int argc, char* argv[])
{
    Timer t = {};

    strings_init(1024 * 1, 256);

    timer_start(&t);
    str hello = str_alloc("Hello, world!");
    str second = str_alloc("This is just another test string");
    str third = str_alloc("This is the third one we have there");
    str staticAlloc =
        static_str("The world is static, and static is the world!");
    float alloc = timer_elapsed_ms(&t);
    println(fmt_f(alloc, 6));

    str sub = substr(hello, -5, 5);
    str sub2 = substr(hello, 2000, 5);
    str sub3 = substr(hello, -10, -5);
    str sub4 = substr(hello, 20, 40);

    str printBin2 = fmt_bin(2715030);

    println(hello);
    println(sub);
    println(sub2);
    println(sub3);
    println(sub4);
    println(fmt_bin(0b11001));
    println(printBin2);
    println(fmt_bin(-12));
    println(fmt_ptr(&String_Mem.pool));
    println(fmt_b(12));
    println(fmt_b(false));
    println(fmt_i(12));
    println(fmt_i(-17));
    println(fmt_i(134775));
    println(pad_left(static_str("1"), '0', 5));

    timer_elapsed_ms(&t);
    str_append(&hello, staticAlloc);
    // TODO: need a format list -> not just a expand
    str_append(&second, printBin2);
    float append = timer_elapsed_ms(&t);
    println(fmt_f(append, 6));

    println(fmt_str(hello));
    float fmtString = timer_elapsed_ms(&t);
    println(fmt_f(fmtString, 6));
    printf("%.6f ms\n", fmtString);
    println(fmt_str(static_str("Very Short!")));

    timer_elapsed_ms(&t);
    debug_print_pool(String_Mem.pool, 64);
    debug_print_pool(String_Mem.trans_pool, 64);

    float printPoolTime = timer_elapsed_ms(&t);
    float programTime = timer_ms_since_start(&t);
    println(fmt_f(printPoolTime, 3));
    println(fmt_f(programTime, 3));
    printf("%.3f ms\n", programTime);

    // test_simple_perf(1000);

    return 0;
}
