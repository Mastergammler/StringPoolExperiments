#include "string/internal.h"
#include "string/macros.h"
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
        printstr(fmt_f(testNum, 9));
    }
    float ownTime = timer_elapsed_ms(&perf);

    printf("%i iterations\nPrf: %.3f ms\nOwn: %.3f ms\n", iterations,
           printfTime, ownTime);
}

void test_basic_formatting()
{
    Timer t = {};

    timer_start(&t);
    str hello = allocstr("Hello, world!");
    str second = allocstr("This is just another test string");
    str third = allocstr("This is the third one we have there");
    str staticAlloc =
        staticstr("The world is static, and static is the world!");
    float alloc = timer_elapsed_ms(&t);
    printstr(fmt_f(alloc, 6));

    str sub = substr(hello, -5, 5);
    str sub2 = substr(hello, 2000, 5);
    str sub3 = substr(hello, -10, -5);
    str sub4 = substr(hello, 20, 40);

    str printBin2 = fmt_bin(2715030);

    printstr(hello);
    printstr(sub);
    printstr(sub2);
    printstr(sub3);
    printstr(sub4);
    printstr(fmt_bin(0b11001));
    printstr(printBin2);
    printstr(fmt_bin(-12));
    printstr(fmt_ptr(&String_Mem.persistent));
    printstr(fmt_b(12));
    printstr(fmt_b(false));
    printstr(fmt_i(12));
    printstr(fmt_i(-17));
    printstr(fmt_i(134775));
    printstr(pad_left(staticstr("1"), '0', 5));

    timer_elapsed_ms(&t);
    appendstr(&hello, staticAlloc);
    // TODO: need a format list -> not just a expand
    appendstr(&second, printBin2);
    float append = timer_elapsed_ms(&t);
    printstr(fmt_f(append, 6));

    printstr(fmt_str(hello));
    float fmtString = timer_elapsed_ms(&t);
    printstr(fmt_f(fmtString, 6));
    printf("%.6f ms\n", fmtString);
    printstr(fmt_str(staticstr("Very Short!")));
}

void test2()
{
    Timer t;
    timer_start(&t);
    float initTime = timer_elapsed_ms(&t);
    print("Buffer Init: % ms", FLOAT(initTime, 3));
    float formatTime = timer_elapsed_ms(&t);
    printf("Buffer Init: %.3f ms\n", initTime);
    float pfFormatTime = timer_elapsed_ms(&t);
    print("Own f-format: % ms\nPrintf f-format: % ms", FLOAT(formatTime, 4),
          FLOAT(pfFormatTime, 4));

    timer_elapsed_ms(&t);
    str tmpl = allocstr("This is a static string template. % time we already "
                        "used it. \nObj: %");

    int iterations = 20;
    for (int i = 0; i < iterations; i++)
    {
        printstr(tmpl, INT(i), STR_OBJ(tmpl));
    }
    str formatted =
        formatstr(tmpl, INT(iterations), STR_OBJ(staticstr("Something new")));
    float aFewFormats = timer_elapsed_ms(&t);
    print("21 Formats: %", FLOAT(aFewFormats, 3));

    str a = staticstr("'First One'");
    str b = allocstr("'Second One'");
    print("%-%", STR(a), STR(b));
    print("-%-%", STR(a), STR(b));
    print("-%-%- %:%", STR(a), STR(b), INT(0b1101101), INT_BIN(0b1101101));
    float threeFormats = timer_elapsed_ms(&t);
    print("| % ms : % formats | % ms other formats", FLOAT(aFewFormats, 3),
          INT(iterations), FLOAT(threeFormats, 3));

    print("% % % % %", FLOAT(aFewFormats, 0), FLOAT(aFewFormats, 1),
          FLOAT(aFewFormats, 2), FLOAT(aFewFormats, 3), FLOAT(aFewFormats, 4));

    print("> ipos % - ineg % - ibin %", INT(5), INT(-7), INT_BIN(19));

    print("'%' - %", STR(a), STR_OBJ(a));
}

int main(int argc, char* argv[])
{
    Timer t = {};
    timer_start(&t);
    strings_init(1024 * 1, 256, 256);

    test2();

    str formatted =
        format("% - % - % - % - %", FLOAT(1.005, 5), FLOAT(1.005, 4),
               FLOAT(1.005, 3), FLOAT(0.07, 2), FLOAT(0.07, 1));

    timer_elapsed_ms(&t);
    debug_print_pool(String_Mem.persistent, 64);
    debug_print_pool(String_Mem.print_buffer, 64);
    debug_print_pool(String_Mem.transient, 64);
    float printPoolTime = timer_elapsed_ms(&t);
    float programTime = timer_ms_since_start(&t);

    print("| % ms print pool | % ms program runtime |", FLOAT(printPoolTime, 3),
          FLOAT(programTime, 3));

    return 0;
}
