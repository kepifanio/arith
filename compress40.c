#include <stdio.h>
#include <stdlib.h>
#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "input_output.h"
#include "testing.h"
#include "compression_conversion.h"

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
    struct Pnm_ppm image;
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);
    A2Methods_mapfun *map = methods->map_default;
    assert(map);

    /* Create ppm for image and populate array of codewords */
    image = createPPM(fp, methods);
    A2Methods_UArray2 codewords = readCompressed
                (fp, methods, image.width, image.height);

    /* Convert array of codewords to array of cv structs */
    A2Methods_UArray2 cv_array = codewords_to_cv(codewords, &image,
                                                    methods, map);


    methods->free(&codewords);
    methods->free(&cv_array);
}
