#include <TestSuite.h>
#include <YOURLIBRARY.h>
#include <sys/types.h>

int main( void ) {
    TestSuite* t = new TestSuite( "YOUR_TEST" );

    FrameBuffer::Font f = FrameBuffer::Font();

    t->failUnless( 1 == 0, "testname" );

    return t->doneTesting();
}
