#include "APMul.h"

APInt *apint_mul(APInt *x, APInt *y) {
    if (x->size == 0 || (x->size == 1 && x->digits[0] == 0) || y->size == 0 || (y->size == 1 && y->digits[0] == 0)) {
        APInt *zero = init_apint(1);
        zero->size = 1;
        zero->digits[0] = 0;
        return zero;
    }
    size_t max_size = x->size + y->size;
    APInt *res = init_apint(max_size);
    res->size = max_size;

    for (size_t i = 0; i < x->size; i++) {
        int carry = 0;
        for (size_t j = 0; j < y->size || carry; j++) {
            int product = res->digits[i + j] + x->digits[i] * (j < y->size ? y->digits[j] : 0) + carry;
            res->digits[i + j] = product % 10;
            carry = product / 10;
        }
    }
    res->sign = x->sign * y->sign;
    apint_normalize(res);
    return res;
}

APFloat *apfloat_mul(APFloat *x, APFloat *y) {
    APFloat *res = init_apfloat(x->significand->size+y->significand->size);
    res->sign = x->sign * y->sign;
    res->exponent = x->exponent + y->exponent;
    APInt *significand_product = apint_mul(x->significand, y->significand);
    res->significand = significand_product;
    apfloat_normalize(res);
    return res;
}

APComplex *apcomplex_mul(APComplex *x, APComplex *y) {
    return NULL;
}
