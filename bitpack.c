#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include <stdbool.h>
#include <assert.h>

uint64_t leftShift(uint64_t val, unsigned shift)
{
    assert(shift <= 64);

    val = val << shift;

    return val;
}

/* This function tests whether n can be represented in width bits. */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= 64);

    uint64_t maxInt = ~0;

    uint64_t maxVal = ~(leftShift(maxInt, width));

    if (n <= maxVal) {
        return true;
    }

    else {
        return false;
    }
}

bool Bitpack_fitss(int64_t n, unsigned width)
{

}
