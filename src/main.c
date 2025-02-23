#include <stdint.h>

#include "APCtx.h"
#include "APIntOps.h"
#include "APNumber.h"

int main() {
    // Set precision to 5 (we don't need more)
    ctx.precision = 5;
    // Create two ints
    APInt *small_int = apint_from_int(1000);
    APInt *small_int2 = apint_copy(small_int);
    for (uint32_t i = 0; i < small_int->capacity; i++) printf("%d", small_int->digits[i]);
    printf("\n");
    for (uint32_t i = 0; i < small_int2->capacity; i++) printf("%d", small_int2->digits[i]);
    printf("\n");
    APInt *sum_int = apint_add(small_int, small_int2);
    for (uint32_t i = 0; i < sum_int->capacity; i++) printf("%d", sum_int->digits[i]);
    apint_free(small_int);
    apint_free(small_int2);
    apint_free(sum_int);
    return 0;
}