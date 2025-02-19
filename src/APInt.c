#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "APInt.h"

APInt *init_apint(size_t capacity) {
    APInt *num = (APInt*)malloc(sizeof(APInt));
    if (!num) return NULL;
    num->sign = 1;
    num->size = 0;
    num->capacity = capacity;
    num->digits = calloc(num->capacity, sizeof(int));
    if (!num->digits) {
        free(num);
        return NULL;
    }
    return num;
}

APInt *copy_apint(APInt *num) {
    APInt *res = init_apint(num->capacity);
    res->sign = num->sign;
    memcpy(res->digits, num->digits, num->size * sizeof(int));
    res->size = num->size;
    return res;
}

APInt *apint_from_string(const char *str) {
    if (!str || !*str) return NULL;
    size_t len = strlen(str);
    int sign = 1;
    size_t start = 0;
    
    if (str[0] == '-') {
        sign = -1;
        start = 1;
    } else if (str[0] == '+') start = 1;
    while (str[start] == '0' && start < len - 1) start++;
    size_t num_digits = len - start;
    APInt *res = init_apint(num_digits);
    if (!res) return NULL;
    res->sign = sign;
    res->size = num_digits;
    for (size_t i = 0; i < num_digits; i++) {
        if (!isdigit(str[start + i])) {
            free_apint(res);
            return NULL;
        }
        res->digits[num_digits-i-1] = str[start + i] - '0';
    }
    return res;
}

void free_apint(APInt *num) {
    free(num->digits);
    free(num);
}

void apint_normalize(APInt *num) {
    while (num->size > 1 && num->digits[num->size - 1] == 0) num->size--;
    if (num->size == 0) {
        num->size = 1;
        num->digits[0] = 1;
    }
}
