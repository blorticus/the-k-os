#ifndef __LCHECK_H__
#define __LCHECK_H__

typedef struct {
    char*           name;
    unsigned int    tests_run;
    unsigned int    tests_failed;
} lcheck_suite;


lcheck_suite* create_suite   ( const char* name );
void          fail_unless    ( lcheck_suite* s, int result, char* testname );
void          fmt_fail_unless( lcheck_suite* s, int result, char* testname_fmt, ... );
int           conclude_suite ( lcheck_suite* s );

int   __strncmp       ( const char* s1, const char* s2, unsigned int n );
int   __strcmp        ( const char* s1, const char* s2 );
int   __wcscmp        ( const int* s1, const int* s2 );
char* __strcat_realloc( const char* dest, const char* src );
int   __strlen        ( const char* s1 );
int   __sprintf       ( char* s, const char* fmt, ... );
void* __memset        ( void *b, int c, unsigned int len );
void* __malloc        ( unsigned long size );

#endif
