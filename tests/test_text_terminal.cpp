#include <TestSuite.h>
#include <video/fb/cpp-text-terminal.h>
#include <sys/types.h>

const uint32 TEST_BUF_LEN = 8 * 8 * 16 * 8;

int main( void ) {
    uint32 buf[TEST_BUF_LEN];

    TestSuite* t = new TestSuite( "FrameBuffer::TextTerminal" );

    FrameBuffer::TextTerminal* fb = new FrameBuffer::TextTerminal( buf, 8, 8, 0x12341234, 0xabcdabcd );
    fb->clear();
    
    t->assertArrayRepeats( buf, 0xabcdabcd, TEST_BUF_LEN, "Test 1: clear()" );

    return t->doneTesting();
}
