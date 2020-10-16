#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <a2methods.h>
#include <a2plain.h>
#include <pnm.h>

int main(int argc, char *argv[])
{
    assert(argc == 3);
    FILE *fp1, *fp2;
    Pnm_ppm image1, image2;
    int col, row;

    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);

    fp1 = fopen(argv[1], "rb");
    image1 = pnm_ppmread(fp1, methods);

    fp2 = fopen(argv[2], "rb");
    image2 = pnm_ppmread(fp2, methods);
}
