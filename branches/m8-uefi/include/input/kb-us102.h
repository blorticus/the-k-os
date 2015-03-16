#ifndef __KB_US102__
#define __KB_US102__


/**
 *
 * Provide the driver for a US102 keyboard.  It provides a callback method that can be
 * used by a hardware scancode producer (generally, an ISR that reads from the keyboard).
 * The callback method places incoming scancodes into a short queue (6 elements, the
 * maximum number of scancodes needed to interpret a key stroke or release).  Incoming
 * scancodes are placed into the short queue until a key stroke or release is identified.
 * At that point, the short queue is cleared, and a normalized scancode is produced.  This
 * normalized scancode is placed in a long queue.  A consumer may read from this queue
 * in order to retrieve those normalized scancodes.
 *
 **/

void kb_us102_init();

void kb_us102_hardware_scancode_callback( u8* scancodes, u8 sc_count );




#endif
