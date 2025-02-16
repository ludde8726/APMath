#pragma once

#include "APMath.h"

int apint_abs_compare(const APInt *x, const APInt *y);
void apint_shift_right(APInt *num, size_t shift);
void apfloat_align_exponents(APFloat *x, APFloat *y);

APInt *apint_add(APInt* x, APInt *y);
APFloat *apfloat_add(APFloat* x, APFloat *y);
APComplex *apcomplex_add(APComplex* x, APComplex *y);

APInt *apint_sub(APInt* x, APInt *y);
APFloat *apfloat_sub(APFloat* x, APFloat *y);
APComplex *apcomplex_sub(APComplex* x, APComplex *y);