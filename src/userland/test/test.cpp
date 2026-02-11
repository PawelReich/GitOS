#include "graphics/framebuffer.hpp"
extern "C"
{
#include <misc.h>
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "misc.h"
}

void test_printf()
{
    debug_printf("%s: %d %x\n", __FUNCTION__, 12345, 0xDEADBEEF);
}

void test_simple_scanf()
{
    int base10number = 0;
    int base16number = 0;

    scanf("12345", "%d", &base10number);
    scanf("ABBA", "%x", &base16number);

    debug_printf("%s: %d, %x\n", __FUNCTION__, base10number, base16number);
}

void test_multiple_scanf()
{
    int base10number = 0;
    int base10number2 = 0;

    int base16number = 0;
    int base16number2 = 0;

    scanf("12345 CAFEBABE 67890 ABBA", "%d %x %d %x", &base10number, &base16number, &base10number2, &base16number2);

    debug_printf("%s: %d %x %d %x\n", __FUNCTION__, base10number, base16number, base10number2, base16number2);
}

int main(int argc, char* argv[])
{
    (void)(argc);
    (void)(argv);

    debug_printf("GitOS TestSuite\n");

    test_printf();
    test_simple_scanf();
    test_multiple_scanf();

    debug_printf("Finished.");

    while (1)
        ;
}
