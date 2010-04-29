#include <lcheck.h>
#include <koshlib.h>
#include <string.h>


/* Parses a command "word" (no whitespace permitted) and determines whether it is memory location or a register name (or neither).
 *  * Memory locations must begin with 0x and must be hex digits only.  Registers must be Intel canonical format names for 8-, 16- or
 *   * 32-bit registers in any case (even mixed case).  'word' must be NULL terminated.  If 'word' is not null-terminated or contains
 *    * whitespace, results are not defined.  If 'word' is a memory location, its value is converted to 'memaddr' type and fed to
 *     * instruction.memory_location (and instruction.reg is set to -1).  If it appears to be a register name, instruction.reg is set to
 *      * the enum 'register_name' value and instruction.memory_location is set to 0.  If 'word' appears to be invalid, instruction.error
 *       * is set and zero is returned.  Otherwise, non-zero is returned.  If 'word' is a memory location, and it specifies a value outside
 *        * the bounds of the type 'memaddr', instruction.error will be set and zero will be returned.
 *         */
//int extract_reg_or_mem( char* word, kosh_instruction* instruction ) {


void _test_for_reg( lcheck_suite* s, const char* test_value, register_name rn ) {
    kosh_instruction i;

    fail_unless( s, extract_reg_or_mem( (char*)test_value, &i ),
                 __strcat_realloc( "extract_reg_or_mem() returns false value for ", test_value ) );

    fail_unless( s, i.reg == rn,
                 __strcat_realloc( "extract_reg_or_mem() fails compare for", test_value ) );
}


int main( void ) {
    lcheck_suite* s;

    /* could conceivably use the same kosh_instruction repeatedly, but I want to start with a clean copy each time */
    kosh_instruction i[20];  /* one for each test that compares value of instruction struct passed to extract_reg_or_mem() */
    kosh_instruction j[20];  /* ... same, but for tests that should fail */

    s = create_suite( "kosh_extract_reg_or_mem" );

    fail_unless( s, extract_reg_or_mem( "0x0", &i[0] ),
                 "extract_reg_or_mem() returns false value for 0x0, which is valid" );

    fail_unless( s, i[0].memory_location == 0,
                 "extract_reg_or_mem() failed to set memaddr to 0 when value is 0x0" );

    fail_unless( s, extract_reg_or_mem( "0x1", &i[1] ),
                 "extract_reg_or_mem() returns false value for 0x1, which is valid" );

    fail_unless( s, i[1].memory_location == 1,
                 "extract_reg_or_mem() failed to set memaddr to 1 when value is 0x1" );

    fail_unless( s, extract_reg_or_mem( "0xfffffffe", &i[2] ),
                 "extract_reg_or_mem() returns false value for 0xfffffffe, which is valid" );

    fail_unless( s, i[2].memory_location == 0xfffffffe,
                 "extract_reg_or_mem() failed to set memaddr to 0xfffffffe when value is 0xfffffffe" );

    fail_unless( s, extract_reg_or_mem( "0xA231b", &i[3] ),
                 "extract_reg_or_mem() returns false value for 0xA231b, which is valid" );

    fail_unless( s, i[3].memory_location == 0xa231b,
                 "extract_reg_or_mem() failed to set memaddr to 0xa231b when value is 0xA231b" );

    fail_unless( s, extract_reg_or_mem( "0x19056", &i[4] ),
                 "extract_reg_or_mem() returns false value for 0x19056, which is valid" );

    fail_unless( s, i[4].memory_location == 0x19056,
                 "extract_reg_or_mem() failed to set memaddr to 0x19056 when value is 0x19056" );

    fail_unless( s, extract_reg_or_mem( "0xabddecfc", &i[5] ),
                 "extract_reg_or_mem() returns false value for 0xabddecfc, which is valid" );

    fail_unless( s, i[5].memory_location == 0xabddecfc,
                 "extract_reg_or_mem() failed to set memaddr to 0xabddecfc when value is 0xabddecfc" );

    fail_unless( s, extract_reg_or_mem( "0x00135a0", &i[6] ),
                 "extract_reg_or_mem() returns false value for 0x00135a0, which is valid" );

    fail_unless( s, i[6].memory_location == 0x135a0,
                 "extract_reg_or_mem() failed to set memaddr to 0x135a0 when value is 0x00135a0" );

    _test_for_reg( s, "al", AL );
    _test_for_reg( s, "AH", AH );
    _test_for_reg( s, "Bl", BL );
    _test_for_reg( s, "bH", BH );
    _test_for_reg( s, "cl", CL );
    _test_for_reg( s, "CH", CH );
    _test_for_reg( s, "Dl", DL );
    _test_for_reg( s, "dH", DH );
    _test_for_reg( s, "ax", AX );
    _test_for_reg( s, "BX", BX );
    _test_for_reg( s, "cX", CX );
    _test_for_reg( s, "DX", DX );
    _test_for_reg( s, "EAX", EAX );
    _test_for_reg( s, "ebx", EBX );
    _test_for_reg( s, "eDx", EDX );
    _test_for_reg( s, "EcX", ECX );
    _test_for_reg( s, "eDX", EDX );
    _test_for_reg( s, "ss", SS );
    _test_for_reg( s, "CS", CS );
    _test_for_reg( s, "eS", ES );
    _test_for_reg( s, "fS", FS );
    _test_for_reg( s, "Gs", GS );
    _test_for_reg( s, "Si", SI );
    _test_for_reg( s, "DI", DI );
    _test_for_reg( s, "esI", ESI );
    _test_for_reg( s, "Edi", EDI );
    _test_for_reg( s, "bP", BP );
    _test_for_reg( s, "EBP", EBP );
    _test_for_reg( s, "sp", SP );
    _test_for_reg( s, "EsP", ESP );
    _test_for_reg( s, "EflAgs", EFLAGS );


    fail_unless( s, !extract_reg_or_mem( "", &j[0] ),
                 "extract_reg_or_mem() returns true value when passed empty string" );

    fail_unless( s, j[0].error != NULL && __strlen( j[0].error ),
                 "extract_reg_or_mem() does no set error when passed empty string" );

    fail_unless( s, !extract_reg_or_mem( "0x123t", &j[1] ),
                 "extract_reg_or_mem() returns true value when passed 0x123t" );

    fail_unless( s, j[1].error != NULL && __strlen( j[1].error ),
                 "extract_reg_or_mem() does no set error when passed 0x123t" );

    return conclude_suite( s );
}
