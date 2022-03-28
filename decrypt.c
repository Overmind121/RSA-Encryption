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
    // Initializing the variables
    Set set_commands = empty_set();
    mpz_t n, d;
    mpz_inits(n, d, NULL);
    FILE *input = stdin;
    FILE *output = stdout;
    size_t bit_count = 0;
    FILE *private = fopen("rsa.priv", "r");

    // Opting the uwer for an input
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
        private
            = fopen(optarg, "r");
            break;
        case 'v': set_commands = insert_set(VERBOSE, set_commands); break;
        case 'h': set_commands = insert_set(HELP, set_commands); break;
        }
    }

    // If help is asked
    if (member_set(HELP, set_commands)) {
        printf("SYNOPSIS\n");
        printf("   Decrypts data using RSA decryption.\n");
        printf("   Encrypted data is encrypted by the encrypt program.\n\n");
        printf("USAGE\n");
        printf("   ./decrypt [-hv] [-i infile] [-o outfile] -n privkey\n\n");
        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -i infile       Input file of data to encrypt (default: stdin).\n");
        printf("   -o outfile      Output file for decrypted data (default: stdout).\n");
        printf("   -n pvfile       Private key file (default: rsa.priv).\n");

        fclose(input);
        fclose(output);
        fclose(private);
        mpz_clears(n, d, NULL);
        return 0;
    }

    // Reading the private key
    rsa_read_priv(n, d, private);

    // Decrypting the file
    rsa_decrypt_file(input, output, n, d);

    // If verbose is asked
    if (member_set(VERBOSE, set_commands)) {
        bit_count = mpz_sizeinbase(n, 2);
        gmp_printf("n (%zu) = %Zd\n", bit_count, n);

        bit_count = mpz_sizeinbase(d, 2);
        gmp_printf("d (%zu) = %Zd\n", bit_count, d);
    }

    // Closing the file and mpz_t
    fclose(input);
    fclose(output);
    fclose(private);
    mpz_clears(n, d, NULL);
    return 0;
}
