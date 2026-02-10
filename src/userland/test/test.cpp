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

int main(int argc, char* argv[])
{
    (void)(argc);
    (void)(argv);

    debug_printf("GitOS TestSuite\n");

    test_printf();

    debug_printf("Finished.");

    while (1)
        ;
}
