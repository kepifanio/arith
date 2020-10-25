#include <stdio.h>
#include <stdlib.h>
#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "image.h"
#include "testing.h"
#include "rgb_cv_conversion.h"

void compress40(FILE *fp)
{
    /* Create UArray2 and set default mapping function */
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);
    A2Methods_mapfun *map = methods->map_default;
    assert(map);

    /* Process fp and trim as necessary */
    Pnm_ppm image = readPPM(fp, methods);

    /* Convert from rgb pixels to array of y_pb_pr structs */
    //A2Methods_UArray2 cv_image = rgb_to_cv(image, methods, map);

    /* Convert from cv structs to quantized word */
    //A2Methods_UArray2 quantized_image = cv_to_word
                        //(cv_image, methods, map);


    /* Convert from words to codewords */
    //A2Methods_UArray2 codeword_image = word_to_codeword
                        //(quantized_image, methods, map);


    printf("testing area!\n");

    // uint64_t n = -3;
    // unsigned width = 3;
    //
    // if(Bitpack_fitsu(n, width)) { printf("fitsu is working\n"); }
    //
    // if(Bitpack_fitss(n, width)) { printf("fitss is working\n"); }

    uint64_t test = Bitpack_gets(0x3f4, 6, 2);

    printf("test == %ld\n", test);

    /* Free memory */
    Pnm_ppmfree(&image);
    // methods->free(&cv_image);
    // methods->free(&quantized_image);
    // methods->free(&codeword_image);
}

void decompress40(FILE *fp)
{
    printf("in decompress\n");
    (void) fp;
}
