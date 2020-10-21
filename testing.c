#include <stdio.h>
#include <stdlib.h>
#include "testing.h"
#include "rgb_cv_conversion.h"
#include <stdbool.h>

/* Outputs the ppm to the file "output.ppm"  after the inputted image
 *     is processed and trimmed as neccesary.
 */
void testTrimming(Pnm_ppm image)
{
    FILE *fp = fopen("output.ppm", "w");
    Pnm_ppmwrite(fp, image);
    fclose(fp);
}

/* Sanity check: prints values of initial rgb_structs, values of
 *     y_pb_pr structs, and checks for expected y_pb_pr values.
 */
void test_rgb_to_cv(Pnm_ppm image, A2Methods_UArray2 cv_image)
{
    y_pb_pr cv_struct;
    float Y, Pb, Pr;
    Pnm_rgb rgb_struct;
    unsigned red, green, blue;

    /* Print cv struct values */
    for (unsigned row = 0; row < image->height; row++) {
        for (unsigned col = 0; col < image->width; col++) {
            cv_struct = image->methods->at(cv_image, col, row);
            Y = cv_struct->Y;
            Pb = cv_struct->Pb;
            Pr = cv_struct->Pr;
            printf("Y: %f  Pb: %f  Pr: %f\n", Y, Pb, Pr);
        }
    }

    /* Print original rgb values */
    for (unsigned row = 0; row < image->height; row++) {
        for (unsigned col = 0; col < image->width; col++) {
            rgb_struct = image->methods->at(image->pixels, col, row);
            red = rgb_struct->red;
            green = rgb_struct->green;
            blue = rgb_struct->blue;
            printf("R: %u  G: %u  B: %u\n", red, green, blue);
        }
    }

    /* Check values */
    bool correct = true;
    for (unsigned row = 0; row < image->height; row++) {
        for (unsigned col = 0; col < image->width; col++) {
            red = (float) red / image->denominator;
            green = (float) green / image->denominator;
            blue = (float) blue / image->denominator;
            if (Y != (0.299 * red + 0.587 * green + 0.114 * blue)) {
                correct = false;
            }
            if (Pb != (- 0.168736 * red - 0.331264 * green + 0.5 * blue)) {
                correct = false;
            }
            if (Pr != (0.5 * red - 0.418688 * green - 0.081312 * blue)) {
                correct = false;
            }
        }
    }

    if (correct == false) {
        printf("ERROR: Unexpected value in y_pb_pr struct!!!\n");
    }
}
