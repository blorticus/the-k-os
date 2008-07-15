#include <math.h>

unsigned int upow( unsigned int base, unsigned short exp ) {
    unsigned int result = base;

    if (exp == 0)
        return 1;

    while (--exp)
        result *= base;

    return result;
}
