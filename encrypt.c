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
#include "numtheory.h"
#include "rsa.h"
#include "set.h"

typedef enum { VERBOSE, HELP, SEED, INPUT, OUTPUT } commands;
int main(int argc, char **argv) {
    // Initializing variables
    Set set_commands = empty_set();
    mpz_t s, n, e, m, user;
    mpz_inits(s, n, e, m, user, NULL);
    size_t bit_count = 0;
    FILE *input = stdin;
    FILE *output = stdout;
    FILE *public = fopen("rsa.pub", "r");
    char username[512];

    // Asking for input
    int opt = 0;
    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (opt) {
        case 'i':
            input = fopen(optarg, "r");
            set_commands = insert_set(INPUT, set_commands);
            break;
        case 'o':
            output = fopen(optarg, "w");
            set_commands = insert_set(OUTPUT, set_commands);
            break;
        case 'n':
        public
            = fopen(optarg, "r");
            break;
        case 'v': set_commands = insert_set(VERBOSE, set_commands); break;
        case 'h': set_commands = insert_set(HELP, set_commands); break;
        default:
            printf("Input and output are not specified.\n");
            fclose(input);
            fclose(output);
            fclose(public);
            mpz_clears(s, n, e, m, user, NULL);
            return 0;
        }
    }

    // If help is asked
    if (member_set(HELP, set_commands)) {
        printf("SYNOPSIS\n");
        printf("   Encrypts data using RSA encryption.\n");
        printf("   Encrypted data is decrypted by the decrypt program.\n\n");
        printf("USAGE\n");
        printf("   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -i infile       Input file of data to encrypt (default: stdin).\n");
        printf("   -o output   Output file for encrypted data (default: stdout).\n");
        printf("   -n pbfile       Public key file (default: rsa.pub).\n");

        fclose(input);
        fclose(output);
        fclose(public);
        mpz_clears(s, n, e, m, user, NULL);
        return 0;
    }

    // Reads the public file
    rsa_read_pub(n, e, s, username, public);

    // Converts username to bits
    mpz_set_str(user, username, 62);

    // Verifies signature
    if (!rsa_verify(user, s, e, n)) {
        mpz_clears(s, n, e, m, user, NULL);
        return 0;
    }

    // Encrypts the file
    rsa_encrypt_file(input, output, n, e);

    // If verbose is asked
    if (member_set(VERBOSE, set_commands)) {
        printf("user = %s\n", username);

        bit_count = mpz_sizeinbase(s, 2);
        gmp_printf("s (%zu) = %Zd\n", bit_count, s);

        bit_count = mpz_sizeinbase(n, 2);
        gmp_printf("n (%zu) = %Zd\n", bit_count, n);

        bit_count = mpz_sizeinbase(e, 2);
        gmp_printf("e (%zu) = %Zd\n", bit_count, e);
    }

    // Close the files and clear mpz_t
    fclose(input);
    fclose(output);
    fclose(public);
    mpz_clears(s, n, e, m, user, NULL);

    return 0;
}
