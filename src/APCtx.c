#include "APCtx.h"

struct APContext ctx = { 16 };

void ap_set_precision(uint32_t prec) {
    ctx.precision = prec;
}