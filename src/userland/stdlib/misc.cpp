#include "misc.h"

void sleep(uint64_t ms)
{
    uint64_t* val = new uint64_t;
    gettick(val);
    uint64_t start = *val;
    do
    {
        gettick(val);
    } while ((*val - start) < ms);
    delete val;
}
