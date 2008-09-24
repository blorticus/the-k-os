#ifndef __KTERM_H__
#define __KTERM_H__

#include <video/b8000textmode.h>


void kterm_create( void ) {
    textmode_default_init();     
    textmode_cls();
}


void kterm_puts( char* str ) {
    textmode_puts( str );
}


void kterm_fgets( char* buffer, unsigned int size ) {

}


void kterm_cls( void ) {
    textmode_cls();
}


#endif
