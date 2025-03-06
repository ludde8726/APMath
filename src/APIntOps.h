#pragma once

#include <stdint.h>

#include "APNumber.h"

// **** Helper functions ****
#if 1

// Compare the absolute values of x and y
// if x > y -> 1, if x < y -> -1, if x == y -> 0
int apint_abs_compare(const APInt *x, const APInt *y);

// In-place left shift: Multiply the number by 10^n.
// This is achieved by shifting the digits right in the digits array (toward higher indices).
// Note that this function modifies x.
void apint_left_shift_inplace(APInt *x, uint32_t n);

// In-place right shift: Divide the number by 10^n.
// This is achieved by shifting the digits left in the digits array (toward lower indices).
// Note that this function modifies x.
void apint_right_shift_inplace(APInt *x, uint32_t n);

// Left shift: Multiply the number by 10^n
// This is achieved by shifting the digits right in the digits array (toward higher indices).
// Note that this function returns a new shifted copy of x.
APInt *apint_left_shift(APInt *x, uint32_t n);

// Right shift: Divide the number by 10^n
// This is achieved by shifting the digits left in the digits array (toward lower indices).
// Note that this function returns a new shifted APInt.
APInt *apint_right_shift(APInt *x, uint32_t n);

#endif 

// **** Arthmetic operations ****
#if 1

// Addition function: Add two APInts  
// Computes the sum of two APInts using long addition  
// If the result of the operation is larger than UINT32_MAX undefined behaviour may occur
APInt *apint_add(APInt *x, APInt *y);  
APInt *apint_add_impl(APInt *x, APInt *y, uint32_t precision);  

// Subtraction function: Subtract two APInts  
// Computes the difference of two APInts using long subtraction  
// If the result of the operation is larger than UINT32_MAX undefined behaviour may occur
APInt *apint_sub(APInt *x, APInt *y);    
APInt *apint_sub_impl(APInt *x, APInt *y, uint32_t precision);  

// Multiplication function: Multiply two APInts  
// Computes the product of two APInts using long multiplication  
// If the result of the operation is larger than UINT32_MAX undefined behaviour may occur
APInt *apint_mul(APInt *x, APInt *y);  
APInt *apint_mul_impl(APInt *x, APInt *y, uint32_t precision);  

// Division function: Divide two APInts  
// Computes the quotient of two APInts using long division  
// Stores the remainder in the provided pointer if not null  
// If the result of the operation is larger than UINT32_MAX undefined behaviour may occur
APInt *apint_div(APInt *x, APInt *y, APInt **remainder);  
APInt *apint_div_impl(APInt *x, APInt *y, APInt **remainder, uint32_t precision);  

// Exponentiation function: Compute x raised to the power of y  
// Computes the power using the fast exponentiation algorithm (exponentiation by squaring),  
// which repeatedly squares the base and reduces the exponent by half.  
// If the result of the operation is larger than UINT32_MAX undefined behaviour may occur
APInt *apint_pow(APInt *x, APInt *y);

#endif