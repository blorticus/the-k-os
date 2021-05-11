WITH_DEBUGGING_SYMBOLS = yes
export WITH_DEBUGGING_SYMBOLS

CFLAGS := -Wall -Wextra -g	-std=gnu11

all: FrameBuffer.t TextTerminal.t

FrameBuffer.t: libTestSuite.a FrameBuffer.o Memory.o test_FrameBuffer.c
	$(CC) $(CFLAGS) -o FrameBuffer.t ../FrameBuffer.o ../Memory.o test_FrameBuffer.c -I../include -I../TestSuite -L../TestSuite -lTestSuite

TextTerminal.t: libTestSuite.a FrameBuffer.o TextTerminal.o Memory.o test_TextTerminal.c
	$(CC) $(CFLAGS) -o TextTerminal.t ../FrameBuffer.o ../TextTerminal.o ../Memory.o test_TextTerminal.c -I../include -I../TestSuite -L../TestSuite -lTestSuite


libTestSuite.a:
	$(MAKE) -C ../TestSuite libTestSuite.a

FrameBuffer.o:
	$(MAKE) -C .. FrameBuffer.o

TextTerminal.o:
	$(MAKE) -C .. TextTerminal.o

Memory.o:
	$(MAKE) -C .. Memory.o

.PHONY: clean

clean:
	rm -f *.t *.o
