.PHONY: all clean

all: libTestSuite.a main

libTestSuite.a: TestSuite.o
	ar rcs libTestSuite.a TestSuite.o

TestSuite.o: TestSuite.c TestSuite.h
	gcc -g -o TestSuite.o -c TestSuite.c -I. -static

main: TestSuite.o main.c
	gcc -g -o main main.c TestSuite.o -I. -static

clean:
	rm -f *.a *.o main
