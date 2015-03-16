#include <input/kb-us102.h>
#include <sys/types.h>

static u8 hw_scancode_to_key[] = {
//  0       1       2       3       4       5       6       7       8       9       a       b       c       d       e       f
// ---     ---     ---     ---     ---     ---     ---     ---     ---     ---     ---     ---     ---     ---     ---     ---
    0,    'ESC',   '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    '-',    '=',    '\b',   '\t',    // 0x
   'Q',    'W',    'E',    'R',    'T',    'Y',    'U',    'I',    'O',    'P',    '[',    ']',    '\n', 'L CTRL', 'A',    'S',     // 1x
   'D',    'F',    'G',    'H',    'J',    'K',    'L',    ';',    '\'',   '`', 'L SHFT',  '\\',   'Z',    'X',    'C',    'V',     // 2x
   'B',    'N',    'M',    ',',    '.',    '/', 'R SHFT', 'KP *', 'L ALT', ' ', 'CAPS',    'F1',   'F2',a  'F3',   'F4',   'F5',    // 3x
   'F6',   'F7',   'F8',   'F9',   'F10',  'NUM','SCROLL', 'KP 7', 'KP 8', 'KP 9', 'KP -', 'KP 4', 'KP 5', 'KP 6', 'KP +', 'KP 1',  // 4x
   'KP 2', 'KP 3', 'KP 0', 'KP .', 'F11',  'F12'                                                                                    // 5x
};


void hardware_scancode_callback( u8* sclist, u8 sc_count ) {
    int i;

    for (i = 0; i < sc_count; i++) {
        
    }
}
