#ifndef __MATH_H__
#define __MATH_H__

/**
 * BRIEF:           Basic math functions
 * BACKGROUND:      These are implementations of basic math functions.  Since this is a "learning" OS project,
 *                  we implement these ourselves for now, but later, we'll use the code of others who are smarter
 *                  and wiser in the ways of the world.
 * RE-ENTRANT?:     PER-METHOD
 ***/


/**
 *
 * DESCRIPTION:     raise 'base' by 'exp'.  Equivalent of base^exp
 * RETURN:          base raised to exp
 * SIDE-EFFECTS:    none
 * NOTES:           no attempt to account for overflow.  If base ^ exp > 2^32 - 1, return value will overflow.
 * RE-ENTRANT?:     yes
 */ 
unsigned int upow( unsigned int base, unsigned short exp );


#endif
