#pragma once

#include "APNumber.h"

int apint_abs_compare(const APInt *x, const APInt *y);
void apint_shift_right(APInt *num, size_t shift);

APInt *apint_add(APInt *x, APInt *y);
APInt *apint_add_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_sub(APInt *x, APInt *y);
APInt *apint_sub_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_mul(APInt *x, APInt *y);
APInt *apint_mul_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_div(APInt *x, APInt *y);
APInt *apint_div_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_pow(APInt *x, APInt *y);
APInt *apint_pow_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_log(APInt *x, APInt *base); // Might not be needed