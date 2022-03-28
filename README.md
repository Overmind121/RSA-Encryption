# RSA-Encryption
This is a program that will perform RSA encryption and decryption on text files. It first generates the public and private keys using the keygen file, then encrypts a text file using the public key, and lastly decrypts that encrypted file with the prviate key.
## Building
In order to build the program you need to run this command:

$ make clean
$ make all


## Running
To run this program:

### To generate keys
$./keygen [-v] [-h] [-b bits] [-i iters] [-n pbfile] [-d pvfile] [-s seed] 

OPTIONS
*  -v  Enable verbose printing.
*  -h  Program usage and help.
*  -b: specifies the minimum bits needed for the public modulus n.
*  -i: specifies the number of Miller-Rabin iterations for testing primes (default: 50)
*  -n pbfile: specifies the public key file (default: rsa.pub).
*  -d pvfile: specifies the private key file (default: rsa.priv).
*  -s: specifies the random seed for the random state initialization (default: the seconds since the UNIX epoch, given by time(NULL)).

### To encrypt
$./encrypt [-v] [-h] [-i infile] [-o outfile] [-n pbfile]

OPTIONS
*  -v         Enable verbose printing.
*  -h         Program usage and help.
*  -i infile  Input text file (default: stdin)
*  -o outfile Output encrypted file  (default: stdout)
*  -n pbfile  public key(default: rsa.pub)

### To decrypt
$./decrypt [-v] [-h] [-i infile] [-o outfile] [-n pbfile]

OPTIONS
*  -v         Enable verbose printing.
*  -h         Program usage and help.
*  -i infile  Input encrypted file (default: stdin)
*  -o outfile Output dencrypted text file  (default: stdout)
*  -n pvfile  private key(default: rsa.priv)

## Bugs and Errors
There are no bugs or errros that you should be worried about.
