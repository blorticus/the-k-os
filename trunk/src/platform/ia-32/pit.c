#include <platform/ia-32/pit.h>

#define PIT_CHANNEL_0_DATA_IO_PORT      0x40
#define PIT_CHANNEL_1_DATA_IO_PORT      0x41
#define PIT_CHANNEL_2_DATA_IO_PORT      0x42
#define PIT_MODE_CMD_REGISTER_IO_PORT   0x43

/* upper two bits of 8 bit command to MCD_REGISTER_IO_PORT */
#define PIT_CMD_SELECT_CH_0                         0x00    // 00 xx xxxx
#define PIT_CMD_SELECT_CH_1                         0x40    // 01 xx xxxx
#define PIT_CMD_SELECT_CH_2                         0x80    // 10 xx xxxx
#define PIT_CMD_READ_BACK_MODE                      0xc0    // 11 xx xxxx

/* next two bits of 8 bit command */
#define PIT_CMD_LATCH_CNT_VALUE_MODE                0x00    // xx 00 xxxx
#define PIT_CND_LOBYTE_ACCESS_MODE                  0x10    // xx 01 xxxx
#define PIT_CMD_HIBYTE_ACCESS_MODE                  0x20    // xx 10 xxxx
#define PIT_CMD_LO_HIBYTE_ACCESS_MODE               0x30    // xx 11 xxxx

/* penultimate three bits of 8 bit command */
#define PIT_OP_MODE_INT_ON_TERMINAL_CNT             0x00    // xxxx 000 x
#define PIT_OP_MODE_HARDWARE_RETRIGABLE_ONE_SHOT    0x02    // xxxx 001 x
#define PIT_OP_MODE_RATE_GENERATOR                  0x04    // xxxx 010 x
#define PIT_OP_MODE_SQUARE_WAVE_GENERATOR           0x06    // xxxx 011 x
#define PIT_OP_MODE_SOFTWARE_TRIGGERED_STROBE       0x08    // xxxx 100 x
#define PIT_OP_MODE_HARDWARE_TRIGGERED_STROBE       0x0a    // xxxx 101 x
#define PIT_OP_MODE_RATE_GENERATOR_2                0x0c    // xxxx 110 x
#define PIT_OP_MODE_SQUARE_WAVE_GENERATOR_2         0x0d    // xxxx 111 x

/* final bit of 8 bit command */
#define PIT_BCD_MODE_16_BIT                         0x00    // xxxx xxx 0
#define PIT_BCD_MODE_FOUR_DIGIT                     0x01    // xxxx xxx 1


