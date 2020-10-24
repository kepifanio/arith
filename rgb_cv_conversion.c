#include <stdio.h>
#include <stdlib.h>
#include "rgb_cv_conversion.h"
#include <assert.h>

#define A_COEF 511
#define BCD_COEF 50
#define ABCD_WIDTH 6
#define PBPR_WIDTH 4

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
    unsigned denominator = image->denominator;
    y_pb_pr cv_struct = malloc(sizeof(struct y_pb_pr));
    Pnm_rgb rgb_pixel = image->methods->at(image->pixels, i, j);

    /* Convert from rgb to floating point representation */
    float red = (float) rgb_pixel->red / denominator;
    float green = (float) rgb_pixel->green / denominator;
    float blue = (float) rgb_pixel->blue / denominator;

    /* Calculate Y, Pb, and Pr values and store in struct */
    cv_struct->Y = 0.299 * red + 0.587 * green + 0.114 * blue;
    cv_struct->Pb = - 0.168736 * red - 0.331264 * green + 0.5 * blue;
    cv_struct->Pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;

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
    avgPb = ((cv1->Pb) + (cv2->Pb) + (cv3->Pb) + (cv4->Pb)) / 4.0;
    avgPr = ((cv1->Pr) + (cv2->Pr) + (cv3->Pr) + (cv4->Pr)) / 4.0;

    /* Convert a, b, c, d */
    a_float = (cv4->Y + cv3->Y + cv2->Y + cv1->Y) / 4.0;
    b_float = (cv4->Y + cv3->Y - cv2->Y - cv1->Y) / 4.0;
    c_float = (cv4->Y - cv3->Y + cv2->Y - cv1->Y) / 4.0;
    d_float = (cv4->Y - cv3->Y - cv2->Y + cv1->Y) / 4.0;

    /* b_float, c_float, and d_float must be between -0.3 and 0.3 */
    range(&b_float);
    range(&c_float);
    range(&d_float);

    /* Store quantized values in struct */
    word_struct->Pb = (uint64_t)(Arith40_index_of_chroma(avgPb));
    word_struct->Pr = (uint64_t)(Arith40_index_of_chroma(avgPr));
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
void range(float *value) {
    if (*value < -0.3) {
        *value = -0.3;
    } else if (*value > 0.3) {
        *value = 0.3;
    }
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

void apply_word_to_codeword(int i, int j,
    A2Methods_UArray2 quantized_image, void *elem, void *cl)
{
    (void) quantized_image;
    struct closure *codeword_cl = cl;
    abcdPbPr word_struct = elem;
    uint64_t temp = 0;
    temp = Bitpack_newu(temp, PBPR_WIDTH, 0, word_struct->Pr);
    temp = Bitpack_newu(temp, PBPR_WIDTH, PBPR_WIDTH, word_struct->Pb);
    temp = Bitpack_news(temp, ABCD_WIDTH, 2 * PBPR_WIDTH, word_struct->d);
    temp = Bitpack_news(temp, ABCD_WIDTH, (2 * PBPR_WIDTH) + ABCD_WIDTH,
                                                        word_struct->c);
    temp = Bitpack_news(temp, ABCD_WIDTH, (2 * PBPR_WIDTH) + (2 * ABCD_WIDTH),
                                                        word_struct->b);
    // temp = Bitpack_news(temp, ABCD_WIDTH, (2 * PBPR_WIDTH) + (3 * ABCD_WIDTH),
    //                                                     word_struct->a);

    uint64_t *codeword = codeword_cl->methods->at
                        (codeword_cl->array, i, j);
    *codeword = temp;
}