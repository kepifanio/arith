#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include <stdbool.h>
#include <assert.h>
#include <except.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

uint64_t left_shiftu(uint64_t val, unsigned shift)
{
    if (shift == 64) {
        return 0;
    } else {
        uint64_t new_val = val << shift;
        return new_val;
    }
}

uint64_t rightShift(uint64_t val, unsigned shift)
{
    assert(shift <= 64);

    if (shift < 64)
    {
        val = val << shift;
    }
    else {
        val = 0;
    }

    return val;
}

int64_t arithRight(int64_t val, unsigned shift)
{
    assert(shift <= 64);

    val = val << shift;

    return val;
}

/* This function tests whether n can be represented in width bits. */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    if (width >= 64) {
        return true;
    }

    uint64_t maxVal = left_shiftu(1, width);

    if (n < maxVal) {
        return true;
    } else {
        return false;
    }
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
    if (width >= 64) {
        return true;
    }

    if (width == 0) {
        return false;
    }

    bool fits = false;
    if (n >= 0) {
        fits = Bitpack_fitsu(2 * n, width);
    } else {
        fits = Bitpack_fitsu((~(n-1)) * 2 - 1, width);
    }

    return fits;
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    uint64_t mask = ~0;
    mask = left_shiftu(rightShift(mask, 64 - width), lsb);

    word = word & mask;

    word = rightShift(word, lsb);

    return word;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    // uint64_t highOrder = ~0;
    // highOrder = left_shiftu(highOrder, 63);
    //
    // uint64_t lowOrder = ~0;
    // lowOrder = left_shiftu(rightShift(lowOrder, 64 - (width - 1)), lsb);
    //
    // uint64_t mask = lowOrder | highOrder;
    //
    // word = word & mask;
    //
    // return (int64_t) word;

    uint64_t mask = ~0;
    mask = left_shiftu(rightShift(mask, 64 - width), lsb);

    word = word & mask;

    word = arithRight(word, lsb);

    return word;
}

uint64_t Bitpack_newu(uint64_t word, unsigned width,
    unsigned lsb, uint64_t value)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    /* Check if the given value fits in the given width */
    if (Bitpack_fitsu(value, width) != true) {
        RAISE(Bitpack_Overflow);
        assert(0);
    }

    /* Make mask */
    unsigned shift = 64 - width - lsb;
    uint64_t left = (left_shiftu(1, shift)) - 1;
    left = left_shiftu(left, width + lsb);
    uint64_t right = (left_shiftu(1, lsb)) - 1;
    uint64_t mask = left + right;

    /* Get word */
    uint64_t new_word = (word & mask) | left_shiftu(value, lsb);
    return new_word;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
    int64_t value)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    if (Bitpack_fitss(value, width) != true) {
        RAISE(Bitpack_Overflow);
        assert(0);
    }

    /* Make mask and transfer sign value */
    uint64_t left = (left_shiftu(1, 64 - width - lsb)) - 1;
    left = left_shiftu(left, width + lsb);
    uint64_t right = (left_shiftu(1, lsb)) - 1;
    uint64_t mask = left + right;
    uint64_t new_value = value;
    new_value = left_shiftu(value, 64 - width);
    new_value = left_shiftu(value, 64 - width - lsb);

    /* Get word */
    uint64_t new_word = word & mask;
    new_word = new_word | new_value;
    return new_word;
}
