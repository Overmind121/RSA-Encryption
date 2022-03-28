#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "randstate.h"
#include "rsa.h"
#include "numtheory.h"
#include "set.h"

typedef enum { BITS, ITERS, VERBOSE, HELP, SEED } commands;
int main(int argc, char **argv) {
    // initializing vars
    Set set_commands = empty_set();
    uint64_t seed = time(NULL);
    uint64_t iters = 50;
    uint64_t bits = 256;
    size_t bit_count = 0;
    mpz_t m, c, e, n, s, p, q, d, user;
    mpz_inits(m, c, e, n, s, p, q, d, user, NULL);
    char *username = getenv("USER");
    FILE *public = fopen("rsa.pub", "w+");
    FILE *private = fopen("rsa.priv", "w+");

    // Opting user for input
    int opt = 0;
    while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) {
        switch (opt) {
        case 'b':
            bits = (uint64_t) optarg;
            set_commands = insert_set(BITS, set_commands);
            break;
        case 'i':
            iters = (uint64_t) optarg;
            set_commands = insert_set(ITERS, set_commands);
            break;
        case 'n':
        public
            = fopen(optarg, "w+");
            break;
        case 'd':
        private
            = fopen(optarg, "w+");
            break;
        case 's':
            seed = (uint64_t) optarg;
            set_commands = insert_set(SEED, set_commands);
            break;
        case 'v': set_commands = insert_set(VERBOSE, set_commands); break;
        case 'h': set_commands = insert_set(HELP, set_commands); break;
        }
    }

    // If help is asked
    if (member_set(HELP, set_commands)) {
        printf("SYNOPSIS\n");
        printf("   Generates an RSA public/private key pair.\n\n");
        printf("USAGE\n");
        printf("   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -b bits         Minimum bits needed for public key n (default: 256).\n");
        printf("   -i confidence   Miller-Rabin iterations for testing primes (default: 50).\n");
        printf("   -n pbfile       Public key file (default: rsa.pub).\n");
        printf("   -d pvfile       Private key file (default: rsa.priv).\n");
        printf("   -s seed         Random seed for testing.\n");

        fclose(public);
        fclose(private);
        mpz_clears(m, c, e, n, s, p, q, d, user, NULL);
        return 0;
    }

    // Setting file permissions
    fchmod(fileno(private), 0600);

    // Setting the seed for rand_state and srandom
    randstate_init(seed);
    srandom(seed);

    // Making the public keys
    rsa_make_pub(p, q, n, e, bits, iters);
    rsa_make_priv(d, e, p, q);

    // Converting username to mpz bits
    mpz_set_str(user, username, 62);
    rsa_sign(s, user, d, n);

    // Writing the public keys onto the public key file and same for private
    rsa_write_pub(n, e, s, username, public);
    rsa_write_priv(n, d, private);

    // If verbose is asked
    if (member_set(VERBOSE, set_commands)) {
        printf("user = %s\n", username);

        bit_count = mpz_sizeinbase(s, 2);
        gmp_printf("s (%zu) = %Zd\n", bit_count, s);

        bit_count = mpz_sizeinbase(p, 2);
        gmp_printf("p (%zu) = %Zd\n", bit_count, p);

        bit_count = mpz_sizeinbase(q, 2);
        gmp_printf("q (%zu) = %Zd\n", bit_count, q);

        bit_count = mpz_sizeinbase(n, 2);
        gmp_printf("n (%zu) = %Zd\n", bit_count, n);

        bit_count = mpz_sizeinbase(e, 2);
        gmp_printf("e (%zu) = %Zd\n", bit_count, e);

        bit_count = mpz_sizeinbase(d, 2);
        gmp_printf("d (%zu) = %Zd\n", bit_count, d);
    }

    // Clearing randstate, mpz, and closing files
    randstate_clear();
    fclose(public);
    fclose(private);

    mpz_clears(m, c, e, n, s, p, q, d, user, NULL);

    return 0;
}
