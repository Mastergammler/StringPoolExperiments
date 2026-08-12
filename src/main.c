#include "alloc/module.h"
#include "measure/module.h"
#include "string/internal.h"
#include "string/macros.h"
#include "string/module.h"
#include "string/types.h"

#include <stdio.h>

// TODO: create a macro, to expand array into fmt args?
// -> Is this not somehow possible?
#define EXP_8_ARG(arr)                                                         \
    &arr[0], &arr[1], &arr[2], &arr[3], &arr[4], &arr[5], &arr[6], &arr[7]

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
    str_printc("Buffer Init: % ms", fmt_f(initTime, 3));
    float formatTime = timer_elapsed_ms(&t);
    printf("Buffer Init: %.3f ms\n", initTime);
    float pfFormatTime = timer_elapsed_ms(&t);
    str_printc("Own f-format: % ms\nPrintf f-format: % ms",
               fmt_f(formatTime, 4), fmt_f(pfFormatTime, 4));

    timer_elapsed_ms(&t);
    str tmpl = str_alloc("This is a static string template. % time we already "
                         "used it. \nObj: %");

    int iterations = 20;
    for (int i = 0; i < iterations; i++)
    {
        str_print(tmpl, fmt_n(i), STR_OBJ(tmpl));
    }
    str formatted = str_format(tmpl, fmt_n(iterations),
                               STR_OBJ(str_static("Something new")));
    float aFewFormats = timer_elapsed_ms(&t);
    str_printc("21 Formats: %", fmt_f(aFewFormats, 3));

    str a = str_static("'First One'");
    str b = str_alloc("'Second One'");
    str_printc("%-%", fmt_s(a), fmt_s(b));
    str_printc("-%-%", fmt_s(a), fmt_s(b));
    str_printc("-%-%- %:%", fmt_s(a), fmt_s(b), fmt_n(0b1101101),
               fmt_n(0b1101101, pxB));
    float threeFormats = timer_elapsed_ms(&t);
    str_printc("| % ms : % formats | % ms other formats", fmt_f(aFewFormats, 3),
               fmt_n(iterations), fmt_f(threeFormats, 3));

    str_printc("% % % % %", fmt_f(aFewFormats, 0), fmt_f(aFewFormats, 1),
               fmt_f(aFewFormats, 2), fmt_f(aFewFormats, 3),
               fmt_f(aFewFormats, 4));

    str_printc("> ipos % - ineg % - ibin %", fmt_n(5), fmt_n(-7),
               fmt_n(19, pxB));

    str_printc("'%' - %", fmt_s(a), STR_OBJ(a));
}

NumFormat* pInMethod()
{
    NumFormat* fmt = fmt_n(5);
    int dumme = 123455;
    NumFormat actualObj = *fmt;
    printf("%p %ld\n", fmt, fmt->value);
    printf("Actual: %p %ld\n", &actualObj, actualObj.value);
    return fmt;
}

void test_compares()
{
    Timer t = {};
    timer_start(&t);

    NumFormat* fmt = pInMethod();
    NumFormat actualOutside = *fmt;
    printf("ActualOutside: %p %ld\n", &actualOutside, actualOutside.value);
    fmt->value = 7;
    printf("Outside: %p %ld\n", fmt, fmt->value);

    str formatted =
        str_formatc("% - % - % - % - %", fmt_f(1.005, 5), fmt_f(1.005, 4),
                    fmt_f(1.005, 3), fmt_f(0.07, 2), fmt_f(0.07, 1));

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

    str_printc("| %  ms for 5 comparisons | % (t %) % % %", fmt_f(cmpTime, 5),
               fmt_b(notLower), fmt_b(withLower), fmt_b(notPart),
               fmt_b(notAtAll), fmt_b(notLonger));
}

