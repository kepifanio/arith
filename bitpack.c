/**************************************************************
 *                      bitpack.c
 *
 *    Assignment: locality
 *    Authors: Noah Wright (nwrigh05) &
               Katherine Epifanio (kepifa01)
 *    Date: 10.26.20
 *
 *    Summary:
 *
 *          This file contains the implementation for the
 *          bitpack interface. In the compression process
 *          Bitpack_newu and Bitpack_news pack the integer
 *          elements of each pixel into a 32-bit codeword,
 *          while bitpack_getu and bitpack_gets are called
 *          in the decompression process to unpack codewords.
 *
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include <stdbool.h>
#include <assert.h>
#include <except.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };


/* This is a helper function that preforms a left shift on unsigned bits.
 *     It takes a uint64_t val and left shifts it by shift number of bits.
 *     If shift value is greater than the max number of bits than it results
 *     in a checked runtime error. If shift is equal to max number of bits
 *     then it returns a val = 0;
 */
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

/* This is a helper function that preforms a right shift on unsigned bits.
 *     It takes a uint64_t val and right shifts it by shift number of bits.
 *     If shift value is greater than the max number of bits than it results
 *     in a checked runtime error. If shift is equal to max number of bits
 *     then it returns a val = 0;
 */
uint64_t rightShift(uint64_t val, unsigned shift)
{
    assert(shift <= 64);

    if (shift <= 63) {
        val = (val >> shift);
    } else {
        val = 0;
    }
    return val;
}

/* This is a helper function that preforms a right shift on signed bits.
 *     It takes a int64_t val and right shifts it by shift number of bits.
 *     If shift value is greater than the max number of bits than it results
 *     in a checked runtime error. If shift is equal to max number of bits
 *     then it returns a val = 0 if val is positive and returns ~0 if val
 *     is negative.
 */
int64_t arithRight(int64_t val, unsigned shift)
{
    assert(shift <= 64);

    if (shift <= 63) {
        val = (val >> shift);
    } else if (val >= 0) {
        val = 0;
    } else {
        val = ~0;
    }

    return val;
}

/* This function is a helper function that replaces the value in word from lsb
 *      to width + lsb with the inputed uint64_t value. Returns a new uint64_t
 *      with the repalced value.
 */
uint64_t Bitpack_change(uint64_t word, unsigned width, unsigned lsb,
    int64_t value)
{
    int64_t mask = ~0;
    mask = leftShift(mask, 64 - width);
    mask = rightShift(mask, 64 - width - lsb);
    value = leftShift(value, lsb);
    value = value & mask;
    word = word & ~mask;
    word = word | value;
    return word;
}

/* This function tests whether uint64_t n can be represented in width bits.
 *      Results in a checked runtime error if width is greater than the max
 *      number of bits in a uint64_t. Returns true if n can be represented in
 *      width bits and false otherwise.
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= 64);

    uint64_t maxVal = ~0;

    maxVal = ~(leftShift(maxVal, width));

    return (n <= maxVal);
}

/* This function tests whether int64_t n can be represented in width bits.
 *      Results in a checked runtime error if width is greater than the max
 *      number of bits in a uint64_t. Returns true if n can be represented in
 *      width bits and false otherwise.
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= 64);

    uint64_t maxInt = ~0;

    int64_t maxVal = (int64_t) ~(leftShift(maxInt, width - 1));

    int64_t minVal = ~maxVal;

    return (n <= maxVal && n >= minVal);
}

/* This function extracts a field from lsb to lsb + width from uint64_t word.
 *      Results in a checked runtime error if width is greater than the max
 *      number of bits in a uint64_t or if width + lsb is greater than the max
 *      number of bits. Returns the extracted value.
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    uint64_t mask = ~0;
    mask = leftShift(mask, 64 - width);
    mask = rightShift(mask, 64 - width - lsb);
    word = word & mask;
    word = rightShift(word, lsb);

    return word;
}

/* This function extracts a field from lsb to lsb + width from int64_t word.
 *      Results in a checked runtime error if width is greater than the max
 *      number of bits in a uint64_t or if width + lsb is greater than the max
 *      number of bits. Returns the extracted value.
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    uint64_t mask = ~0;
    mask = leftShift(mask, 64 - width);
    mask = rightShift(mask, 64 - width - lsb);
    word = word & mask;
    word = leftShift(word, 64 - width - lsb);
    word = arithRight(word, 64 - width);

    return (int64_t) word;
}

/* This function replaces the value in word from lsb to width + lsb with the
 *      inputed uint64_t value. Results in a checked runtime error if width is
 *      greater than the max number of bits in a uint64_t or if width + lsb is
 *      greater the max number of bits. Raises an exception if value can not be
 *      represented in width amount of bits. Returns a new uint64_t with the
 *      repalced value.
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned
      lsb, uint64_t value)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    /* Check if the given value fits in the given width */
    if (Bitpack_fitsu(value, width)) {
        word = Bitpack_change(word, width, lsb, value);
    } else {
        RAISE(Bitpack_Overflow);
    }

    return word;
}

/* This function replaces the value in word from lsb to width + lsb with the
 *      inputed int64_t value. Results in a checked runtime error if width is
 *      greater than the max number of bits in a uint64_t or if width + lsb is
 *      greater the max number of bits. Raises an exception if value can not be
 *      represented in width amount of bits. Returns a new uint64_t with the
 *      repalced value.
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
      int64_t value)
{
    assert(width <= 64);
    assert(width + lsb <= 64);

    if (Bitpack_fitss(value, width)) {
        word = Bitpack_change(word, width, lsb, value);
    } else {
        RAISE(Bitpack_Overflow);
    }

    return word;
}
