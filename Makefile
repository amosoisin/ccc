CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ccc: $(OBJS)
	$(CC) -o ccc $(OBJS) $(LDFLAGS)

$(OBJS): ccc.h

test: ccc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean
