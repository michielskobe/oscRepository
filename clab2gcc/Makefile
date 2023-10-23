# Makefile for debug_challenge
# author: TD

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -g

clean:
	rm -f debugchallenge.o

debugchallenge: cstring.c
	$(CC) $(CFLAGS) -o debugchallenge.o cstring.c
	#./debugchallenge.o

memtest: debugchallenge
	valgrind --leak-check=yes ./debugchallenge.o
	#./debugchallenge.o

all: debugchallenge
	#./debugchallenge.o

memtest: all	valgrind --leak-check=yes ./cstring

# launch with CLION target executable configured or uncomment line
