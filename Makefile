CC=       	gcc
CFLAGS= 	-g -gdwarf-2 -std=gnu99 -Wall -lmavalloc
CLFLAGS= 	-g -gdwarf-2 -std=gnu99 -Wall 
LDFLAGS=
LIBRARIES=      lib/libmavalloc.a

all:   unit_test 

mytest: main_kate.o libmavalloc.a
	gcc -o kate_test -g main_kate.o -L. -lmavalloc

unit_test: main.o libmavalloc.a
	gcc -o unit_test -g main.o -L. -lmavalloc

main.o: main.c
	gcc -O -g -c main.c 

main_kate.o: main_kate.c
	gcc -O -g -c main_kate.c

mavalloc.o: mavalloc.c
	gcc -O -g -c mavalloc.c

libmavalloc.a: mavalloc.o
	ar rcs libmavalloc.a mavalloc.o

clean:
	rm -f *.o *.a unit_test

.PHONY: all clean
