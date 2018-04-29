#include <stdio.h>
#include <lcheck.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

lcheck_suite* create_suite( const char* name ) {
    lcheck_suite* s = (lcheck_suite*)malloc( sizeof( lcheck_suite ) );

    s->name = (char*)name;
    s->tests_run = 0;
    s->tests_failed = 0;

    return s;
}


void fail_unless( lcheck_suite* s, int result, const char* testname ) {
    s->tests_run++;

    if (!result) {
        s->tests_failed++;
        fprintf( stderr, " -- failed test number [%d], named [%s]\n", s->tests_run, testname );
    }
}


char fmt_buf[1024];

void fmt_fail_unless( lcheck_suite *s, int result, const char* testname_fmt, ... ) {
    va_list argptr;
    va_start( argptr, testname_fmt );
    vsnprintf( fmt_buf, 1023, testname_fmt, argptr );
    fail_unless( s, result, fmt_buf ); 
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


int __ustrncmp( const char_t *s1, const char_t *s2, unsigned int n ) {
    for ( ; *s1 && *s2 && n > 0; s1++, s2++, n--) {
        if (*s1 < *s2) return -1;
        if (*s1 > *s2) return 1;
    }

    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;

    return 0;
}

int __strcmp( const char *s1, const char *s2 ) {
    return strcmp( s1, s2 );
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

int __sprintf( char* s, const char* fmt, ... ) {
    va_list argptr;
    va_start( argptr, fmt );
    return vsprintf( s, fmt, argptr );
}


void* __memset( void *b, int c, unsigned int len ) {
    return memset( b, c, len );
}
