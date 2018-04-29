#ifndef INCLUDE_GATE__LCHECK_H
#define INCLUDE_GATE__LCHECK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int char_t;

typedef struct {
    char*           name;
    unsigned int    tests_run;
    unsigned int    tests_failed;
} lcheck_suite;


lcheck_suite* create_suite   ( const char* name );
void          fail_unless    ( lcheck_suite* s, int result, const char* testname );
void          fmt_fail_unless( lcheck_suite* s, int result, const char* testname_fmt, ... );
int           conclude_suite ( lcheck_suite* s );

int   __strncmp       ( const char* s1, const char* s2, unsigned int n );
int   __ustrncmp      ( const char_t* s1, const char_t* s2, unsigned int n );
int   __strcmp        ( const char* s1, const char* s2 );
char* __strcat_realloc( const char* dest, const char* src );
int   __strlen        ( const char* s1 );
int   __sprintf       ( char* s, const char* fmt, ... );
void* __memset        ( void *b, int c, unsigned int len );

#ifdef __cplusplus
}
#endif



#endif
