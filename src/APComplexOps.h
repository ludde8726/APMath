#pragma once

#include "APNumber.h"

APComplex *apcomplex_add(APComplex *x, APComplex *y);
APComplex *apcomplex_sub(APComplex *x, APComplex *y);
APComplex *apcomplex_mul(APComplex *x, APComplex *y);
APComplex *apcomplex_div(APComplex *x, APComplex *y);
APComplex *apcomplex_pow(APComplex *x, APComplex *y);
APComplex *apcomplex_log(APComplex *x, APComplex *base);