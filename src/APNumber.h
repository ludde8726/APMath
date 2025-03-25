#pragma once

#include <stdint.h>
#include <stdio.h>

#define DIGITS_DTYPE uint8_t

// Struct for reprecenting large ints.
// sign = 1 or -1
// size = The number of digits
// capacity = The maximum number of digits the int can store (the amount allocated)
// digits = The digits of stored in little endian order (ex. 123 = 3 2 1)
// NOTE: Size and capacity does not need to be bigger than unit32_t since this requires ~4.3 GB of memory
typedef struct {
    int8_t sign;
    uint32_t size;
    uint32_t capacity;
    DIGITS_DTYPE *digits;
} APInt;

// Struct for reprecenting large floats.
// sign = 1 or -1
// significand = An APInt storing the significant values of the float
// exponent = The exponent, all APFloats are represented as a*10^b, where a is an int and b is a int exponent 
// NOTE: Size and capacity does not need to be bigger than unit32_t since this requires ~4.3 GB of memory
typedef struct {
    int8_t sign;
    APInt *significand;
    int64_t exponent;
} APFloat;

// Struct for representing large complex numbers
// real & imag = APFloats storing the real and imaginary values of the complex number
typedef struct {
    APFloat *real;
    APFloat *imag;
} APComplex;

// Int helpers
APInt *apint_init();
APInt *apint_init_ex(uint32_t precision);
APInt *apint_copy(APInt *num);
APInt *apint_copy_ex(APInt *num, uint32_t precision);
void apint_copy_into(APInt *x, APInt *y);
void apint_copy_into_resize(APInt *x, APInt *y);
int apint_resize(APInt *num, uint32_t precision);
APInt *apint_from_int(long long num);
void apint_free(APInt *num);
void apint_normalize(APInt *num);
int apint_is_zero(APInt *num);

// Float helpers
APFloat *apfloat_init();
APFloat *apfloat_init_ex(uint32_t precision);
APFloat *apfloat_init_empty();
APFloat *apfloat_copy(APFloat *num);
APFloat *apfloat_copy_ex(APFloat *num, uint32_t precision);
int apfloat_resize(APFloat *num, uint32_t precision);
APFloat *apfloat_from_apint(APInt *num, int64_t exponent);
APFloat *apfloat_from_apint_ex(APInt *num, int64_t exponent, uint32_t precision);
void apfloat_free(APFloat *num);
void apfloat_normalize(APFloat *num);
int apfloat_is_zero(APFloat *num);
int apfloat_is_int(APFloat *num);

// Complex helpers
APComplex *apcomplex_init();
APComplex *apcomplex_init_ex(uint32_t precision);
APComplex *apcomplex_copy(APComplex *num);
void apcomplex_free(APComplex *num);