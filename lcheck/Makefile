INCLUDES := -I.
CC_FLAGS := -Wall -g 
CXX_FLAGS := -Wall -g 


# TARGET: build lcheck.a library
liblcheck.a: lcheck.c lcheck.h
	$(CC) $(CC_FLAGS) $(INCLUDES) -o lcheck.o -c lcheck.c
	$(AR) rcs liblcheck.a lcheck.o

TestSuite.o: TestSuite.cpp TestSuite.h
	$(CXX) $(CXX_FLAGS) -o $@ -c $< $(INCLUDES)

libtestsuite.a: TestSuite.o
	$(AR) rvs libtestsuite.a TestSuite.o


.PHONY: clean
clean:
	rm -rf *.a *.o
