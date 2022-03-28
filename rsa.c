#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <gmp.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "randstate.h"
#include "numtheory.h"
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {

    mpz_t low, high, prand_val, bits_lim, qrand_val, gcd_val, totient, temp;
    mpz_inits(low, high, prand_val, bits_lim, qrand_val, gcd_val, totient, temp, NULL);

    uint64_t nbit_low = nbits / 4;
    uint64_t nbit_high = (3 * nbits) / 4;

    uint64_t rand_valp = (random() % (nbit_high - nbit_low + 1)) + nbit_low;
    uint64_t rand_valq = nbits - rand_valp;

    make_prime(p, rand_valp, iters);
    make_prime(q, rand_valq, iters);
    mpz_mul(n, p, q);

    mpz_sub_ui(qrand_val, q, 1);
    mpz_sub_ui(prand_val, p, 1);
    mpz_mul(totient, qrand_val, prand_val);

    mpz_urandomb(e, state, nbits);
    while (mpz_cmp_ui(gcd_val, 1) != 0) {
        mpz_urandomb(e, state, nbits);
        mpz_set(temp, e);
        gcd(gcd_val, totient, temp);
    }

    mpz_clears(low, high, prand_val, bits_lim, qrand_val, gcd_val, totient, temp, NULL);

    return;
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    gmp_fprintf(pbfile, "%s\n", username);
    return;
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fscanf(pbfile, "%s\n", username);

    return;
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t phi, p_phi, q_phi;
    mpz_inits(phi, p_phi, q_phi, NULL);

    mpz_sub_ui(p_phi, p, 1);
    mpz_sub_ui(q_phi, q, 1);

    mpz_mul(phi, p_phi, q_phi);

    mod_inverse(d, e, phi);

    mpz_clears(phi, p_phi, q_phi, NULL);
    return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);

    return;
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);

    return;
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {

    mpz_t m, c;
    mpz_inits(m, c, NULL);
    uint32_t k_val = floor((mpz_sizeinbase(n, 2) - 1) / 8);
    uint8_t *buf = (uint8_t *) calloc(k_val, sizeof(uint8_t));
    buf[0] = 0xFF;
    size_t count;

    while (!feof(infile)) {
        count = fread(buf + 1, sizeof(uint8_t), k_val - 1, infile);
        mpz_import(m, count + 1, 1, sizeof(uint8_t), 1, 0, buf);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    free(buf);
    mpz_clears(m, c, NULL);
    return;
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);
    uint32_t k = floor((mpz_sizeinbase(n, 2) - 1) / 8);
    uint8_t *buf = (uint8_t *) calloc(k, sizeof(uint8_t));

    size_t count;
    while (!feof(infile)) {
        gmp_fscanf(infile, "%Zx\n", c);

        rsa_decrypt(m, c, d, n);
        mpz_export(buf, &count, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(buf + 1, sizeof(uint8_t), count - 1, outfile);
    }
    free(buf);
    mpz_clears(m, c, NULL);

    return;
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    }
    mpz_clears(t);
    return false;
}
