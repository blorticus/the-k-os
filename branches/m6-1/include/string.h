#ifndef __STRING_H__
#define __STRING_H__

#include <sys/types.h>


/**
 *
 * DESCRIPTION:     determine the length of a string (the count of characters in the string up to but not including the NULL)
 * RETURN:          length of string s1
 * SIDE-EFFECTS:    none
 * NOTES:           none
 * RE-ENTRANT?:     yes
 *
 */ 
unsigned int strlen( const char *s1 );


/**
 *
 * DESCRIPTION:     compare two null-terminated strings
 * RETURN:          -1 if s1 is lexically "greater than" s2; 0 if they are lexically identical; 1 if s2 is lexically "greater than" s2
 * SIDE-EFFECTS:    none
 * NOTES:           strncmp and strcmp are the same, except that strncmp accepts a size.  It will not compare more than 'n' characters of data.  strncmp should
 *                  be used except in cases where both strings are certainly known to be null-terminated.
 * RE-ENTRANT?:     yes
 *
 */ 
int strcmp( const char *s1, const char *s2 );
int strncmp( const char *s1, const char *s2, size_t n );

/**
 *
 * DESCRIPTION:     convert string to all lower-case or all upper-case, but attempt to alter no more than 'n' characters (stopping at NULL if before 'n' chars)
 * RETURN:          the number of characters in the soure string for which case was changed
 * SIDE-EFFECTS:    original string is altered in-place
 * NOTES:           none
 * RE-ENTRANT?:     yes, if source string is not modified by something else at the same time
 *
 */ 
int strntolower( char *s, size_t n );
int strntoupper( char *s, size_t n );


/**
 *
 * DESCRIPTION:     copy the value 'value' (converted to unsigned 8-bit integer) to 'destination' 'len' times, advancing 'destination' pointer on each write
 * RETURN:          pointer to 'destination'
 * SIDE-EFFECTS:    none
 * RE-ENTRANT?:     yes
 *
 */
void* memset( void *destination, int value, size_t len );



#endif
