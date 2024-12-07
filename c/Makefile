CFLAGS:=-std=c11 -g -static

chibicc: chibicc.c

test: chibicc
	./test.sh

clean:
	rm -f chibicc *.o *~ tmp*

.PHONY: test clean
