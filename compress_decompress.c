/**************************************************************
 *                      compress_decompress.c
 *
 *    Assignment: locality
 *    Authors: Noah Wright (nwrigh05) &
               Katherine Epifanio (kepifa01)
 *    Date: 10.26.20
 *
 *    Summary:
 *
 *          This file contains the main driver code for
 *          compression from ppm images to codeword arrays
 *          and decompression from codeword arrays to ppm
 *          images. Each step in the compression and
 *          decompression process is divided into mapping
 *          and respective apply functions.
 *
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "compression_conversion.h"
#include <assert.h>

#define DENOM 255
#define A_COEF 511
#define BCD_COEF 50
#define A_WIDTH 9
#define BCD_WIDTH 5
#define PBPR_WIDTH 4
#define BCD_RANGE 0.3

/* -------------------------------------------------------------------*/
/* --------------------- Compression Functions -----------------------*/
/* -------------------------------------------------------------------*/

/* This function converts the array of Pnm_rgb's to an array of
 *     y_pb_pr structs by declaring a new UArray2 and passing
 *     it to the mapping function. The function returns the new
 *     UArray2 of initialized y_pb_pr rgb_structs.
 */
A2Methods_UArray2 rgb_to_cv(Pnm_ppm image,
    A2Methods_T methods, A2Methods_mapfun *map)
{
    /* Make new array to store cv structs */
    A2Methods_UArray2 cv_image = methods->new
        (image->width, image->height, sizeof(struct y_pb_pr));

    map(cv_image, apply_rgb_to_cv, image);
    return cv_image;
}

/* This function is the apply function for the mapping process of
 *     converting from an array of Pnm_rgb's to an array of y_pb_pr
 *     structs.
 */
void apply_rgb_to_cv(int i, int j, A2Methods_UArray2 original,
    void *elem, void *cl)
{
    (void) original;
    Pnm_ppm image = (Pnm_ppm) cl;
    y_pb_pr cv_struct = malloc(sizeof(struct y_pb_pr));
    Pnm_rgb rgb_pixel = image->methods->at(image->pixels, i, j);

    /* Convert from rgb to floating point representation */
    float red = (float) rgb_pixel->red / (float)image->denominator;
    float green = (float) rgb_pixel->green / (float)image->denominator;
    float blue = (float) rgb_pixel->blue / (float)image->denominator;

    /* Calculate Y, Pb, and Pr values and store in struct */
    float Y = 0.299 * red + 0.587 * green + 0.114 * blue;
    float Pb = - 0.168736 * red - 0.331264 * green + 0.5 * blue;
    float Pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;

    cv_struct->Y = Y;
    cv_struct->Pb = Pb;
    cv_struct->Pr = Pr;

    *(y_pb_pr)elem = *cv_struct;
    free(cv_struct);
}

/* This function converts the array of cv structs to an array of
 *     half that height and width, containing structs of the
 *     quantized a, b, c, d, Pb, and Pr values. The function
 *     declares a new UArray2, passes it to the map function,
 *     and returns the initialized array.
 */
A2Methods_UArray2 cv_to_word(A2Methods_UArray2 cv_image,
    A2Methods_T methods, A2Methods_mapfun map)
{
    int newWidth = (methods->width(cv_image)) / 2;
    int newHeight = (methods->height(cv_image)) / 2;

    A2Methods_UArray2 quantized_image = methods->new
                    (newWidth, newHeight, sizeof(struct abcdPbPr));

    struct closure *quantize_closure =
        malloc(sizeof(*quantize_closure));
    quantize_closure->array = cv_image;
    quantize_closure->methods = methods;

    map(quantized_image, apply_cv_to_word, quantize_closure);

    free(quantize_closure);
    return quantized_image;
}

/* This function is the apply function for the cv_to_word function
 *     and executes all of the operations to calculate and store
 *     the elements of the abcdPbPr structs.
 *     NEEDS WORK: could be split up into helper functions.
 */
void apply_cv_to_word(int i, int j, A2Methods_UArray2 cv_image,
    void *elem, void *cl)
{
    (void) cv_image;
    struct closure *quantize_closure = cl;
    y_pb_pr cv1, cv2, cv3, cv4;
    int curr_col = i * 2;
    int curr_row = j * 2;
    abcdPbPr word_struct = malloc(sizeof(struct abcdPbPr));
    float avgPb, avgPr, a_float, b_float, c_float, d_float;

    /* Get cv closures from 4 pixels */
    cv1 = quantize_closure->methods->at
        (quantize_closure->array, curr_col, curr_row);
    cv2 = quantize_closure->methods->at
        (quantize_closure->array, curr_col + 1, curr_row);
    cv3 = quantize_closure->methods->at
        (quantize_closure->array, curr_col, curr_row + 1);
    cv4 = quantize_closure->methods->at
        (quantize_closure->array, curr_col + 1, curr_row + 1);

    /* Convert Pb and Pr values */
    avgPb = (float)((cv1->Pb + cv2->Pb + cv3->Pb + cv4->Pb) / 4.0);
    avgPr = (float)((cv1->Pr + cv2->Pr + cv3->Pr + cv4->Pr) / 4.0);

    /* Convert a, b, c, d */
    float Y1 = (float)cv1->Y;
    float Y2 = (float)cv2->Y;
    float Y3 = (float)cv3->Y;
    float Y4 = (float)cv4->Y;
    a_float = ((Y4 + Y3 + Y2 + Y1) / 4.0);
    b_float = ((Y4 + Y3 - Y2 - Y1) / 4.0);
    c_float = ((Y4 - Y3 + Y2 - Y1) / 4.0);
    d_float = ((Y4 - Y3 - Y2 + Y1) / 4.0);

    /* b_float, c_float, and d_float must be between -0.3 and 0.3 */
    b_float = compressRange(b_float);
    c_float = compressRange(c_float);
    d_float = compressRange(d_float);

    /* Store quantized values in struct */
    word_struct->Pb = (unsigned)(Arith40_index_of_chroma(avgPb));
    word_struct->Pr = (unsigned)(Arith40_index_of_chroma(avgPr));
    word_struct->a = (unsigned)(roundf(a_float * A_COEF));
    word_struct->b = (signed)(roundf(b_float * BCD_COEF));
    word_struct->c = (signed)(roundf(c_float * BCD_COEF));
    word_struct->d = (signed)(roundf(d_float * BCD_COEF));

    *(abcdPbPr)elem = *word_struct;
    free(word_struct);
}

/* This function ensures that the b, c, and d floats are within the
 *     range -0.3 and 0.3. If the float value passed is less than
 *     -0.3, value is set to -0.3. If the float value passed is
 *     greater than 0.3, value is set to 0.3. This is part of the
 *     quantization process.
 */
float compressRange(float value) {
    if (value < -0.3) {
        value = -0.3;
    } else if (value > 0.3) {
        value = 0.3;
    }
    return value;
}

/* This function converts the array of word structs to an array
 *     of the final codewords. The function declares a new UArray2,
 *     passes it to the map function, and returns the initialized array.
 */
A2Methods_UArray2 word_to_codeword(A2Methods_UArray2 quantized_image,
    A2Methods_T methods, A2Methods_mapfun map)
{
    int width = methods->width(quantized_image);
    int height = methods->height(quantized_image);

    A2Methods_UArray2 codeword_image = methods->new(width, height,
                                                sizeof(uint64_t));

    struct closure *codeword_cl = malloc(sizeof(*codeword_cl));
    codeword_cl->array = codeword_image;
    codeword_cl->methods = methods;

    map(quantized_image, apply_word_to_codeword, codeword_cl);

    free(codeword_cl);
    return codeword_image;
}

/* This function is the apply function for the word_to_codeword function
 *     and calls the bitpack interface in order to create a uint64_t
 *     with the elements from a abcdPbPr struct packed in.
 */
void apply_word_to_codeword(int i, int j,
    A2Methods_UArray2 quantized_image, void *elem, void *cl)
{
    (void) quantized_image;
    struct closure *codeword_cl = cl;
    abcdPbPr word_struct = elem;
    uint64_t temp = 0;
    temp = Bitpack_newu(temp, PBPR_WIDTH, 0, word_struct->Pr);
    temp = Bitpack_newu(temp, PBPR_WIDTH, PBPR_WIDTH, word_struct->Pb);
    temp = Bitpack_news(temp, BCD_WIDTH, 2 * PBPR_WIDTH, word_struct->d);
    temp = Bitpack_news(temp, BCD_WIDTH, (2 * PBPR_WIDTH) + BCD_WIDTH,
                                                        word_struct->c);
    temp = Bitpack_news(temp, BCD_WIDTH, (2 * PBPR_WIDTH) + (2 * BCD_WIDTH),
                                                        word_struct->b);
    temp = Bitpack_newu(temp, A_WIDTH, (2 * PBPR_WIDTH) + (3 * BCD_WIDTH),
                                                        word_struct->a);

    uint64_t *codeword = codeword_cl->methods->at
                        (codeword_cl->array, i, j);
    *codeword = temp;
}


/* -------------------------------------------------------------------*/
/* -------------------- Decompression Functions ----------------------*/
/* -------------------------------------------------------------------*/

/* This function takes in an array of codewords from a compressed
 *     image and returns an array of unpacked words by mapping
 *     through the array and calling the bitpack interface
 */
A2Methods_UArray2 codewords_to_words(A2Methods_UArray2 codewords,
    A2Methods_mapfun map, A2Methods_T methods)
{

    int width = methods->width(codewords);
    int height = methods->height(codewords);
    A2Methods_UArray2 cv_array = methods->new(width, height,
    sizeof(struct abcdPbPr));

    struct closure *unpack_cl = malloc(sizeof(*unpack_cl));
    unpack_cl->methods = methods;
    unpack_cl->array = cv_array;

    map(codewords, apply_codewords_to_words, unpack_cl);

    free(unpack_cl);
    return cv_array;
}

/* This function is the apply function for the codewords_to_words
 *     function. It calls Bitpack_getu and Bitpack_gets to unpack
 *     the codewords of the array.
 */
void apply_codewords_to_words(int i, int j, A2Methods_UArray2 array,
    void *elem, void *cl)
{
    (void) array;
    struct closure *unpack_cl = cl;
    uint64_t codeword = *((uint64_t*)elem);
    abcdPbPr word_struct = unpack_cl->methods->at(unpack_cl->array, i, j);

    word_struct->a = Bitpack_getu(codeword, A_WIDTH, (3 * BCD_WIDTH)
                                                + (2 * PBPR_WIDTH));
    word_struct->b = Bitpack_gets(codeword, BCD_WIDTH, (2 * BCD_WIDTH)
                                                 + (2 * PBPR_WIDTH));
    word_struct->c = Bitpack_gets(codeword, BCD_WIDTH, BCD_WIDTH +
                                                2 * PBPR_WIDTH);
    word_struct->d = Bitpack_gets(codeword, BCD_WIDTH, 2 * PBPR_WIDTH);
    word_struct->Pb = Bitpack_getu(codeword, PBPR_WIDTH, PBPR_WIDTH);
    word_struct->Pr = Bitpack_getu(codeword, PBPR_WIDTH, 0);

}

/* This function takes in an array of words and returns an array of
 *     cv structs (each of which stores Y, Pb, and Pr).
 */
A2Methods_UArray2 words_to_cv(A2Methods_UArray2 words_array,
    A2Methods_mapfun *map, A2Methods_T methods)
{
    int width = (methods->width(words_array)) * 2;
    int height = (methods->height(words_array)) * 2;
    A2Methods_UArray2 cv_array = methods->new(width, height,
                                    sizeof(struct y_pb_pr));

    struct closure *cv_closure = malloc(sizeof(*cv_closure));
    cv_closure->methods = methods;
    cv_closure->array = cv_array;

    map(words_array, apply_words_to_cv, cv_closure);

    free(cv_closure);
    return cv_array;

}

/* This function is the apply function for the words_to_cv function.
 *     It accesses the y_pb_pr struct at the current element and
 *     performs operators to convert those values to abcdPbPr values.
 *     by performing reverse cosine operations.
 *     NEEDS WORK: could be split up into helper functions. 
 */
