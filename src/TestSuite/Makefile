.PHONY: all clean

all: TestSuite.o main

TestSuite.o: TestSuite.c TestSuite.h
	gcc -g -o TestSuite.o -c TestSuite.c -I. -static

main: TestSuite.o main.c
	gcc -g -o main main.c TestSuite.o -I. -static

clean:
	rm *.o main
