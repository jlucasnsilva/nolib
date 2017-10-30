CC=clang
CFLAGS= -O0 -g -std=c99 -pedantic -Wall -Wno-initializer-overrides
LDFLAGS= -lSDL2 -lSDL2_image -lSDL2_ttf

all: test

test:
	$(CC) $(CFLAGS) $(LDFLAGS) -o test test.c
