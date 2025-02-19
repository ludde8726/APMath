#pragma once

#include <stdio.h>

typedef struct {
    int sign;
    size_t size;
    size_t capacity;
    int *digits;
} APInt;

APInt *init_apint(size_t capacity);
APInt *copy_apint(APInt *num);
APInt *apint_from_string(const char *str);
void free_apint(APInt *num);
void apint_normalize(APInt *num);