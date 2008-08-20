#ifndef __KTERM_H__
#define __KTERM_H__

/* print 'str' to the kernel terminal */
void _puts( char* str ) {}


/* get up to 'size' characters from keyboard, and put value in buffer, null-terminated.  _fgets()
 * will stop after a newline is reached.  'buffer' will not include the newline.  'size' should
 * be no more than the length of 'buffer' minus 1 */
void _fgets( char* buffer, unsigned int size ) {}


/* clear the kernel terminal */
void _cls( void ) {}


#endif
