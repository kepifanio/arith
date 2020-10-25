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

A2Methods_UArray2 readCompressed(FILE *input, A2Methods_T methods);
