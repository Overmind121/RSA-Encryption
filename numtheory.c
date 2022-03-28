#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "randstate.h"

// Cited from Prof Long's manual
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t t, b_temp, a_temp;
    mpz_inits(t, b_temp, a_temp, NULL);
    mpz_set(b_temp, b);
    mpz_set(a_temp, a);

    while (mpz_cmp_ui(b_temp, 0) != 0) {
        mpz_set(t, b_temp);
        mpz_mod(b_temp, a_temp, b_temp);
        mpz_set(a_temp, t);
    }
    mpz_set(d, a_temp);
    mpz_clears(t, b_temp, a_temp, NULL);
}

// Cited from professor Long's Manual
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t q, r, r_prime, r_temp, t, t_prime, t_temp, mul_temp;
    mpz_inits(q, r, r_prime, r_temp, t, t_prime, t_temp, mul_temp, NULL);

    mpz_set(r, n);
    mpz_set(r_prime, a);

    mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);

    while (mpz_cmp_ui(r_prime, 0) != 0) {
        mpz_fdiv_q(q, r, r_prime);

        mpz_set(r_temp, r);
        mpz_set(r, r_prime);
        mpz_mul(mul_temp, q, r_prime);
        mpz_sub(r_prime, r_temp, mul_temp);

        mpz_set(t_temp, t);
        mpz_set(t, t_prime);
        mpz_mul(mul_temp, q, t_prime);
        mpz_sub(t_prime, t_temp, mul_temp);
    }

    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(i, 0);
    } else if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
        mpz_set(i, t);
    } else {
        mpz_set(i, t);
    }
    mpz_clears(q, r, r_prime, r_temp, t, t_prime, t_temp, mul_temp, NULL);
}

// Cited form professor Long's manual
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t v, p, expo_mod, expo_temp, mod_temp;
    mpz_inits(v, p, expo_mod, expo_temp, mod_temp, NULL);

    mpz_set(expo_temp, exponent);
    mpz_set(mod_temp, modulus);
    mpz_set(p, base);
    mpz_set_ui(v, 1);

    while (mpz_cmp_ui(expo_temp, 0) > 0) {
        mpz_mod_ui(expo_mod, expo_temp, 2);

        if (mpz_cmp_ui(expo_mod, 1) == 0) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, mod_temp);
        }

        mpz_mul(p, p, p);
        mpz_mod(p, p, mod_temp);

        mpz_fdiv_q_ui(expo_temp, expo_temp, 2);
    }
    mpz_set(out, v);
    mpz_clears(v, p, expo_mod, expo_temp, mod_temp, NULL);

    return;
}

// Cited from Professor Long's manual
bool is_prime(mpz_t n, uint64_t iters) {
    mpz_t a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num, random_set, sub_temp,
        sub_temps, base_temp;
    mpz_inits(a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num, random_set, sub_temp,
        sub_temps, base_temp, NULL);

    mpz_mod_ui(mod_temp, n, 2);
    if (mpz_cmp_ui(mod_temp, 0) == 0 && mpz_cmp_ui(n, 2) != 0) {
        mpz_clears(a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num, random_set,
            sub_temp, sub_temps, base_temp, NULL);
        return false;
    }
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        mpz_clears(a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num, random_set,
            sub_temp, sub_temps, base_temp, NULL);
        return true;
    }
    if (mpz_cmp_ui(n, 2) < 0) {
        mpz_clears(a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num, random_set,
            sub_temp, sub_temps, base_temp, NULL);
        return false;
    }

    mpz_sub_ui(r_temp, n, 1);
    mpz_mod_ui(mod_temp, r_temp, 2);
    mpz_set_ui(base_temp, 2);

    while (mpz_cmp_ui(mod_temp, 0) == 0) {
        mpz_add_ui(s_temp, s_temp, 1);
        mpz_divexact_ui(r_temp, r_temp, 2);
        mpz_mod_ui(mod_temp, r_temp, 2);
    }

    for (uint64_t i = 1; i <= iters; i++) {

        mpz_sub_ui(random_set, n, 3);
        mpz_urandomm(a_temp, state, random_set);
        mpz_add_ui(a_temp, a_temp, 2);

        pow_mod(y_temp, a_temp, r_temp, n);

        // n-1
        mpz_sub_ui(sub_temp, n, 1);
        if (mpz_cmp_ui(y_temp, 1) != 0 && mpz_cmp(y_temp, sub_temp) != 0) {
            mpz_set_ui(j_temp, 1);

            // s-1
            mpz_sub_ui(sub_temps, s_temp, 1);
            while ((mpz_cmp(j_temp, sub_temps) < 0 || mpz_cmp(j_temp, sub_temps) == 0)
                   && (mpz_cmp(y_temp, sub_temp) != 0)) {
                pow_mod(y_temp, y_temp, base_temp, n);
                if (mpz_cmp_ui(y_temp, 1) == 0) {
                    mpz_clears(a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num,
                        random_set, sub_temp, sub_temps, base_temp, NULL);
                    return false;
                }
                mpz_add_ui(j_temp, j_temp, 1);
            }

            if (mpz_cmp(y_temp, sub_temp) != 0) {
                mpz_clears(a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num, random_set,
                    sub_temp, sub_temps, base_temp, NULL);
                return false;
            }
        }
    }

    mpz_clears(a_temp, y_temp, j_temp, r_temp, s_temp, mod_temp, random_num, random_set, sub_temp,
        sub_temps, base_temp, NULL);

    return true;
}

// Cited from Prof. Long's random_prime()
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t base;
    mpz_inits(base, NULL);
    size_t num_base = mpz_sizeinbase(p, 2);
    while ((!is_prime(p, iters)) || num_base < bits) {
        mpz_urandomb(p, state, bits);
        mpz_ui_pow_ui(base, 2, bits);
        mpz_add(p, p, base);
        num_base = mpz_sizeinbase(p, 2);
    }
    mpz_clears(base, NULL);
    return;
}
