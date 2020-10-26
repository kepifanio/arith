#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "arith40.h"
#include "math.h"
#include "bitpack.h"

typedef struct y_pb_pr {
    float Y;
    float Pb;
    float Pr;
} *y_pb_pr;

typedef struct abcdPbPr {
    unsigned a;
    signed b;
    signed c;
    signed d;
    unsigned Pb;
    unsigned Pr;
} *abcdPbPr;

struct closure {
    A2Methods_UArray2 *array;
    A2Methods_T methods;
};

A2Methods_UArray2 rgb_to_cv(Pnm_ppm image,
      A2Methods_T methods, A2Methods_mapfun *map);

void apply_rgb_to_cv(int i, int j, A2Methods_UArray2 original,
    void *elem, void *cl);

A2Methods_UArray2 cv_to_word(A2Methods_UArray2 cv_image,
    A2Methods_T methods, A2Methods_mapfun map);

void apply_cv_to_word(int i, int j, A2Methods_UArray2 cv_image,
    void *elem, void *cl);

uint64_t float_to_int_a(float value, uint64_t coefficient);

int64_t float_to_int_bcd(float value, uint64_t coefficient);


void compressRange(float *value);

A2Methods_UArray2 word_to_codeword(A2Methods_UArray2 quantized_image,
    A2Methods_T methods, A2Methods_mapfun map);

void apply_word_to_codeword(int i, int j,
    A2Methods_UArray2 quantized_image, void *elem, void *cl);


/* -------------------------------------------------------------------*/
/* -------------------- Decompression Functions ----------------------*/
/* -------------------------------------------------------------------*/

A2Methods_UArray2 codewords_to_words(A2Methods_UArray2 codewords,
    A2Methods_mapfun map, A2Methods_T methods);

void apply_codewords_to_words(int i, int j, A2Methods_UArray2 array,
    void *elem, void *cl);

A2Methods_UArray2 words_to_cv(A2Methods_UArray2 cv_array,
    A2Methods_mapfun *map, A2Methods_T methods);

void apply_words_to_cv (int i, int j, A2Methods_UArray2 array,
    void *elem, void *cl);

float decompressRange(float value);

// Pnm_ppm cv_to_rgb(A2Methods_UArray2 cv_array,
//     A2Methods_mapfun *map, A2Methods_T methods);
