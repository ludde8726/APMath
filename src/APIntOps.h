#pragma once

#include "APNumber.h"

int apint_abs_compare(const APInt *x, const APInt *y);
void apint_left_shift_inplace(APInt *x, uint32_t n);
void apint_right_shift_inplace(APInt *x, uint32_t n);
APInt *apint_left_shift(APInt *x, uint32_t n);
APInt *apint_right_shift(APInt *x, uint32_t n);

APInt *apint_add(APInt *x, APInt *y);
APInt *apint_add_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_sub(APInt *x, APInt *y);
APInt *apint_sub_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_mul(APInt *x, APInt *y);
APInt *apint_mul_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_div(APInt *x, APInt *y, APInt **remainder);
APInt *apint_div_ex(APInt *x, APInt *y, APInt **remainder, uint32_t precision);
APInt *apint_pow(APInt *x, APInt *y);
APInt *apint_pow_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_log(APInt *x, APInt *base); // Might not be needed