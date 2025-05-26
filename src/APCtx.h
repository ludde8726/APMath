#pragma once

#include <stdint.h>

struct APContext {
    uint32_t precision; // Global eval precision, will be accessed by all ops. The ops will create a result with the capacity of precision.
};

extern struct APContext ctx;

uint32_t ap_get_precision(void);
void ap_set_precision(uint32_t prec);
