

CC = gcc

IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii

CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

LDLIBS = -l40locality -lnetpbm -lcii40 -lm -lrt

INCLUDES = $(shell echo *.h)

############### Rules ###############

all: ppmdiff


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

# a2test: a2test.o uarray2b.o uarray2.o a2plain.o
# 	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)



ppmdiff: ppmdiff.o uarray2b.o uarray2.o a2methods.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)



clean:
	rm -f ppmdiff *.o
