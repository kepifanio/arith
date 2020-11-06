# Makefile for arith (Comp 40 Assignment 4)
#
# Includes build rules for ppmdiff and 40image



CC = gcc # The compiler being used

IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii

CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

LDLIBS = -larith40 -l40locality -lnetpbm -lcii40 -lm -lrt

INCLUDES = $(shell echo *.h)

############### Rules ###############

all: ppmdiff 40image

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

ppmdiff: ppmdiff.o a2plain.o uarray2.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

40image: 40image.o compress40.o uarray2.o a2plain.o input_output.o \
		testing.o compress_decompress.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f ppmdiff 40image *.o
