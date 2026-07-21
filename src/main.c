#include "alloc/module.h"
#include "measure/module.h"
#include "string/internal.h"
#include "string/macros.h"
#include "string/module.h"
#include "string/types.h"

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
    str_print(str_fmt_ptr(&StrMem->pools[StrMem->default_pool]));
    str_print(str_fmt_b(12));
    str_print(str_fmt_b(false));
    str_print(str_fmt_i(12, 0));
    str_print(str_fmt_i(-17, 0));
    str_print(str_fmt_i(134775, 0));
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

IntFormat* pInMethod()
{
    IntFormat* fmt = NUM(5);
    int dumme = 123455;
    IntFormat actualObj = *fmt;
    printf("%p %i\n", fmt, fmt->value);
    printf("Actual: %p %i\n", &actualObj, actualObj.value);
    return fmt;
}

void test_compares()
{
    Timer t = {};
    timer_start(&t);

    IntFormat* fmt = pInMethod();
    IntFormat actualOutside = *fmt;
    printf("ActualOutside: %p %i\n", &actualOutside, actualOutside.value);
    fmt->value = 7;
    printf("Outside: %p %i\n", fmt, fmt->value);

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
}

void testing_floats()
{
    Timer t = {};
    timer_elapsed_ms(&t);
    float num = 3.97152;
    float num2 = 3.31;
    str_printc("[0]%  [1]% [2]%  [3]%  [4]%  [5]% [0-2]%", FLOAT(num, 0),
               FLOAT(num, 1), FLOAT(num, 2), FLOAT(num, 3), FLOAT(num, 4),
               FLOAT(num, 5), FLOAT(num2, 0));

    float multi = -3.9999;
    str_printc("[0]%  [1]% [2]%  [3]%  [4]%  [5]% ", FLOAT(multi, 0),
               FLOAT(multi, 1), FLOAT(multi, 2), FLOAT(multi, 3),
               FLOAT(multi, 4), FLOAT(multi, 5));
    float format10 = timer_elapsed_ms(&t);
    str_printc("Formatting 10 floats: %", FLOAT(format10, 3));

    float leadingZero = 3.0007;
    str_printc("[0]%  [1]% [2]%  [3]%  [4]%  [5]% ", FLOAT(leadingZero, 0),
               FLOAT(leadingZero, 1), FLOAT(leadingZero, 2),
               FLOAT(leadingZero, 3), FLOAT(leadingZero, 4),
               FLOAT(leadingZero, 5));

    str partAlloc = str_allocn("This is my string", 6);
    str_printc("%", STR(partAlloc));
    str_printc("I'm a freaking ninja now!");
    str_printc("That reall kind of works huh?");
}
// TODO: write general possibility to print a hres
//  https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/0642cb2f-2075-4469-918c-4441e69c548a
void print_hres(int hres)
{
    int severityMask = 1 << 31;
    int reservedMask = 1 << 30;
    int customerMask = 1 << 29;
    int ntStatusMask = 1 << 28;
    int reservedX = 1 << 27;
    // FIXME: for some reason not working
    // short facilityMask = (~((unsigned short)0)) >> 5;
    short facilityMask = 0b11111111111;
    // short codeMask = ~(unsigned short)0;
    // STFO: why 15???? why does the mask contain only 15 bits????
    short codeMask = 0b111111111111111;

    str_printc("Hres: %, FacilityMask %, codemask %", INT_BIN(hres),
               INT_BIN(facilityMask), INT_BIN(codeMask));

    int facility = hres >> 16 & facilityMask;
    int code = hres & codeMask;
    short code2 = hres;

    printf("Facility: %x Code: %x Code2: %x\n", facility, code, code2);
    str_printc("Failure %, Reserved %, Custom %, NtStatus %, ReservedX %",
               BOO(hres & severityMask), BOO(hres & reservedMask),
               BOO(hres & customerMask), BOO(hres & ntStatusMask),
               BOO(hres & reservedX));
}

typedef struct
{
    Timer timer;
    Arena memory;
    AllocContext ctx;
    StringMemory strings;

} Program;

typedef enum
{
    POOL_DEFAULT,
    POOL_PRINT,
    POOL_TRANSIENT,
    POOL_count
} StrPools;

static Program Prog;

void testing_strings_etc()
{
    str_printc("% % % % % % %", NUM(1), NUM(12), NUM(237), NUM(5730),
               NUM(-7738), NUM(50027), NUM(0));
    str_printc("% % % % % %", NUM_PAD(5, 0), NUM_PAD(5, 1), NUM_PAD(5, 2),
               NUM_PAD(5, 3), NUM_PAD(307, 5), NUM_PAD(0, 3));
    str_printc("% % % % %", NUM_PAD(7, 7), NUM(10), NUM_PAD(10, 2), NUM(11),
               NUM(100));

    testing_floats();

    str maStrings[3] = {str_static("hello"), str_static("world"),
                        str_static("this is a bit longer")};
    int count = sizeof(maStrings) / sizeof(str);

    str joined =
        str_joinc(maStrings, count, ">>Totally busted and stuff yeee<<");
    str_printc("Joined: %", STR(joined));

    int hres = -2146233087;
    print_hres(hres);

    // timer_elapsed_ms(&t);
    debug_print_pool(Prog.strings.pools[POOL_DEFAULT], 64);
    debug_print_pool(Prog.strings.pools[POOL_PRINT], 64);
    debug_print_pool(Prog.strings.pools[POOL_TRANSIENT], 64);
}

void init_program(uint64_t memory)
{
    timer_start(&Prog.timer);

    // TODO: not sure if this is the best approach
    MglibcAllocCtx = &Prog.ctx;
    Prog.ctx = ctx_init_default(&Prog.memory);
    arena_init(&Prog.memory, memory);

    str_pool_prepare(Prog.ctx, &Prog.strings, POOL_count);
    define_pool(POOL_DEFAULT, 512, .is_default = true);
    define_pool(POOL_PRINT, 256, .is_print = true);
    define_pool(POOL_TRANSIENT, 128, .is_transient = true);
    str_pool_finalize(Prog.ctx);

    str_printc("Program init: % ms", FLOAT(timer_elapsed_ms(&Prog.timer), 3));
}

void print_ascii()
{
    str_printc("Space-ascii: %", NUM(' '));
    str_printc("Tab-ascii: %", NUM('\t'));
    str_printc("Newline-ascii: %", NUM('\n'));
    str_printc("CR-ascii: %", NUM('\r'));
}

int main(int argc, char* argv[])
{
    init_program(4096);

    int* myInt = NULL;
    arr_append(myInt, 2);
    arr_append(myInt, 4);
    arr_append(myInt, 7);

    for (int i = 0; i < arr_len(myInt); i++)
    {
        str_printc("Array el: %", NUM(myInt[i]));
    }

    print_ascii();

    str someString = str_static("  \t\nThis is my string  \t\r\r");
    str_printc("'%'", STR(str_trim(someString)));

    float printPoolTime = timer_elapsed_ms(&Prog.timer);
    float programTime = timer_ms_since_start(&Prog.timer);
    str_printc("| % ms print pool | % ms program runtime |",
               FLOAT(printPoolTime, 3), FLOAT(programTime, 3));

    return 0;
}
