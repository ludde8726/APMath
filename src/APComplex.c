#include <stdlib.h>

#include "APFloat.h"
#include "APComplex.h"

APComplex *init_apcomplex(size_t capacity) {
    APComplex *num = (APComplex*)malloc(sizeof(APComplex));
    if (!num) return NULL;
    num->real = init_apfloat(capacity);
    num->imag = init_apfloat(capacity);
    if (num->real == NULL || num->imag == NULL) {
        free_apfloat(num->real);
        free_apfloat(num->imag);
        free(num);
    }
    return num;
}

APComplex *copy_apcomplex(APComplex *num)
{
    if (num->real->significand->capacity != num->imag->significand->capacity) return NULL;
    APComplex *res = init_apcomplex(num->real->significand->capacity);
    res->real = copy_apfloat(num->real);
    res->imag = copy_apfloat(num->imag);
    return res;
}

void free_apcomplex(APComplex *num) {
    free_apfloat(num->real);
    free_apfloat(num->imag);
    free(num);
}
