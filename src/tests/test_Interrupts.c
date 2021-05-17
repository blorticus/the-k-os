#include <TestSuite.h>
#include <Interrupts.h>
#include <stdlib.h>

int main( void )
{
    TestSuite suite = CreateTestSuite( "Interrupts" );

    InterruptDescriptor d = calloc( 1, sizeof( InterruptDescriptor_t ) );

    Error e = PopulateInterruptDescriptor( d, (void*)0x0a0bc0e1abcd0012, CPL0, 0 );

    suite->AssertEquals->Int32( suite, NoError, e, "First PopulateInterruptDescriptor()" );
    suite->AssertEquals->Uint16( suite, 0x0012, d->HandlerAddressLow, "HandlerAddressLow for first PopulateInterruptDescriptor()" );
    suite->AssertEquals->Uint16(suite, 0xabcd, d->HandlerAddressMid, "HandlerAddressMid for first PopulateInterruptDescriptor()");
    suite->AssertEquals->Uint32(suite, 0x0a0bc0e1, d->HandlerAddressHigh, "HandlerAddressHigh for first PopulateInterruptDescriptor()");
    suite->AssertEquals->Uint16( suite, 40, d->CodeSegementSelector, "CodeSegmentSelector for first PopulateInterruptDescriptor()" );
    suite->AssertEquals->Uint8( suite, 0x0, d->InterruptStackTable, "InterruptStackTable for first PopulateInterruptDescriptor()" );
    suite->AssertEquals->Uint8( suite,0x8e, d->PrivilegeLevelFlagsAndType, "PrivilegeLevelFlagsAndType for first PopulateInterruptDescriptor()" );

    suite->Done(suite);
}