void apply_words_to_cv (int i, int j, A2Methods_UArray2 array,
    void *elem, void *cl)
{
    (void) array;
    int col = i * 2;
    int row = j * 2;
    struct closure *unpack_cl = cl;
    abcdPbPr word_struct = elem;
    y_pb_pr cv1, cv2, cv3, cv4;
    float avgPb, avgPr, a, b, c, d;

    /* Set cv struct pointers to indices of cv_array */
    cv1 = unpack_cl->methods->at(unpack_cl->array, col, row);
    cv2 = unpack_cl->methods->at(unpack_cl->array, col + 1, row);
    cv3 = unpack_cl->methods->at(unpack_cl->array, col, row + 1);
    cv4 = unpack_cl->methods->at(unpack_cl->array, col + 1, row + 1);

    /* Calculate and set average Pb values */
    avgPb = Arith40_chroma_of_index(word_struct->Pb);
    cv1->Pb = avgPb;
    cv2->Pb = avgPb;
    cv3->Pb = avgPb;
    cv4->Pb = avgPb;

    // /* Calculate and set average Pr values */
    avgPr = Arith40_chroma_of_index(word_struct->Pr);
    cv1->Pr = avgPr;
    cv2->Pr = avgPr;
    cv3->Pr = avgPr;
    cv4->Pr = avgPr;

    /* Convert a, b, c, d values */
    a = ((float)(word_struct->a) / A_COEF);
    b = ((float)(word_struct->b) / BCD_COEF);
    c = ((float)(word_struct->c) / BCD_COEF);
    d = ((float)(word_struct->d) / BCD_COEF);

    /* Convert y values for each cv struct */
    cv1->Y = (a - b - c + d);
    cv2->Y = (a - b + c - d);
    cv3->Y = (a + b - c - d);
    cv4->Y = (a + b + c + d);
}

/* This function takes in a float as an argument and checks that its
 *     value is between 0.0 and 1.0
 */
float decompressRange(float value)
{
    if (value < 0) {
        return 0.0;
    } else if (value > 1) {
        return 1.0;
    }
    return value;
}

/* This function takes in an array of cv structs, calls a mapping
 *     function, and then returns a new array of Pnm_rgb structs.
 */
Pnm_ppm cv_to_rgb(A2Methods_UArray2 cv_array,
    A2Methods_mapfun *map, A2Methods_T methods)
{
    int width = methods->width(cv_array);
    int height = methods->height(cv_array);
    Pnm_ppm image;
    NEW(image);

    /* Populate data members of Pnm_ppm */
    image->width = width;
    image->height = height;
    image->denominator = DENOM;
    image->methods = methods;
    image->pixels = methods->new(width, height, sizeof(struct Pnm_rgb));

    /* Set closure */
    struct closure *rgb_closure = malloc(sizeof(*rgb_closure));
    rgb_closure->methods = methods;
    rgb_closure->array = image->pixels;

    /* Map and return rgb array */
    map(cv_array, apply_cv_to_rgb, rgb_closure);

    free(rgb_closure);
    return image;
}

/* This function is the apply function for the cv_to_rgb function.
 *     It accesses the cv_struct struct for the current element of
 *     the array and performs the math necessary to produce the
 *     red, green, and blue unsigned values.
 */
void apply_cv_to_rgb(int i, int j, A2Methods_UArray2 array,
    void *elem, void *cl)
{
    (void) array;
    struct closure *rgb_closure = cl;
    y_pb_pr cv_struct = elem;
    Pnm_rgb rgb_struct = rgb_closure->methods->at
                     (rgb_closure->array, i, j);

    /* Convert cv struct values to rgb values */
    rgb_struct->red = (float)(255 * decompressRange(1.0 * (cv_struct->Y)
        + 0.0 * (cv_struct->Pb) + 1.402 * (cv_struct->Pr)));

    rgb_struct->green = (float)(255 * decompressRange(1.0 * (cv_struct->Y)
        - 0.344136 * (cv_struct->Pb) - 0.714136 * (cv_struct->Pr)));

    rgb_struct->blue = (float)(255 * decompressRange(1.0 * (cv_struct->Y)
        + 1.772 * (cv_struct->Pb) + 0.0 * (cv_struct->Pr)));

}
