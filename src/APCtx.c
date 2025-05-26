#include "APCtx.h"

struct APContext ctx = { 16 };

uint32_t ap_get_precision(void) {
    return ctx.precision;
}

void ap_set_precision(uint32_t precision) {
    ctx.precision = precision;
}
