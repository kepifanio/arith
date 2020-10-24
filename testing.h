#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"
#include "bitpack.h"

extern void testTrimming(Pnm_ppm image);

void test_rgb_to_cv(Pnm_ppm image, A2Methods_UArray2 cv_image);

void test_cv_to_word(A2Methods_UArray2 quantized_image,
    A2Methods_T methods); 

void test_bitpack();
