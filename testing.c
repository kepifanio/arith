#include <stdio.h>
#include <stdlib.h>
#include "testing.h"
#include "rgb_cv_conversion.h"
#include <stdbool.h>
#include "arith40.h"

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
    (void) cv_image;
    float y_expected, pb_expected, pr_expected;
    float y_stored, pb_stored, pr_stored;
    bool correct = true;
    for (unsigned row = 0; row < image->height; row++) {
        for (unsigned col = 0; col < image->width; col++) {
            Pnm_rgb rgb_struct = image->methods->at(image->pixels, col, row);
            float red = (float) rgb_struct->red / image->denominator;
            float green = (float) rgb_struct->green / image->denominator;
            float blue = (float) rgb_struct->blue / image->denominator;
            y_expected = 0.299 * red + 0.587 * green + 0.114 * blue;
            pb_expected = - 0.168736 * red - 0.331264 * green + 0.5 * blue;
            pr_expected = 0.5 * red - 0.418688 * green - 0.081312 * blue;
            y_pb_pr cv_struct = image->methods->at(cv_image, col, row);
            y_stored = cv_struct->Y;
            pb_stored = cv_struct->Pb;
            pr_stored = cv_struct->Pr;
            if (y_expected != y_stored) {
                correct = false;
                printf("[%d][%d]\n", col, row);
                printf("\nStored values: Y: %f  Pb: %f  Pr: %f\n",
                y_stored, pb_stored, pr_stored);
                printf("Expected     : Y: %f  Pb: %f  Pr: %f\n\n",
                y_expected, pb_expected, pr_expected);
            }
        }
    }
    if (correct == true) {
        printf("All good!\n");
    } else {
        printf("Unexptected values!!\n");
    }
}

/* Test Bitpack.h */
void test_bitpack(Pnm_ppm image, A2Methods_UArray2 cv_image) {
    /* word will pack a, b, c, d, Pb, and Pr */
    /* a, b, c, d have width 6 & Pr, Pb have width 4 */
    int abcd_width = 6;
    int PbPr_width = 4;
    uint64_t word = 0;

    /* Tester Pb & Pr 4-bit values */
    y_pb_pr cv1 = image->methods->at(cv_image, 0, 0);
    y_pb_pr cv2 = image->methods->at(cv_image, 1, 0);
    y_pb_pr cv3 = image->methods->at(cv_image, 2, 0);
    y_pb_pr cv4 = image->methods->at(cv_image, 3, 0);
    float avgPb = ((cv1->Pb) + (cv2->Pb) + (cv3->Pb) + (cv4->Pb)) / 4;
    float avgPr = ((cv1->Pr) + (cv2->Pr) + (cv3->Pr) + (cv4->Pr)) / 4;
    unsigned Pb_4bit = Arith40_index_of_chroma(avgPb);
    unsigned Pr_4bit = Arith40_index_of_chroma(avgPr);

    /* pack word */
    // word = Bitpack_newu(word, PbPr_width, 0, Pr_4bit);

    (void) Pb_4bit;
    (void) Pr_4bit;
    (void) PbPr_width; 
    (void) word;
    (void) abcd_width;
}
