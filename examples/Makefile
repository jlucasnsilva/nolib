CC=clang
CFLAGS= -O0 -g -std=c99 -pedantic -Wall -Wno-initializer-overrides
LDFLAGS= -lSDL2 -lSDL2_image -lSDL2_ttf

all: test space

test:
	$(CC) $(CFLAGS) $(LDFLAGS) -o test.bin test.c

space:
	$(CC) $(CFLAGS) $(LDFLAGS) -o space_invaders.bin space_invaders.c

animation:
	$(CC) $(CFLAGS) $(LDFLAGS) -o animation.bin animation.c

moving:
	$(CC) $(CFLAGS) $(LDFLAGS) -o moving.bin moving.c
