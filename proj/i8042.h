#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */

#define KEYBOARD_IRQ 1 /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ 12 /**< @brief Mouse IRQ line */

#define FIRST_BYTE_TWO_BYTE_SCANCODE 0xE0
#define BREAK_CODE_BIT BIT(7)

#define ESC_BREAK_CODE 0x81

#define KBC_OUT_BUF 0x60
#define KBC_ARG_REG 0x60
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64

// KBC COMMANDS
#define CMD_READ_BYTE 0x20
#define CMD_WRITE_BYTE 0x60
#define CMD_CHECK_KBC 0xAA
#define CMD_CHECK_IF 0xAB
#define CMD_DISABLE_IF 0xAD
#define CMD_ENABLE_IF 0xAE

// MOUSE COMMANDS
#define CMD_DISABLE_MS 0xA7
#define CMD_ENABLE_MS 0xA8
#define CMD_CHECK_MS_IF 0xA9
#define CMD_WRITE_BYTE_TO_MS 0xD4

// KBC Command byte
#define CMD_BYTE_DISABLE_MS BIT(5)
#define CMD_BYTE_DISABLE_KBD_IF BIT(4)
#define CMD_BYTE_ENABLE_MS_INT BIT(1)
#define CMD_BYTE_ENABLE_KBD_INT BIT(0)

// STATUS REGISTER
#define KBC_ST_IBF BIT(1)
#define KBC_OBF BIT(0)
#define KBC_AUX BIT(5)
#define KBC_PAR_ERR BIT(7)
#define KBC_TO_ERR BIT(6)

// MOUSE PACKETS
#define MS_PACKET_LB BIT(0)
#define MS_PACKET_RB BIT(1)
#define MS_PACKET_MB BIT(2)
#define MS_PACKET_XDELTA_MSB BIT(4)
#define MS_PACKET_YDELTA_MSB BIT(5)
#define MS_PACKET_XOVFL BIT(6)
#define MS_PACKET_YOVFL BIT(7)

#define MS_ENABLE_DATA_REPORTING 0xF4
#define MS_DISABLE_DATA_REPORTING 0xF5
#define MS_READ_DATA 0xEB
#define MS_SET_STREAM_MODE 0xEA

#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

#define DELAY_US 20000

#endif /* _LCOM_I8042_H */
