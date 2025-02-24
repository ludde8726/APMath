#pragma once

#include <stdint.h>

struct APContext {
    uint32_t precision; // Global eval precision, will be accessed by all ops. The ops will create a result with the capacity of precision.
};

extern struct APContext ctx;

inline uint32_t ap_get_precision(void) {
    return ctx.precision;
}

void ap_set_precision(uint32_t prec);