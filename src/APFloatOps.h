#pragma once

#include <stdint.h>

#include "APNumber.h"

// Alignment function: Aligns the exponent of two APFloats
// Adjusts the mantissas of the given APFloats so that both share the same exponent,
// ensuring that subsequent arithmetic operations (like addition or subtraction)
// can be performed accurately.
// Note that this function operates at the current working precision and some precision may be lost.
void apfloat_align(APFloat *x, APFloat *y);

// Addition function: Add two APFloats
// Computes the sum of two APFloats using long addition  
// Note that this function works on the current working precision and may modify the parameters.  
APFloat *apfloat_add(APFloat *x, APFloat *y);

// Addition function: Add two APFloats at a specified precision
// Computes the sum of two APFloats using long addition  
// Requires a specified precision and may modify the parameters  
APFloat *apfloat_add_ex(APFloat *x, APFloat *y, uint32_t precision);
APFloat *apfloat_add_impl(APFloat *x, APFloat *y, uint32_t precision);


// Subtraction function: Subtract two APFloats
// Computes the difference between two APFloats using long subtraction  
// Note that this function works on the current working precision and may modify the parameters.  
APFloat *apfloat_sub(APFloat *x, APFloat *y);

// Subtraction function: Subtract two APFloats at a specified precision
// Computes the difference between two APFloats using long subtraction
// Requires a specified precision and may modify the parameters
APFloat *apfloat_sub_ex(APFloat *x, APFloat *y, uint32_t precision);
APFloat *apfloat_sub_impl(APFloat *x, APFloat *y, uint32_t precision);


APFloat *apfloat_mul(APFloat *x, APFloat *y);
APFloat *apfloat_mul_ex(APFloat *x, APFloat *y, uint32_t precision);
APFloat *apfloat_mul_impl(APFloat *x, APFloat *y, uint32_t precision);


APFloat *apfloat_div(APFloat *x, APFloat *y);
APFloat *apfloat_div_ex(APFloat *x, APFloat *y, uint32_t precision);
APFloat *apfloat_div_impl(APFloat *x, APFloat *y, uint32_t precision);


APFloat *apfloat_pow(APFloat *x, APFloat *y);
APFloat *apfloat_pow_ex(APFloat *x, APFloat *y, uint32_t precision);
APFloat *apfloat_pow_impl(APFloat *x, APFloat *y, uint32_t precision);


APFloat *apfloat_log(APFloat *x, APFloat *base);
APFloat *apfloat_log_ex(APFloat *x, APFloat *base);
APFloat *apfloat_log_impl(APFloat *x, APFloat *base);