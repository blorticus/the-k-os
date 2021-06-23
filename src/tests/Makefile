WITH_DEBUGGING_SYMBOLS = yes
TESTING = yes
export WITH_DEBUGGING_SYMBOLS TESTING

CFLAGS := -Wall -Wextra -g	-std=gnu11

TESTS := FrameBuffer.t TextTerminal.t String.t Interrupts.t Cpu.t

.DEFAULT_GOAL := runall

all: $(TESTS)

FrameBuffer.t: libTestSuite.a FrameBuffer.o Memory.o test_FrameBuffer.c
	$(CC) $(CFLAGS) -o FrameBuffer.t ../FrameBuffer.o ../Memory.o test_FrameBuffer.c -I../include -I../TestSuite -L../TestSuite -lTestSuite

TextTerminal.t: libTestSuite.a FrameBuffer.o TextTerminal.o Memory.o String.o test_TextTerminal.c
	$(CC) $(CFLAGS) -o TextTerminal.t ../FrameBuffer.o ../TextTerminal.o ../Memory.o ../String.o test_TextTerminal.c -I../include -I../TestSuite -L../TestSuite -lTestSuite

String.t: libTestSuite.a String.o test_String.c
	$(CC) $(CFLAGS) -o String.t ../String.o test_String.c -I../include -I../TestSuite -L../TestSuite -lTestSuite

Interrupts.t: libTestSuite.a Interrupts.o test_Interrupts.c String.o
	$(CC) $(CFLAGS) -o Interrupts.t ../Interrupts.o ../String.o ../IOPorts.o test_Interrupts.c -I../include -I../TestSuite -L../TestSuite -lTestSuite

Cpu.t: libTestSuite.a Cpu.o test_Cpu.c
	$(CC) $(CFLAGS) -o Cpu.t ../Cpu.o test_Cpu.c -I../include -I../TestSuite -L../TestSuite -lTestSuite

libTestSuite.a:
	$(MAKE) -C ../TestSuite libTestSuite.a

FrameBuffer.o:
	$(MAKE) -C .. FrameBuffer.o

TextTerminal.o:
	$(MAKE) -C .. TextTerminal.o

Memory.o:
	$(MAKE) -C .. Memory.o

String.o:
	$(MAKE) -C .. String.o

Interrupts.o:
	$(MAKE) -C .. Interrupts.o

Cpu.o:
	$(MAKE) -C .. Cpu.o


.PHONY: clean cleanall runall

runall: all
	./FrameBuffer.t
	./TextTerminal.t
	./String.t
	./Interrupts.t
	./Cpu.t

clean:
	rm -f *.t *.o

cleanall: clean
	$(MAKE) -C .. clean
