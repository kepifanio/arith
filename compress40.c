#include <stdio.h>
#include <stdlib.h>
#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "input_output.h"
#include "compress_decompress.h"

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
    A2Methods_UArray2 cv_image = rgb_to_cv(image, methods, map);

    /* Convert from cv structs to quantized word */
    A2Methods_UArray2 quantized_image = cv_to_word
                        (cv_image, methods, map);

    /* Convert from words to codewords */
    A2Methods_UArray2 codeword_image = word_to_codeword
                        (quantized_image, methods, map);

    /* Print compressed image to standard output */
    printCompressed(codeword_image, methods);

    /* Free memory */
    Pnm_ppmfree(&image);
    methods->free(&cv_image);
    methods->free(&quantized_image);
    methods->free(&codeword_image);
}

void decompress40(FILE *fp)
{
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);
    A2Methods_mapfun *map = methods->map_default;
    assert(map);

    /* Create ppm for image and populate array of codewords */
    A2Methods_UArray2 codewords = readBinary(fp, map, methods);

    A2Methods_UArray2 word_array = codewords_to_words(codewords, map, methods);

    A2Methods_UArray2 cv_array = words_to_cv(word_array, map, methods);

    Pnm_ppm rgb_image = cv_to_rgb(cv_array, map, methods);

    Pnm_ppmwrite(stdout, rgb_image);

    methods->free(&codewords);
    methods->free(&word_array);
    methods->free(&cv_array);
    Pnm_ppmfree(&rgb_image);
}
