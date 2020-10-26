#include <stdio.h>
#include <stdlib.h>
#include "arith40.h"
#include "pnm.h"
#include "a2plain.h"
#include "a2methods.h"
#include "bitpack.h"
#include <assert.h>

extern Pnm_ppm readPPM(FILE *input, A2Methods_T methods);

void printCompressed(A2Methods_UArray2 codeword_image,
    A2Methods_T methods);

// struct Pnm_ppm createPPM(FILE *input, A2Methods_T methods);
//
// A2Methods_UArray2 readCompressed(FILE *input, A2Methods_T methods,
//     int width, int height);

void applyRead(int i, int j, A2Methods_UArray2 array,
    void *elem, void *cl);

A2Methods_UArray2 readBinary(FILE *input, A2Methods_mapfun *map,
    A2Methods_T methods);
