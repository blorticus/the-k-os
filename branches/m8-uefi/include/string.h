#ifndef __STRING__
#define __STRING__


#define memset( s, c, n )           __builtin_memset( s, c, n )
#define mempcpy( dest, src, n )     __builtin_mempcpy( dest, src, n )


#endif