void testing_floats()
{
    Timer t = {};
    timer_elapsed_ms(&t);
    float num = 3.97152;
    float num2 = 3.31;
    str_printc("[0]%  [1]% [2]%  [3]%  [4]%  [5]% [0-2]%", fmt_f(num, 0),
               fmt_f(num, 1), fmt_f(num, 2), fmt_f(num, 3), fmt_f(num, 4),
               fmt_f(num, 5), fmt_f(num2, 0));

    float multi = -3.9999;
    str_printc("[0]%  [1]% [2]%  [3]%  [4]%  [5]% ", fmt_f(multi, 0),
               fmt_f(multi, 1), fmt_f(multi, 2), fmt_f(multi, 3),
               fmt_f(multi, 4), fmt_f(multi, 5));
    float format10 = timer_elapsed_ms(&t);
    str_printc("Formatting 10 floats: %", fmt_f(format10, 3));

    float leadingZero = 3.0007;
    str_printc("[0]%  [1]% [2]%  [3]%  [4]%  [5]% ", fmt_f(leadingZero, 0),
               fmt_f(leadingZero, 1), fmt_f(leadingZero, 2),
               fmt_f(leadingZero, 3), fmt_f(leadingZero, 4),
               fmt_f(leadingZero, 5));

    str partAlloc = str_allocn("This is my string", 6);
    str_printc("%", fmt_s(partAlloc));
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

    str_printc("Hres: %, FacilityMask %, codemask %", fmt_n(hres, pxB),
               fmt_n(facilityMask, pxB), fmt_n(codeMask, pxB));

    int facility = hres >> 16 & facilityMask;
    int code = hres & codeMask;
    short code2 = hres;

    printf("Facility: %x Code: %x Code2: %x\n", facility, code, code2);
    str_printc("Failure %, Reserved %, Custom %, NtStatus %, ReservedX %",
               fmt_b(hres & severityMask), fmt_b(hres & reservedMask),
               fmt_b(hres & customerMask), fmt_b(hres & ntStatusMask),
               fmt_b(hres & reservedX));
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
    POOL_count
} StrPools;

static Program Prog;

void testing_strings_etc()
{
    str_printc("% % % % % % %", fmt_n(1), fmt_n(12), fmt_n(237), fmt_n(5730),
               fmt_n(-7738), fmt_n(50027), fmt_n(0));
    str_printc("% % % % % %", fmt_n(5, 0, 0), fmt_n(5, 0, 1), fmt_n(5, 0, 2),
               fmt_n(5, 0, 3), fmt_n(307, 0, 5), fmt_n(0, 0, 3));
    str_printc("% % % % %", fmt_n(7, 0, 7), fmt_n(10), fmt_n(10, 0, 2),
               fmt_n(11), fmt_n(100));

    testing_floats();

    str maStrings[3] = {str_static("hello"), str_static("world"),
                        str_static("this is a bit longer")};
    int count = sizeof(maStrings) / sizeof(str);

    str joined =
        str_joinc(maStrings, count, ">>Totally busted and stuff yeee<<");
    str_printc("Joined: %", fmt_s(joined));

    int hres = -2146233087;
    print_hres(hres);

    // timer_elapsed_ms(&t);
    debug_print_pool(Prog.strings.pools[POOL_DEFAULT], 64);
    debug_print_pool(Prog.strings.pools[POOL_PRINT], 64);
}

StringMemory* StrMem;

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
    str_pool_finalize(Prog.ctx);

    str_printc("Program init: % ms", fmt_f(timer_elapsed_ms(&Prog.timer), 3));
}

void print_ascii()
{
    str_printc("Space-ascii: %", fmt_n(' '));
    str_printc("Tab-ascii: %", fmt_n('\t'));
    str_printc("Newline-ascii: %", fmt_n('\n'));
    str_printc("CR-ascii: %", fmt_n('\r'));
}

void array_test()
{
    int* myInt = NULL;
    arr_append(myInt, 2);
    arr_append(myInt, 4);
    arr_append(myInt, 7);

    for (int i = 0; i < arr_len(myInt); i++)
    {
        str_printc("Array el: %", fmt_n(myInt[i]));
    }

    print_ascii();

    str someString = str_static("  \t\nThis is my string  \t\r\r");
    str_printc("'%'", fmt_s(str_trim(someString)));
}

