#include <stdio.h>
#include <lcheck.h>
#include <stdlib.h>
#include <string.h>

lcheck_suite* create_suite( const char* name ) {
    lcheck_suite* s = (lcheck_suite*)malloc( sizeof( lcheck_suite ) );

    s->name = (char*)name;
    s->tests_run = 0;
    s->tests_failed = 0;

    return s;
}


void fail_unless( lcheck_suite* s, int result, char* testname ) {
    s->tests_run++;

    if (!result) {
        s->tests_failed++;
        fprintf( stderr, " -- failed test number [%d], named [%s]\n", s->tests_run, testname );
    }
}


int conclude_suite( lcheck_suite* s ) {
    int r;

    r = s->tests_failed;

    fprintf( stderr, " ++ TEST [%s] COMPLETE: Ran [%d] Tests: [%d] Succeeded, [%d] Failed  -- %s -- \n",
                     s->name, s->tests_run, (s->tests_run - s->tests_failed), s->tests_failed, (s->tests_failed ? "NOT OK" : "OK") );
    free( s );

    return r;
}


int __strncmp( const char *s1, const char *s2, unsigned int n ) {
    return strncmp( s1, s2, (size_t)n );
}


char* __strcat_realloc( const char* dest, const char* src ) {
    int dl = strlen( dest );
    int sl = strlen( src  );

    char* temp = (char*)malloc( sizeof( char ) * (dl + sl) );
    char* tp = temp;

    char* dp = (char*)dest;

    while (*dp)
        *tp++ = *dp++;

    while (*src)
        *tp++ = *src++;

    *tp = '\0';

    return temp;
}


int __strlen( const char* s ) {
    return strlen( s );
}
