CC = clang
CFLAGS = -g -Wall -Wpedantic -Werror -Wextra $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp) -lm

all: keygen encrypt decrypt

keygen: keygen.o numtheory.o randstate.o rsa.o
	$(CC) -o keygen keygen.o numtheory.o randstate.o rsa.o $(LFLAGS)

keygen.o: keygen.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c keygen.c 

encrypt: encrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o encrypt encrypt.o numtheory.o randstate.o rsa.o $(LFLAGS)

encrypt.o: encrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c encrypt.c 

decrypt: decrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o decrypt decrypt.o numtheory.o randstate.o rsa.o $(LFLAGS)

decrypt.o: decrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c decrypt.c 

numtheory.o: numtheory.c
	$(CC) $(CFLAGS) -c numtheory.c

randstate.o: randstate.c
	$(CC) $(CFLAGS) -c randstate.c

rsa.o: rsa.c
	$(CC) $(CFLAGS) -c rsa.c

clean:
	rm -f keygen encrypt decrypt *.o
format:
	clang-format -i -style=file *.[ch]