void xor_tests()
{
    int cmp10 = 10;
    int cmp100 = 100;

    for (int i = 0; i < 32; i++)
    {
        int res = i;
        int stripped = i; // & 0b1111;

        int loops = 0;

        while (res >= cmp10 && loops < 5)
        {
            res = cmp10 ^ stripped;
            str_printc("[%] Stripped res: %", fmt_n(loops), fmt_n(res, pxB));
            loops++;
        }

        str_printc("10: (%) % ^ % -> %", fmt_n(i), fmt_n(stripped, pxB),
                   fmt_n(cmp10, pxB), fmt_n(res));
        // str_printc("100: (%) % ^ % -> %", fmt_n(i), INT_BIN(i),
        // INT_BIN(cmp100), fmt_n(xor100));
    }
}

int main(int argc, char* argv[])
{
    init_program(4096);

    Timer timer = {};

    StringPool* transient = &StrMem->pools[StrMem->default_pool];
    int64_t num = -1527713833408340293;

    // it seems cacheing only applies if i print the exact same value
    // -> Then it's quite a bit faster, but else just a bit
    // => I mean it's still 5.00 microns difference but yea
    printf("Checking if this leads to printf cacheing\n");
    timer_start(&timer);

    str_printc("Num: '%'", fmt_u(num));
    float printDirect = timer_elapsed_ms(&timer);

    NumDesc p = extract_signed(num, 0);
    str expanded = print_signed(transient->memory, &p.header);
    str_printc("Num: '%'", fmt_s(expanded));
    float printNew = timer_elapsed_ms(&timer);

    str numStr = str_formatc("%", fmt_n(num));
    str_printc("Num: '%'", fmt_s(numStr));
    float printOld = timer_elapsed_ms(&timer);

    printf("Num: '%lu'\n", (long)num);
    float tprintf = timer_elapsed_ms(&timer);

    str_printc("% - %", fmt_b(true), fmt_b(false));
    float printStatic = timer_elapsed_ms(&timer);

    str_printc("Bin: %", fmt_n(num, pxB));
    str_printc("Padded: %", fmt_n(172, 0, 5));

    // testing_strings_etc();

    float printPoolTime = timer_elapsed_ms(&Prog.timer);
    float programTime = timer_ms_since_start(&Prog.timer);
    str_printc(
        "| % ms print pool | % ms program runtime \n| % fmt-print | % new | % "
        "printDirect | % printf | % static |",
        fmt_f(printPoolTime, 3), fmt_f(programTime, 3), fmt_f(printOld, 5),
        fmt_f(printNew, 5), fmt_f(printDirect, 5), fmt_f(tprintf, 5),
        fmt_f(printStatic, 5));

    FloatFormat times[16];
    timer_start(&timer);
    for (int i = 0; i < 16; i++)
    {
        str_printc("Num: %", fmt_n(num + 10 * i));
        float elapsed = timer_elapsed_ms(&timer);
        times[i] = *fmt_f(elapsed, 5);
    }

    str_printc("% % % % % % % %", EXP_8_ARG(times));
    str_printc("% % % % % % % %", EXP_8_ARG((times + 8)));

    debug_print_pool(*POOL(POOL_PRINT), 64);

    timer_start(&timer);
    str_printc("Bin: %", fmt_n(num, pxB));
    float normalPrint = timer_elapsed_ms(&timer);
    str_printc("Bin: % and something", fmt_n(num, pxB));
    float wraparound = timer_elapsed_ms(&timer);
    str_printc("Bin: % and more", fmt_n(num, pxB));
    float wrap2 = timer_elapsed_ms(&timer);
    str_printc("Bin: % ...", fmt_n(num, pxB));
    float wrap3 = timer_elapsed_ms(&timer);
    str_printc("Bin: % last", fmt_n(num, pxB));
    float wraplast = timer_elapsed_ms(&timer);

    str_printc("| % normal | % wrapped | % w2 | % w3 | % last |",
               fmt_f(normalPrint, 5), fmt_f(wraparound, 5), fmt_f(wrap2, 5),
               fmt_f(wrap3, 5), fmt_f(wraplast, 5));

    debug_print_pool(*POOL(POOL_PRINT), 64);

    return 0;
}
