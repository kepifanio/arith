#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <a2plain.h>
#include <pnm.h>
#include <a2methods.h>
#include <stdbool.h>

void width_and_height(Pnm_ppm image1, Pnm_ppm image2);
void pixels(Pnm_ppm image1, Pnm_ppm image2);
int comparePixs(Pnm_rgb pix1, Pnm_rgb pix2);

int main(int argc, char *argv[])
{
    assert(argc == 3);

    FILE *fp1, *fp2;
    Pnm_ppm image1, image2;
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    fp1 = fopen(argv[1], "rb");
    fp2 = fopen(argv[2], "rb");
    assert(fp1 != NULL && fp2 != NULL);
    image1 = Pnm_ppmread(fp1, methods);
    image2 = Pnm_ppmread(fp2, methods);
    assert(image1 != NULL && image2 != NULL);

    width_and_height(image1, image2);
    pixels(image1, image2);

    return 0;
}

void width_and_height(Pnm_ppm image1, Pnm_ppm image2)
{
    unsigned int width1, height1, width2, height2;
    width1 = image1->width;
    height1 = image1->height;
    width2 = image2->width;
    height2 = image2->height;

    int widthDiff = width1 - width2;
    int heightDiff = height1 - height2;
    if (abs(widthDiff) > 1) {
        fprintf(stderr, "\nWidth dimensions differ by more than 1\n");
        printf("1.0\n");
    }
    if (abs(heightDiff) > 1) {
        fprintf(stderr, "\nHeight dimensions differ by more than 1\n");
        printf("1.0\n");
    }
    printf("\n");
}

void pixels(Pnm_ppm image1, Pnm_ppm image2)
{
    int sum = 0;
    Pnm_rgb pix1, pix2;
    unsigned int row, col;
    for (row = 0; row < image1->height; row++) {
        if (row >= image2->height) {
            break;
        }
        for (col = 0; col < image1->width; col++) {
            if (col >= image2->width) {
                break;
            }
            pix1 = image1->methods->at(image1->pixels, col, row);
            pix2 = image2->methods->at(image2->pixels, col, row);
            sum += comparePixs(pix1, pix2);
        }
    }
    sum = sum / (3 * (row + 1) * (col + 1));
    double E = sqrt(1.0 * sum / (3 * (row + 1) * (col + 1)));
    printf("E: %.4f\n\n", E);
}

int comparePixs(Pnm_rgb pix1, Pnm_rgb pix2)
{
    int redDiff, greenDiff, blueDiff;
    redDiff = pix1->red - pix2->red;
    greenDiff = pix1->green - pix2->green;
    blueDiff = pix1->blue - pix2->blue;
    int sum = (redDiff * redDiff) + (greenDiff * greenDiff)
        + (blueDiff * blueDiff);
    return sum;
}
