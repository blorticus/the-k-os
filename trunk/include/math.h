#ifndef __MATH_H__
#define __MATH_H__

/**
 * BRIEF:           Basic math functions
 * BACKGROUND:      These are implementations of basic math functions.  Since this is a "learning" OS project,
 *                  we implement these ourselves for now, but later, we'll use the code of others who are smarter
 *                  and wiser in the ways of the world.
 * RE-ENTRANT?:     PER-METHOD
 ***/


/* Raises base to 'exp' power, returning the result.  May overflow without error */
unsigned int upow( unsigned int base, unsigned short exp );


#endif
