# Makefile for ex3_q1 spring 2023B

CFLAGS   = -Wall -std=c99 -pthread
LDFLAGS  = -lm -pthread
CC       = gcc
ECHO     = @echo "going to build target $@ (dependent on $^)"


PROG = ex3_q1 

all: $(PROG) test

$(PROG): ex3_q1.c ex3_q1.h ex3_q1_given.h ex3_q1_given.c
	$(ECHO)
	gcc ex3_q1.c ex3_q1_given.c $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm -vf *.o ${PROG1}

test:
	./ex3_q1 gr_1.txt  gr_2.txt  gr_3.txt  gr_4.txt

