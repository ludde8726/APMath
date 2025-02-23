#pragma once

#include "APNumber.h"

int apfloat_abs_compare(const APFloat *x, const APFloat *y);
void apfloat_align(APFloat *x, APFloat *y);

APFloat *apfloat_add(APFloat *x, APFloat *y);
APFloat *apfloat_sub(APFloat *x, APFloat *y);
APFloat *apfloat_mul(APFloat *x, APFloat *y);
APFloat *apfloat_div(APFloat *x, APFloat *y);
APFloat *apfloat_pow(APFloat *x, APFloat *y);
APFloat *apfloat_log(APFloat *x, APFloat *base);