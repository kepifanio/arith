#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <a2methods.h>
#include <a2plain.h>
#include <pnm.h>

int badDimensions(Pnm_ppm image1, Pnm_ppm image2);

int main(int argc, char *argv[])
{
    assert(argc == 3);
    FILE *fp1, *fp2;
    Pnm_ppm image1, image2;
    int col, row;
    (void)col;
    (void)row;

    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);

    fp1 = fopen(argv[1], "rb");
    image1 = Pnm_ppmread(fp1, methods);

    fp2 = fopen(argv[2], "rb");
    image2 = Pnm_ppmread(fp2, methods);

    /* Compare dimensions of two images */
    printf("Image 1 Dimensions - \n");
    printf("Width: %d\n", image1->width);
    printf("Height: %d\n", image1->height);

    printf("Image 2 Dimensions - \n");
    printf("Width: %d\n", image2->width);
    printf("Height: %d\n", image2->height);
    if (badDimensions(image1, image2)) {
        fprintf(stderr, "image dimensions are not similar\n");
        fprintf(stdout, "1.0\n");
    }

    printf("images are not bad dimensions\n");

    return 0;
}

int badDimensions(Pnm_ppm image1, Pnm_ppm image2)
{
    int heightDiff = (image1->height - image2->height);
    int widthDiff = (image1->width - image2->width);

    if (abs(heightDiff) > 1) {
        return 1;
    }

    if (abs(widthDiff) > 1) {
        return 1;
    }

    return 0;
}
