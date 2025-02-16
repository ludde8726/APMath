#include <math.h>

#include "APMath.h"
#include "Print.h"

char *string_apint(APInt *num) {
    int memory_needed = sizeof(char)*(num->size + (num->sign == -1 ? 1: 0));
    char *res = (char*)malloc(memory_needed);
    if (!res) return NULL;
    size_t index = 0;
    if (num->sign == -1) res[index++] = '-';
    for (size_t i = num->size; i>0; i--) {
        res[index++] = num->digits[i-1] + '0';
    }
    return res;
}

char *string_apfloat(APFloat *num) {
    int memory_needed = sizeof(char)*(num->significand->size + (num->sign == -1 ? 2: 1) + (num->exponent > 0 ? 5+(int)log10(num->exponent) : 0));
    char *res = (char*)malloc(memory_needed);
    if (!res) return NULL;
    size_t index = 0;
    if (num->sign == -1) res[index++] = '-';
    for (size_t i = num->significand->size; i>0; i--) {
        if (i == num->significand->size + num->exponent) res[index++] = '.';
        res[index++] = num->significand->digits[i-1] + '0';
    }
    if (num->exponent > 0) snprintf(res+index, 6+(int)log10(num->exponent), "*10^%lld", num->exponent);
    return res;
}

char *string_apcomplex(APComplex *num) {
    int real_memory_needed = sizeof(char)*(num->real->significand->size + (num->real->sign == -1 ? 2: 1) + (num->real->exponent > 0 ? 4+(int)log10(num->real->exponent) : 0));
    int imag_memory_needed = sizeof(char)*(num->imag->significand->size + (num->imag->sign == -1 ? 2: 1) + (num->imag->exponent > 0 ? 4+(int)log10(num->imag->exponent) : 0));
    char *res = (char*)malloc(real_memory_needed+imag_memory_needed);
    char *real_str = string_apfloat(num->real);
    char *imag_str = string_apfloat(num->imag);
    snprintf(res, 5 + (real_memory_needed + imag_memory_needed)/sizeof(char), "%s + %si", real_str, imag_str);
    free(real_str);
    free(imag_str);
    return res;
}

void print_apint(APInt *num) {
    char *str_int = string_apint(num);
    printf("%s\n", str_int);
    free(str_int);
}

void print_apfloat(APFloat *num) {
    char *str_float = string_apfloat(num);
    printf("%s\n", str_float);
    free(str_float);
}

void print_apcomplex(APComplex *num) {
    char *str_complex = string_apcomplex(num);
    printf("%s\n", str_complex);
    free(str_complex);
}