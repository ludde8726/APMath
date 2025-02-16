#include <stdlib.h>

#include "APFloat.h"
#include "APInt.h"

APFloat *init_apfloat(size_t capacity) {
    APFloat *num = (APFloat*)malloc(sizeof(APFloat));
    if (!num) return NULL;
    num->sign = 1;
    num->exponent = 1;
    num->significand = init_apint(capacity);
    num->significand->sign = 1;
    if (num->significand == NULL) {
        free(num);
        return NULL;
    }
    return num;
}

APFloat *apfloat_from_apint(APInt *num, long long exponent)
{
    APFloat *res = init_apfloat(num->capacity);
    res->sign = num->sign;
    num->sign = 1;
    res->significand = num;
    res->exponent = exponent;
}

APFloat *copy_apfloat(APFloat *num)
{
    APFloat *res = init_apfloat(num->significand->capacity);
    res->sign = num->sign;
    res->significand = copy_apint(num->significand);
    res->significand->sign = 1;
    res->exponent = num->exponent;
    return res;
}

void free_apfloat(APFloat *num) {
    free_apint(num->significand);
    free(num);
}