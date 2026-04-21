#include "string/internal.h"
#include "string/macros.h"
#include "string/module.h"
#include "string/types.h"
#include "timing/module.h"

#include <stdio.h>

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
        str_print(str_fmt_f(testNum, 9));
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
        str_static("The world is static, and static is the world!");
    float alloc = timer_elapsed_ms(&t);
    str_print(str_fmt_f(alloc, 6));

    str sub = str_sub(hello, -5, 5);
    str sub2 = str_sub(hello, 2000, 5);
    str sub3 = str_sub(hello, -10, -5);
    str sub4 = str_sub(hello, 20, 40);

    str printBin2 = str_fmt_bin(2715030);

    str_print(hello);
    str_print(sub);
    str_print(sub2);
    str_print(sub3);
    str_print(sub4);
    str_print(str_fmt_bin(0b11001));
    str_print(printBin2);
    str_print(str_fmt_bin(-12));
    str_print(str_fmt_ptr(&StrMemory->persistent));
    str_print(str_fmt_b(12));
    str_print(str_fmt_b(false));
    str_print(str_fmt_i(12));
    str_print(str_fmt_i(-17));
    str_print(str_fmt_i(134775));
    str_print(str_pad_left(str_static("1"), '0', 5));

    timer_elapsed_ms(&t);
    str_append(&hello, staticAlloc);
    // TODO: need a format list -> not just a expand
    str_append(&second, printBin2);
    float append = timer_elapsed_ms(&t);
    str_print(str_fmt_f(append, 6));

    str_print(str_fmt_str(hello));
    float fmtString = timer_elapsed_ms(&t);
    str_print(str_fmt_f(fmtString, 6));
    printf("%.6f ms\n", fmtString);
    str_print(str_fmt_str(str_static("Very Short!")));
}

void test2()
{
    Timer t;
    timer_start(&t);
    float initTime = timer_elapsed_ms(&t);
    str_printc("Buffer Init: % ms", FLOAT(initTime, 3));
    float formatTime = timer_elapsed_ms(&t);
    printf("Buffer Init: %.3f ms\n", initTime);
    float pfFormatTime = timer_elapsed_ms(&t);
    str_printc("Own f-format: % ms\nPrintf f-format: % ms",
               FLOAT(formatTime, 4), FLOAT(pfFormatTime, 4));

    timer_elapsed_ms(&t);
    str tmpl = str_alloc("This is a static string template. % time we already "
                         "used it. \nObj: %");

    int iterations = 20;
    for (int i = 0; i < iterations; i++)
    {
        str_print(tmpl, NUM(i), STR_OBJ(tmpl));
    }
    str formatted =
        str_format(tmpl, NUM(iterations), STR_OBJ(str_static("Something new")));
    float aFewFormats = timer_elapsed_ms(&t);
    str_printc("21 Formats: %", FLOAT(aFewFormats, 3));

    str a = str_static("'First One'");
    str b = str_alloc("'Second One'");
    str_printc("%-%", STR(a), STR(b));
    str_printc("-%-%", STR(a), STR(b));
    str_printc("-%-%- %:%", STR(a), STR(b), NUM(0b1101101), INT_BIN(0b1101101));
    float threeFormats = timer_elapsed_ms(&t);
    str_printc("| % ms : % formats | % ms other formats", FLOAT(aFewFormats, 3),
               NUM(iterations), FLOAT(threeFormats, 3));

    str_printc("% % % % %", FLOAT(aFewFormats, 0), FLOAT(aFewFormats, 1),
               FLOAT(aFewFormats, 2), FLOAT(aFewFormats, 3),
               FLOAT(aFewFormats, 4));

    str_printc("> ipos % - ineg % - ibin %", NUM(5), NUM(-7), INT_BIN(19));

    str_printc("'%' - %", STR(a), STR_OBJ(a));
}

int main(int argc, char* argv[])
{
    Timer t = {};
    timer_start(&t);
    StringMemory mem = {};
    str_init(&mem, 1024 * 1, 256, 256);

    test2();

    str formatted =
        str_formatc("% - % - % - % - %", FLOAT(1.005, 5), FLOAT(1.005, 4),
                    FLOAT(1.005, 3), FLOAT(0.07, 2), FLOAT(0.07, 1));

    str first = str_static("ToCompare");
    str firstLower = str_static("tocompare");
    str partOnly = str_sub(first, 0, 3);
    str doesntMatch = str_static("imDifferent");
    str justLonger = str_static("ToCompare!!");

    timer_elapsed_ms(&t);

    StrCompareOptions caseInsensitive = {true};
    bool notLower = str_equals(first, firstLower);
    bool withLower = str_equals_opt(first, firstLower, caseInsensitive);
    bool notPart = str_equals(first, partOnly);
    bool notAtAll = str_equals(first, doesntMatch);
    bool notLonger = str_equals_opt(first, justLonger, caseInsensitive);

    float cmpTime = timer_elapsed_ms(&t);

    str_printc("| %  ms for 5 comparisons | % (t %) % % %", FLOAT(cmpTime, 5),
               BOO(notLower), BOO(withLower), BOO(notPart), BOO(notAtAll),
               BOO(notLonger));

    str partAlloc = str_allocn("This is my string", 6);
    str_printc("%", STR(partAlloc));

    timer_elapsed_ms(&t);
    debug_print_pool(mem.persistent, 64);
    debug_print_pool(mem.print_buffer, 64);
    debug_print_pool(mem.transient, 64);
    float printPoolTime = timer_elapsed_ms(&t);
    float programTime = timer_ms_since_start(&t);

    str_printc("| % ms print pool | % ms program runtime |",
               FLOAT(printPoolTime, 3), FLOAT(programTime, 3));

    return 0;
}
