#include <stdio.h>
#include <gmp.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

gmp_randstate_t state;

void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    return;
}

void randstate_clear(void) {
    gmp_randclear(state);
    return;
}
