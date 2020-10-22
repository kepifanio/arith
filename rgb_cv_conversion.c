#include <stdio.h>
#include <stdlib.h>
#include "rgb_cv_conversion.h"

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
 *     converint from an array of Pnm_rgb's to an array of y_pb_pr
 *     structs.
 */
void apply_rgb_to_cv(int i, int j, A2Methods_UArray2 original,
    void *elem, void *cl)
{
    (void) original;
    Pnm_ppm image = (Pnm_ppm) cl;
    unsigned denominator = image->denominator;
    // TODO: check whether there's a situation where denominator
    //          is not equal to image->denominator
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

A2Methods_UArray2 cv_to_32bit(A2Methods_UArray2 cv_image, A2Methods_T methods,
      A2Methods_mapfun *map)
{
    
}
