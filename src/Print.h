#pragma once

#include "APMath.h"

char *string_apint(APInt *num);
char *string_apfloat(APFloat *num);
char *string_apcomplex(APComplex *num);

void print_apint(APInt *num);
void print_apfloat(APFloat *num);
void print_apcomplex(APComplex *num);