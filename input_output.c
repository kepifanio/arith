#include <stdio.h>
#include <stdlib.h>
#include "input_output.h"
#include <stdbool.h>

/*
 * This function takes the original inputted image and methods
 *     and returns a Pnm_ppm image. If the inputted image's
 *     width and/or height are not even, the function trims
 *     the rows and/or columns to make them even and then
 *     returns the Pnm_ppm.
 */
Pnm_ppm readPPM(FILE *input, A2Methods_T methods)
{
    bool trim = false;
    Pnm_ppm image = Pnm_ppmread(input, methods);
    A2Methods_UArray2 new_image;

    if (image->width % 2 != 0) {
        trim = true;
        image->width = image->width - 1;
    }
    if (image->height % 2 != 0) {
        trim = true;
        image->height = image->height - 1;
    }

    if (trim == false) {
        return image;
    } else {
        /* New 2D array */
        unsigned int new_width = image->width;
        unsigned int new_height = image->height;
        new_image = methods->new
            (new_width, new_height, sizeof(struct Pnm_rgb));
        unsigned int row, col;
        for (row = 0; row < new_height; row++) {
            for (col = 0; col < new_width; col++) {
                Pnm_rgb original_pixel = methods->at
                    (image->pixels, col, row);
                Pnm_rgb new_pixel = methods->at
                    (new_image, col, row);
                new_pixel->red = original_pixel->red;
                new_pixel->green = original_pixel->green;
                new_pixel->blue = original_pixel->blue;
            }
        }
    }
    /* Free original array of pixels */
    methods->free(&(image->pixels));
    image->pixels = new_image;
    return image;
}

void printCompressed(A2Methods_UArray2 codeword_image, A2Methods_T methods)
{
    int width = methods->width(codeword_image);
    int height = methods->height(codeword_image);

    printf("COMP40 Compressed image format 2\n%u %u", width * 2, height * 2);
    printf("\n");

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            uint64_t word = *(uint64_t *) methods->at
                            (codeword_image, col, row);
             for (int i = 3; i >= 0; i--) {
                 char byte = (char) Bitpack_getu(word, 8, 8 * i);
                 putchar(byte);
             }
        }
    }
}

A2Methods_UArray2 readBinary(FILE *input, A2Methods_mapfun *map,
    A2Methods_T methods)
{
    unsigned width, height;
    int header = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                                        &width, &height);
    assert(header == 2);
    int c_in = getc(input);
    assert(c_in == '\n');
    width = width / 2;
    height = height / 2;
    A2Methods_UArray2 codewords = methods->new(width, height,
                                           sizeof(uint64_t));
    map(codewords, applyRead, input);
    return codewords;

}

void applyRead(int i, int j, A2Methods_UArray2 array,
    void *elem, void *cl)
{
    (void) i;
    (void) j;
    (void) array;
    FILE *input = cl;
    assert(input);
    uint64_t temp = 0;
    uint64_t byte;
    uint64_t *codeword = elem;
    for (int i = 24; i >= 0; i = i - 8 ) {
        byte = getc(input);
        byte = byte << i;
        temp = temp | byte;
    }
    *codeword = temp;
}


// struct Pnm_ppm createPPM(FILE *input, A2Methods_T methods)
// {
//     unsigned width, height;
//     int header = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
//                                                     &width, &height);
//     assert(header == 2);
//     int c_in = getc(input);
//     assert(c_in == '\n');
//
//     struct Pnm_ppm pixmap = { .width = width, .height = height
//                             , .denominator = 255, .pixels = NULL
//                             , .methods = methods
//                             };
//
//     return pixmap;
// }
//
// A2Methods_UArray2 readCompressed(FILE *input, A2Methods_T methods,
//     int width, int height)
// {
//
//     A2Methods_UArray2 codewords = methods->new(width / 2, height / 2,
//                                             sizeof(uint64_t));
//
//     for (int row = 0; row < height / 2; row++) {
//         for (int col = 0; col < width / 2; col++) {
//             uint64_t codeword = 0;
//             for (int i = 3; i >= 0; i--) {
//                 uint64_t byte = getc(input);
//                 codeword = Bitpack_newu(codeword, 8, (8 * i), byte);
//             }
//             *(uint64_t*)methods->at(codewords, col, row) = codeword;
//         }
//     }
//     return codewords;
// }
