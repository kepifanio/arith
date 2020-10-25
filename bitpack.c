#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include <stdbool.h>
#include <assert.h>
#include <except.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

uint64_t leftShift(uint64_t val, unsigned shift)
{
    assert(shift <= 64);

    if (shift < 64)
    {
        return (val << shift);
    }
    else {
        return 0;
    }
}

uint64_t rightShift(uint64_t val, unsigned shift)
{
    assert(shift <= 64);

    if (shift < 64)
    {
        return (val >> shift);
    }
    else {
        return 0;
    }
}

int64_t arithRight(int64_t val, unsigned shift)
{
    assert(shift <= 64);

    val = val >> shift;

    return val;
}

/* This function tests whether n can be represented in width bits. */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= 64);

    uint64_t maxVal = ~0;

    maxVal = ~(leftShift(maxVal, width));

    return (n <= maxVal);
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= 64);

    uint64_t maxInt = ~0;

    int64_t maxVal = (int64_t) ~(leftShift(maxInt, width - 1));

    int64_t minVal = ~maxVal;

    return (n <= maxVal && n >= minVal);
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    uint64_t mask = ~0;
    mask = leftShift(rightShift(mask, 64 - width), lsb);

    word = word & mask;

    word = rightShift(word, lsb);

    return word;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    uint64_t mask = ~0;
    mask = leftShift(rightShift(mask, 64 - width), lsb);

    word = word & mask;

    word = leftShift(word, 64 - (width + lsb));

    return arithRight(word, 64 - width);
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    /* Check if the given value fits in the given width */
    if (!Bitpack_fitsu(value, width)) {
        RAISE(Bitpack_Overflow);
    }

    uint64_t mask = ~0;
    mask = leftShift(rightShift(mask, 64 - width), 64 - (width + lsb));
    mask = ~mask;

    word = word & mask;

    value = leftShift(value, lsb);

    return word | value;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    /* Check if the given value fits in the given width */
    if (!Bitpack_fitss(value, width)) {
        RAISE(Bitpack_Overflow);
    }

    int64_t mask = ~0;
    mask = leftShift(rightShift(mask, 64 - width), 64 - (width + lsb));

    value = leftShift(value, lsb);
    value = value & mask;

    word = word & ~mask;
    word = word | value;

    return word;
}
