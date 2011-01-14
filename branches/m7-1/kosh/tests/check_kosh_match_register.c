#include <lcheck.h>
#include <kosh/koshlib.h>


int main( void ) {
   lcheck_suite* s;

    s = create_suite( "match_register" );

    fail_unless( s, match_register( "al" ) == AL,
                 "match_register for 'al' does not return AL" );
    fail_unless( s, match_register( "AH" ) == AH,
                 "match_register for 'AH' does not return AH" );
    fail_unless( s, match_register( "Bl" ) == BL,
                 "match_register for 'Bl' does not return BL" );
    fail_unless( s, match_register( "bH" ) == BH,
                 "match_register for 'bH' does not return BH" );
    fail_unless( s, match_register( "Cl" ) == CL,
                 "match_register for 'Cl' does not return CL" );
    fail_unless( s, match_register( "CH" ) == CH,
                 "match_register for 'CH' does not return CH" );
    fail_unless( s, match_register( "dl" ) == DL,
                 "match_register for 'dl' does not return DL" );
    fail_unless( s, match_register( "dh" ) == DH,
                 "match_register for 'dh' does not return DH" );
    fail_unless( s, match_register( "aX" ) == AX,
                 "match_register for 'aX' does not return AX" );
    fail_unless( s, match_register( "bX" ) == BX,
                 "match_register for 'bX' does not return BX" );
    fail_unless( s, match_register( "Cx" ) == CX,
                 "match_register for 'Cx' does not return CX" );
    fail_unless( s, match_register( "DX" ) == DX,
                 "match_register for 'DX' does not return DX" );
    fail_unless( s, match_register( "Eax" ) == EAX,
                 "match_register for 'Eax' does not return EAX" );
    fail_unless( s, match_register( "eBx" ) == EBX,
                 "match_register for 'eBx' does not return EBX" );
    fail_unless( s, match_register( "ecX" ) == ECX,
                 "match_register for 'ecX' does not return ECX" );
    fail_unless( s, match_register( "EDx" ) == EDX,
                 "match_register for 'EDx' does not return EDX" );
    fail_unless( s, match_register( "ss" ) == SS,
                 "match_register for 'ss' does not return SS" );
    fail_unless( s, match_register( "cs" ) == CS,
                 "match_register for 'cs' does not return CS" );
    fail_unless( s, match_register( "ds" ) == DS,
                 "match_register for 'ds' does not return DS" );
    fail_unless( s, match_register( "es" ) == ES,
                 "match_register for 'es' does not return ES" );
    fail_unless( s, match_register( "fs" ) == FS,
                 "match_register for 'fs' does not return FS" );
    fail_unless( s, match_register( "gs" ) == GS,
                 "match_register for 'gs' does not return GS" );
    fail_unless( s, match_register( "si" ) == SI,
                 "match_register for 'si' does not return SI" );
    fail_unless( s, match_register( "di" ) == DI,
                 "match_register for 'di' does not return DI" );
    fail_unless( s, match_register( "ESI" ) == ESI,
                 "match_register for 'ESI' does not return ESI" );
    fail_unless( s, match_register( "eDI" ) == EDI,
                 "match_register for 'eDI' does not return EDI" );
    fail_unless( s, match_register( "bp" ) == BP,
                 "match_register for 'bp' does not return BP" );
    fail_unless( s, match_register( "ebp" ) == EBP,
                 "match_register for 'ebp' does not return EBP" );
    fail_unless( s, match_register( "sp" ) == SP,
                 "match_register for 'sp' does not return SP" );
    fail_unless( s, match_register( "esp" ) == ESP,
                 "match_register for 'esp' does not return ESP" );
//    fail_unless( s, match_register( "eflags" ) == EFLAGS,
//                 "match_register for 'eflags' does not return EFLAGS" );

    /* also try some that should fail */
    fail_unless( s, match_register( "ds" ) != ES,
                 "match_register for 'ds' did return ES" );
    fail_unless( s, match_register( "eax" ) != AX,
                 "match_register for 'eax' did return AX" );

    fail_unless( s, match_register( "" ) == -1,
                 "match_register for '' did not return -1" );
    fail_unless( s, match_register( " eax" ) == -1,
                 "match_register for ' eax' did not return -1" );
    fail_unless( s, match_register( "gs\n" ) == -1,
                 "match_register for 'gs\\n' did not return -1" );
    fail_unless( s, match_register( "esidi" ) == -1,
                 "match_register for 'esidi' did not return -1" );
    fail_unless( s, match_register( "esp " ) == -1,
                 "match_register for 'esp ' did not return -1" );

    return conclude_suite( s );
}
