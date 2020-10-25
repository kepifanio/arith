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
             for (int i = 4; i > 0; i--) {
                 char byte = (char) Bitpack_getu(word, 8, 8 * i);
                 putchar(byte);
             }
        }
    }
}

A2Methods_UArray2 readCompressed(FILE *input, A2Methods_T methods)
{

    unsigned width, height;
    int header = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                                    &width, &height);
    assert(header == 2);
    int c_in = getc(input);
    assert(c_in == '\n');

    A2Methods_UArray2 codeword_image = methods->new(width/2, height/2,
                                                    sizeof(uint64_t));


    return codeword_image;

}
