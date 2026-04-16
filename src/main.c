#include "string/internal.h"
#include "string/module.h"
#include "string/types.h"
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

void test_basic_formatting()
{
    Timer t = {};

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
    println(fmt_ptr(&String_Mem.persistent));
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
}

int main(int argc, char* argv[])
{
    Timer t = {};
    timer_start(&t);
    strings_init(1024 * 1, 256, 256);
    float initTime = timer_elapsed_ms(&t);
    printstr("Buffer Init: % ms", FLOAT(initTime, 3));
    float formatTime = timer_elapsed_ms(&t);
    printf("Buffer Init: %.3f ms\n", initTime);
    float pfFormatTime = timer_elapsed_ms(&t);
    printstr("Own f-format: % ms\nPrintf f-format: % ms", FLOAT(formatTime, 4),
             FLOAT(pfFormatTime, 4));

    timer_elapsed_ms(&t);
    str tmpl = str_alloc("This is a static string template. % time we already "
                         "used it. \nObj: %");

    int iterations = 20;
    for (int i = 0; i < iterations; i++)
    {
        println(tmpl, INT(i), STR_OBJ(tmpl));
    }
    str formatted = format_tmpl(tmpl, INT(iterations),
                                STR_OBJ(static_str("Something new")));
    float aFewFormats = timer_elapsed_ms(&t);
    printstr("21 Formats: %", FLOAT(aFewFormats, 3));

    str a = static_str("'First One'");
    str b = str_alloc("'Second One'");
    printstr("%-%", STR(a), STR(b));
    printstr("-%-%", STR(a), STR(b));
    printstr("-%-%- %:%", STR(a), STR(b), INT(0b1101101), INT_BIN(0b1101101));
    float threeFormats = timer_elapsed_ms(&t);
    printstr("| % ms : % formats | % ms other formats", FLOAT(aFewFormats, 3),
             INT(iterations), FLOAT(threeFormats, 3));

    timer_elapsed_ms(&t);
    debug_print_pool(String_Mem.persistent, 64);
    debug_print_pool(String_Mem.print_buffer, 64);
    debug_print_pool(String_Mem.transient, 64);
    float printPoolTime = timer_elapsed_ms(&t);
    float programTime = timer_ms_since_start(&t);

    printstr("| % ms print pool | % ms program runtime |",
             FLOAT(printPoolTime, 3), FLOAT(programTime, 3));

    return 0;
}
