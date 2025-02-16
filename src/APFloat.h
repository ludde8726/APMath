#pragma once

#include "APInt.h"

typedef struct {
    int sign;
    APInt *significand;
    long long exponent;
} APFloat;

APFloat *init_apfloat(size_t capacity);
APFloat *apfloat_from_apint(APInt *num, long long exponent);
APFloat *copy_apfloat(APFloat *num);
void free_apfloat(APFloat *num);