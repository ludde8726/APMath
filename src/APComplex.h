#pragma once

#include "APFloat.h"

typedef struct {
    APFloat *real;
    APFloat *imag;
} APComplex;

APComplex *init_apcomplex(size_t capacity);
APComplex *copy_apcomplex(APComplex *num);
void free_apcomplex(APComplex *num);