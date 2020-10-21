#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"

typedef struct y_pb_pr {
    float Y;
    float Pb;
    float Pr;
} *y_pb_pr;

A2Methods_UArray2 rgb_to_cv(Pnm_ppm image,
      A2Methods_T methods, A2Methods_mapfun *map);

void apply_rgb_to_cv(int i, int j, A2Methods_UArray2 original,
    void *elem, void *cl);
