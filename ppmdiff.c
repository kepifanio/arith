#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <a2plain.h>
#include <pnm.h>
#include <a2methods.h>
#include <stdbool.h>

const float DENOMINATOR = 255.0;

void width_and_height(Pnm_ppm image1, Pnm_ppm image2);
void pixels(Pnm_ppm image1, Pnm_ppm image2);
int compareDimensions(int dimension1, int dimension2);
float comparePixs(Pnm_rgb pix1, Pnm_rgb pix2);
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

    fclose(fp1);
    fclose(fp2);
    Pnm_ppmfree(&image1);
    Pnm_ppmfree(&image2);
    
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
    int width = compareDimensions(image1->width, image2->width);
    int height = compareDimensions(image1->height, image2->height);

    float sum = 0;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            Pnm_rgb pix1 = image1->methods->at(image1->pixels, col, row);
            Pnm_rgb pix2 = image2->methods->at(image2->pixels, col, row);
            sum += comparePixs(pix1, pix2);
        }
    }

    float E = sqrt(1.0 * sum/(3 * width * height));
    fprintf(stdout, "%.4f\n", E);
}

float comparePixs(Pnm_rgb pix1, Pnm_rgb pix2)
{
    float red = ((int)(pix1->red) - (int)(pix2->red)) / DENOMINATOR;
    float green = ((int)(pix1->green) - (int)(pix2->green)) / DENOMINATOR;
    float blue = ((int)(pix1->blue) - (int)(pix2->blue)) / DENOMINATOR;

    red = red * red;
    green = green * green;
    blue = blue * blue;

    return red + green + blue;
}

int compareDimensions(int dimension1, int dimension2)
{
    if (dimension1 > dimension2) {
        return dimension2;
    } else {
        return dimension1;
    }
}
