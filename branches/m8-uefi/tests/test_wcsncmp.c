#include <lcheck.h>
#include <string.h>
#include <sys/types.h>

int main( void ) {
    lcheck_suite* s;

    s = create_suite( "wcsncmp" );

    fail_unless( s, wcsncmp( (const wchar_t*)L"", (const wchar_t*)L"", 10 ) == 0, "Two empty strings are equal" );
    fail_unless( s, wcsncmp( (const wchar_t*)L"abc", (const wchar_t*)L"abcd", 5 ) < 0, "wcsncmp( 'abc', 'abcd', 5 ) < 0" );
    fail_unless( s, wcsncmp( (const wchar_t*)L"abc", (const wchar_t*)L"abcd", 3 ) == 0, "wcsncmp( 'abc', 'abcd', 3 ) == 0" );
    fail_unless( s, wcsncmp( (const wchar_t*)L"Abc", (const wchar_t*)L"abcd", 5 ) < 0, "wcsncmp( 'Abc', 'abcd', 5 ) < 0" );
    fail_unless( s, wcsncmp( (const wchar_t*)L"abc", (const wchar_t*)L"Abcd", 5 ) > 0, "wcsncmp( 'abc', 'Abcd', 5 ) > 0" );
    fail_unless( s, wcsncmp( (const wchar_t*)L"abc", (const wchar_t*)L"Abcd", 5 ) > 0, "wcsncmp( 'abc', 'Abcd', 5 ) > 0" );

    return conclude_suite( s );
}

