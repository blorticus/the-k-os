#ifndef __LCHECK_H__
#define __LCHECK_H__

typedef struct {
    char*           name;
    unsigned int    tests_run;
    unsigned int    tests_failed;
} lcheck_suite;


lcheck_suite* create_suite  ( const char* name );
void          fail_unless   ( lcheck_suite* s, int result, char* testname );
int           conclude_suite( lcheck_suite* s );

#endif
