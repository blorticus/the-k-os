WITH_DEBUGGING_SYMBOLS = yes
export WITH_DEBUGGING_SYMBOLS

 CFLAGS := -Wall -Wextra -g	-std=gnu11

FrameBuffer.t: libTestSuite.a FrameBuffer.o Memory.o test_FrameBuffer.c
	$(CC) $(CFLAGS) -o FrameBuffer.t ../FrameBuffer.o ../Memory.o test_FrameBuffer.c -I../include -I../TestSuite -L../TestSuite -lTestSuite

libTestSuite.a:
	$(MAKE) -C ../TestSuite libTestSuite.a

FrameBuffer.o:
	$(MAKE) -C .. FrameBuffer.o

Memory.o:
	$(MAKE) -C .. Memory.o

.PHONY: clean

clean:
	rm -f *.t *.o